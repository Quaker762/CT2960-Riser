/*
 * Program to demonstrate mixing of two VOC files, pitch shifting one of them,
 * and playing them using low-level programming.
 *
 * Copyright (c) 1993-96 Creative Labs, Inc.
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * You have a royalty-free right to use, modify, reproduce and
 * distribute the Sample Files (and/or any modified version) in
 * any way you find useful, provided that you agree that
 * Creative has no warranty obligations or liability for any Sample Files.
 *
 * This program is for demonstration purposes only.
 *
 * D. Kaden 8/4/93
 *
 * Modified 1/31/94
 * Got rid of PLAYMIX1 and PLAYMIX2, made PLAYMIX.  Made it take file names
 * from the command line.
 *
 */



#define AUTOINIT      /* Define this for auto-initialized mode (no clicks).
                       * Undefine it for non-auto-init mode, but it will
                       * click slightly.  Auto-init is only available on
                       * cards with DSP versions of 2 or above.
                       */



#include <stdio.h>
#include <dos.h>
#include <stdlib.h>
#include <conio.h>
#include <dir.h>
#include <string.h>
#include "mix.h"


#define SPACEKEY ' '
#define ESC 27
#define LEFTKEY 75        /* really 0, 75 */
#define RIGHTKEY 77
#define F1KEY 59
#define F2KEY 60
#define F3KEY 61
#define F4KEY 62

#define dspReady 0xAA     /* byte returned by DSP when ready after reset */


/***
 *** WARNING:  THESE CONSTANTS DEFINE HOW THE PROGRAM ASSUMES THE BOARD TO
 *** BE CONFIGURED.  MAKE SURE THESE MATCH YOUR BOARD BEFORE YOU RUN THIS
 *** PROGRAM.  DON'T USE IRQ 10, THOUGH, BECAUSE THIS PROGRAM ISN'T DESIGNED
 *** FOR IT (it uses the other 8259 PIC).
 ***/

#define SBADDR     0x220        /* Sound Blaster base port address */
#define SBIRQNUM   5            /* Sound Blaster IRQ number */
#define SBDMACHAN  1            /* Sound Blaster DMA channel */


#define IRQ (SBIRQNUM+8)        /* interrupt number corresponding to IRQ number */
#define IRQmask (1 << SBIRQNUM) /* interrupt mask corresponding to IRQ number */


/* port addresses for Sound Blaster */
#define dspaddrWrBuf     (SBADDR+0xC)    /* DSP Write Buffer address */
#define dspaddrReadData  (SBADDR+0xA)    /* DSP Read Data address */
#define dspaddrDataAvail (SBADDR+0xE)    /* DSP Data Available address */
#define mixeraddr        (SBADDR+0x4)    /* mixer address (register select) port */
#define mixerdata        (SBADDR+0x5)    /* mixer data port */
#define dspaddrReset     (SBADDR+0x6)    /* dsp reset port address */


/* port addresses for 8237 DMA controller and its page registers */
#define DMACMaskReg 0xA
#define DMACModeReg 0xB
#define DMACFFReg   0xC
#define DMA1PAGE    0x83
#define DMA1ADDR    0x2
#define DMA1COUNT   0x3
#define DMA3PAGE    0x82
#define DMA3ADDR    0x6
#define DMA3COUNT   0x7


#if SBDMACHAN==1
   #define DMAPAGE  DMA1PAGE
   #define DMAADDR  DMA1ADDR
   #define DMACOUNT DMA1COUNT
#elif SBDMACHAN==3
   #define DMAPAGE  DMA3PAGE
   #define DMAADDR  DMA3ADDR
   #define DMACOUNT DMA3COUNT
#else
   ERROR: No constants defined for DMA channel.
#endif


/* constants for 8250 Peripheral Interrupt Controller */
#define PICMODE     0x20
#define PICMASK     0x21
#define PICEOI      0x20


/* some Sound Blaster commands */
#define dspcmdDirect8DAC   0x10    /* DSP Direct 8-bit DAC command */
#define dspcmdDMADAC       0x14    /* DSP 8-bit DAC DMA transfer */
#define dspcmdDirectADC    0x20    /* DSP Direct ADC command */
#define dspcmdDMAADC       0x24    /* DSP 8-bit ADC DMA transfer */
#define dspcmdTimeConst    0x40    /* set time constant for DMA transfer */
#define dspcmdBlockSize    0x48    /* DMA transfer block size */
#define dspcmdHaltDMA      0xD0    /* stop DMA */
#define dspcmdContDMA      0xD4    /* continue DMA */
#define dspcmdSpeakerOn    0xD1    /* turn speaker on */
#define dspcmdSpeakerOff   0xD3    /* turn speaker off */
#define dspcmdSpeakerStat  0xD8    /* get speaker status */

#define dspcmdAUTODMADAC   0x1C    /* DSP 8-bit ADC Auto-init DMA transfer */
#define dspcmdAUTODMAADC   0x2C    /* DSP 8-bit ADC Auto-init DMA transfer */
#define dspcmdHaltAuto     0xDA    /* halt auto-init DMA transfer */


/* Sound Blaster mixer registers */
#define MASTERVOL 0x22
#define VOCVOL    0x04
#define FMVOL     0x26
#define CDVOL     0x28
#define MICVOL    0x0A
#define ADCSELECT 0x0C



#define BUFSIZE 1024*1

unsigned char buf0[BUFSIZE], buf1[BUFSIZE];


unsigned int page0, page1, offset0, offset1, count=BUFSIZE-1, outtc=166 /* 11K */;

int BusyBuf, BufToFill;

VOCfile v1,v2;

char filename1[MAXPATH], filename2[MAXPATH];


boolean v2pitchshift=false;
boolean v1repeat=true;
boolean v2repeat=true;
unsigned int v2skip=20;



void dspout(unsigned int val)
/* Outputs a byte to the dsp.  This polls the write buffer status bit, waits
 * for it to be 0, then outputs the value to the write buffer.
 * The C equivalent code is:
 *    while (inp(dspaddrWrBuf) & 0x80)
 *        ;
 *    outp(dspaddrWrBuf,val);
 */
{
   asm {
      mov dx,dspaddrWrBuf
   }
   test:
   asm {
      in al,dx
      or al,al
      js test
      mov al,byte ptr val
      out dx,al
   }
}


void dspin(unsigned int *val)
/* Read a value from the dsp.  Poll the Data Available bit until it is 1,
 * then read the value from the read data port.
 */
{
   while (!(inp(dspaddrDataAvail) & 0x80)) {
      ;
   }
   *val=inp(dspaddrReadData);
}



void SegToPhys(unsigned char far *ptr, unsigned long size, unsigned long *physaddr, unsigned long *endaddr)
/* Converts a segmented address to a physical memory address.  Also gives
 * the end address ('size' bytes past ptr).
 */
{
   *physaddr=(unsigned long)(((unsigned long)FP_SEG(ptr) << 4) + FP_OFF(ptr));
   *endaddr = *physaddr+size-1;
}



unsigned int PhysToPage(unsigned long physaddr, unsigned long endaddr, unsigned int *page, unsigned int *offset)
/* Converts a physical memory address to a page/offset type of address for
 * DMA (NOT the same as a segmented address).  It returns the number of bytes
 * between physaddr and endaddr (the DMA buffer) that are in the DMA page.
 * If the return value is less than the buffer size (endaddr-physaddr+1),
 * the DMA buffer is crossing a page boundary and must be discarded.  In that
 * case, allocate some more memory and try again.
 */
{
   unsigned int remaining;
   *page=physaddr >> 16;
   *offset=physaddr & 0xFFFF;

   remaining=0xFFFF - *offset + 1;   /* bytes remaining in page */
   if (remaining>endaddr-physaddr)   /* if more bytes left in page than in buffer */
      return endaddr-physaddr+1;     /* return number of bytes in buffer */
   else return remaining;            /* else return number of bytes left in page */
}




void SetupOutputDMA(unsigned int page, unsigned int ofs, int count,
 unsigned char tc)
/* This programs the Sound Blaster board and the DMA controller to play
 * from a buffer.
 *
 */
{
/**** There MUST be parens around the values to be output, or the compiler
 **** will convert them to unsigned chars BEFORE it does the operations
 **** on them.
 ****/
#ifdef AUTOINIT
  /* In this mode, the DMAC and DSP are only programmed once.  The DSP is
   * programmed for a block size half of what the DMAC is programmed for,
   * so interrupts are generated every half-buffer.  In this program, buf0
   * and buf1 are both half buffers.  The DMAC will be programmed to trans-
   * fer 2*BUFSIZE, while the DSP is programmed for just BUFSIZE.
   */
   outp(DMACMaskReg,4 | SBDMACHAN);     /* mask off dma channel */
   outp(DMACFFReg,0);                   /* clear flip-flop */
/* This next value to DMAC is different between auto-init and non-auto-init.
 * (0x58 vs. 0x48) */
   outp(DMACModeReg,0x58 | SBDMACHAN);  /* set mode for DAC, channel 1 */
   outp(DMAADDR,(ofs & 0xFF));          /* low byte base addr */
   outp(DMAADDR,(ofs >> 8));            /* high byte base addr */
   outp(DMAPAGE,page);                  /* physical page number */
   outp(DMACOUNT,((count-1) & 0xFF));   /* count low byte (ENTIRE buffer) */
   outp(DMACOUNT,((count-1) >> 8));     /* count high byte */
   outp(DMACMaskReg, SBDMACHAN);        /* enable dma channel 1 */

   dspout(dspcmdTimeConst);    /* time constant command */
   dspout(tc);                 /* the time constant (for sample rate) */
   dspout(dspcmdBlockSize);    /* set block size= HALF DMA buffer size */
   dspout((count/2-1) & 0xFF);
   dspout((count/2-1) >> 8);
   dspout(dspcmdAUTODMADAC);   /* program dsp for auto-init dma mode dac */
#else
   outp(DMACMaskReg,4 | SBDMACHAN);     /* mask off dma channel */
   outp(DMACFFReg,0);                   /* clear flip-flop */
   outp(DMACModeReg,0x48 | SBDMACHAN);  /* set mode for DAC, channel 1 */
   outp(DMAADDR,(ofs & 0xFF));          /* low byte base addr */
   outp(DMAADDR,(ofs >> 8));            /* high byte base addr */
   outp(DMAPAGE,page);                  /* physical page number */
   outp(DMACOUNT,(count & 0xFF));       /* count low byte */
   outp(DMACOUNT,(count >> 8));         /* count high byte */
   outp(DMACMaskReg, SBDMACHAN);        /* enable dma channel 1 */

   dspout(dspcmdTimeConst);    /* time constant command */
   dspout(tc);                 /* the time constant (for sample rate) */
   dspout(dspcmdDMADAC);       /* program dsp for dma mode dac */
   dspout(count & 0xFF);       /* block size low byte */
   dspout(count >> 8);         /* high byte */
#endif
}



void SetMixer(void)
/* Resets the Sound Blaster card and sets the volume levels in the SB-PRO
 * mixer chip.
 */
{

   unsigned int val;

   outp(dspaddrReset,1);       /* reset DSP */
   delay(1);
   outp(dspaddrReset,0);
   dspin(&val);
   if (val!=dspReady)
      printf("SBPRO not ready.");

   outp(mixeraddr,MICVOL);
   outp(mixerdata,0);          /* turn mic vol off */
   outp(mixeraddr,VOCVOL);
   outp(mixerdata,0xFF);       /* both chans up */
   outp(mixeraddr,MASTERVOL);
   outp(mixerdata,0xFF);       /* master up */
   dspout(0xD1);               /* turn speaker on*/
}



void interrupt OutputDMAend(...)
/* This is the playback interrupt service routine.  It acknowledges the
 * interrupt, reprograms the sound blaster card and the DMA controller for
 * another buffer transfer, and sends and End Of Interrupt to the interrupt
 * controller.
 */
{
   inp(dspaddrDataAvail);  /* acknowledge interrupt */

   BusyBuf ^= 1;  /* toggle BusyBuf because other buffer is now busy */
#ifdef AUTOINIT
   /* no re-programming necessary. */
#else
   if (BusyBuf) {
      SetupOutputDMA(page1,offset1,count,outtc);
   } else {
      SetupOutputDMA(page0,offset0,count,outtc);
   }
#endif

   outp(PICMODE,PICEOI);    /* end of interrupt */
}




void FillBuffer(void)
/* Fills the buffer specified to BufToFill with data from the VOC file. */
{
   unsigned char *ptr;
   unsigned int i;

   if (BufToFill)
      ptr=buf1;
   else ptr=buf0;

   for (i=0; i<BUFSIZE; i++)
      *ptr++ = (v1.GetSample() + v2.GetSample()) >> 1;

   BufToFill ^= 1;  /* toggle so BufToFill specifies other one */
}



void PrintShiftStat(void)
{
   gotoxy(1,14);
   printf("File 2 pitch shifting: %3s",v2pitchshift ? "ON":"OFF");
}


void NewShift(int skip)
{
   v2.SetShiftAmount(skip);
   gotoxy(1,15);
   printf("skip amount: %7u\r",skip);
}


void SetFile1Repeat(boolean repeat)
{
   v1.SetRepeat(v1repeat=repeat);
   gotoxy(1,11);
   printf("File 1 repeat: %3s",v1repeat ? "ON" : "OFF");
}


void SetFile2Repeat(boolean repeat)
{
   v2.SetRepeat(v2repeat=repeat);
   gotoxy(1,12);
   printf("File 2 repeat: %3s",v2repeat ? "ON" : "OFF");
}


void SetupScreen(void)
{
   clrscr();
   printf("VOC file mixing and pitch shift sample program.\n\n");
   printf("File 1: %s\n",filename1);
   printf("File 2: %s\n\n",filename2);
   printf("F1 - Toggle file 1 repeat             F3 - Reset file 1\n");
   printf("F2 - Toggle file 2 repeat             F4 - Reset file 2\n");
   printf("SPACE - Toggle file 2 pitch shift     L/R arrows - shift pitch\n");
   printf("ESC to quit.\n\n");
}


void appendvoc(char *fname)
/* Appends the .voc extension to a filename, if necessary. */
{
   char drive[MAXDRIVE], dir[MAXDIR], name[MAXFILE], ext[MAXEXT];

   fnsplit(fname, drive, dir, name, ext);  /* split fname into components */

   if (strlen(ext)==0)                     /* if there's no extension */
      strcpy(ext,".voc");                  /* add .voc */

   fnmerge(fname, drive, dir, name, ext);  /* recombine components */
}



void main(int argc, char *argv[])
{
   int i,c,err;
   void interrupt (*IRQsave)(...);
   unsigned long physaddr,endaddr;
   unsigned int masksave;
   boolean done=false;

   SegToPhys(buf0,BUFSIZE,&physaddr,&endaddr);
   if (BUFSIZE > PhysToPage(physaddr,endaddr,&page0,&offset0)) {
      printf("Page error.\n");
      exit(1);
      /* If this happens, you should dynamically allocate memory instead,
       * and don't use any blocks of memory that fail the above test.
       */
   }

   SegToPhys(buf1,BUFSIZE,&physaddr,&endaddr);
   if (BUFSIZE > PhysToPage(physaddr,endaddr,&page1,&offset1)) {
      printf("page error\n");
      exit(1);
      /* See comment above. */
   }

   clrscr();
   if (argc > 1)
      strcpy(filename1, argv[1]);
   else {
      printf("\n.voc file 1 name: ");
      scanf("%s",filename1);
   }
   appendvoc(filename1);

   if (argc > 2)
      strcpy(filename2, argv[2]);
   else {
      printf(".voc file 2 name: ");
      scanf("%s",filename2);
   }
   appendvoc(filename2);

   if (0 != (err=v1.open(filename1))) {
      printf("Error %d opening %s\n",err,filename1);
      exit(1);
   }

   if (0 != (err=v2.open(filename2))) {
      printf("Error %d opening %s\n",err,filename2);
      exit(1);
   }


   v2.SetPitchShift(v2pitchshift);

   SetupScreen();
   PrintShiftStat();
   SetFile1Repeat(true);
   SetFile2Repeat(true);


   for (i=0; i<BUFSIZE; i++) {
      buf0[i]=0;    /* Clear the buffers.  Not necessary, but good idea */
      buf1[i]=0;    /* in case something goes wrong. */
   }


   SetMixer();

   IRQsave=getvect(IRQ);              /* save the old interrupt vector */
   setvect(IRQ,OutputDMAend);         /* point to new ISR */

   BufToFill=0;
   FillBuffer();                      /* fill buffer 0 */
   BusyBuf=0;                         /* buffer 0 will start playing soon */

   masksave=inp(PICMASK);                 /* save mask to restore later */
   outp(PICMASK,(masksave & ~IRQmask));   /* enable interrupt */

#ifdef AUTOINIT
   SetupOutputDMA(page0,offset0,2*BUFSIZE,outtc); /* this will start buf0 playing */
#else
   SetupOutputDMA(page0,offset0,count,outtc);     /* this will start buf0 playing */
#endif

   do {
      while (!done && BufToFill==BusyBuf) {
         /* mainly wait until it's OK to refill BusyBuf, but check keyboard
          * in the mean time */
         if (kbhit()) {
            c=getch();
            switch (c) {
               case 0:
                  c=getch();
                  switch (c) {
                     case LEFTKEY:
                        v2skip += 1;
                        NewShift(v2skip);
                        break;
                     case RIGHTKEY:
                        if (v2skip>1)  /* don't let go to zero */
                           v2skip -= 1;
                        NewShift(v2skip);
                        break;
                     case F1KEY:
                        SetFile1Repeat((boolean)!v1repeat);
                        break;
                     case F2KEY:
                        SetFile2Repeat((boolean)!v2repeat);
                        break;
                     case F3KEY:
                        v1.ResetVOC();
                        break;
                     case F4KEY:
                        v2.ResetVOC();
                        break;
                  }
                  break;
               case SPACEKEY:
                  v2pitchshift= (boolean) !v2pitchshift;
                  v2.SetPitchShift(v2pitchshift);
                  PrintShiftStat();
                  break;
               case ESC:
                  done=true;
                  break;
            }
         }
      }
      FillBuffer();  /* refill the next buffer */
   } while (!done);


#ifdef AUTOINIT
   dspout(dspcmdHaltAuto);        /* stop auto-init DMA transfer */
#else
   dspout(dspcmdHaltDMA);         /* stop DMA transfer */
#endif


   outp(PICMASK, masksave);       /* restore interrupt mask */

   setvect(IRQ,IRQsave);          /* restore old vector */
}

