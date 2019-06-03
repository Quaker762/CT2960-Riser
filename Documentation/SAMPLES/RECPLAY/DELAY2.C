/****************************************************************************
*                                                                           *
*  (C) Copyright Creative Technology Ltd. 1993-96. All rights reserved.     *
*                                                                           *
*  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY    *
*  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE      *
*  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR    *
*  PURPOSE.                                                                 *
*                                                                           *
****************************************************************************/

/*
 * DELAY2.C
 *
 * D. Kaden 6/23/94
 *
 * This program is for demonstration purposes only.
 *
 * Derived from DELAY.C.
 *
 * This program records and plays simultaneously using ONE Sound Blaster 16
 * card.
 *
 * Before running this program, make sure the microphone recording level is
 * turned up, and make sure the microphone is not selected for output in the
 * mixer.
 *
 * For this program to work, you must have a working 16-bit DMA channel.
 * (Your "D" and "H" numbers in you BLASTER environment variable must be
 * different.)
 *
 * This program does a 16-bit recording to disk, then after about 1 second
 * starts an 8-bit playback from the same file.  The recording format is
 * 16-bit signed.  For playback, the low byte of each sample is discarded to
 * convert it to 8-bit signed.  (Note that the usual format for an 8-bit
 * recording is UNsigned.  I programmed it for 8-bit signed for avoid the
 * unsigned to signed conversion.)
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <dos.h>
#include <ctype.h>
#include <mem.h>
#include <fcntl.h>
#include "mydefs.h"
#include "sbdefs.h"



#define debug

#define FILENAME "delay2.raw"


// Structure for storing data associated with each SB card.
typedef struct {
   int base,                         // SB card base I/O address
    MixerAddr, MixerData,            // mixer chip addresses
    Reset, WrBuf,                    // DSP addresses
    DataAvail, ReadData,             // DSP addresses
    irq,                             // IRQ number
    dma8,                            // 8-bit DMA channel
    dma16,                           // 16-bit DMA channel
    midi,                            // midi port
    cardtype;                        // card type number from environment
    void interrupt (*intvecsave)();  // previous interrupt vector
    int intrnum;                     // interrupt number
    int intrmask;                    // interrupt mask
   } cardinfo;


// Structure for retrieving DMA controller register port addresses.
typedef struct {
   unsigned char addr, count, page, mask, mode, FF;
   } dmaportstruct;


// Constant array that defines port addresses for DMA controller registers.
const dmaportstruct dmaportarray[8] = {
    {0x0,  0x1,  0x87, 0xA,  0xB,  0xC},   // chan 0
    {0x2,  0x3,  0x83, 0xA,  0xB,  0xC},   // chan 1
    {0x4,  0x5,  0x81, 0xA,  0xB,  0xC},   // chan 2  DON'T USE
    {0x6,  0x7,  0x82, 0xA,  0xB,  0xC},   // chan 3
    {0x0,  0x0,  0x00, 0xD4, 0xD6, 0xD8},  // chan 4  DON'T USE
    {0xC4, 0xC6, 0x8B, 0xD4, 0xD6, 0xD8},  // chan 5
    {0xC8, 0xCA, 0x89, 0xD4, 0xD6, 0xD8},  // chan 6
    {0xCC, 0xCE, 0x8A, 0xD4, 0xD6, 0xD8}   // chan 7
   };




#define BUFSIZE (8*1024)      // size of the DMA buffer
#define NUMBUFS 2
#define TIMECONST 165         // 165 = approx 11.5 KHz sample rate


cardinfo      card1;

              // It would be better to dynamically allocate the DMA buffers
              // to guarantee that they don't cross a DMA page boundary.
              // This is quick and dirty, but it usually works.
unsigned int  recbuf[BUFSIZE];         // recording DMA buffer
unsigned char playbuf[BUFSIZE];        // playback DMA buffer

unsigned int  inpage,   outpage,       // DMA pages for input and output buffers
              inoffset, outoffset,     // offsets for DMA input and output buffers
              count = BUFSIZE;         // size in words of recording DMA buffer,
                                       // and size in bytes of playback buffer

int           *inbuffers[NUMBUFS];     // Input (recording) buffers to
                                       //  transfer data from DMA buffer
                                       //  to disk.
int           *outbuffers[NUMBUFS];    // Output (playback) buffers to
                                       //  transfer data from disk to DMA
                                       //  buffer.

int           NumFilledInBufs   = 0,   // number of filled input buffers
              NumFilledOutBufs  = 0,   // number of filled output buffers
              NumRecBufToFill   = 0,   // next input buffer to fill from DMA buffer
              NumRecBufToEmpty  = 0,   // next input buffer to copy to disk
              NumPlayBufToFill  = 0,   // next output buffer to fill from disk
              NumPlayBufToEmpty = 0,   // next output buffer to copy to DMA buffer
              HalfBufToEmpty    = 0,   // which half of input DMA buffer to empty
              HalfBufToFill     = 0,   // which half of output DMA buffer to fill
              NumBufsWritten    = 0;   // number of buffers written to disk

#ifdef debug
unsigned int  recintcount       = 0;
unsigned int  playintcount      = 0;
#endif


int base16(char **str, int *val)
/* Takes a double pointer to a string, interprets the characters as a
 * base-16 number, and advances the pointer.
 * Returns 0 if successful, 1 if not.
 */
{
   char c;
   int digit;
   *val = 0;

   while ((**str != ' ') && (**str != '\0')) {
      c = toupper(**str);
      if (c >= '0' && c <= '9')
         digit = c - '0';
      else if (c >= 'A' && c <= 'F')
         digit = c - 'A'  + 10;
      else
         return 1;          // error in string

      *val = *val * 16 + digit;
      (*str)++;
   }
   return 0;
}



int base10(char **str, int *val)
/* Takes a double pointer to a string, interprets the characters as a
 * base-10 number, and advances the pointer.
 * Returns 0 if successful, 1 if not.
 */
{
   char c;
   int digit;
   *val = 0;

   while ((**str != ' ') && (**str != '\0')) {
      c = toupper(**str);
      if (c >= '0' && c <= '9')
         digit = c - '0';
      else
         return 1;          // error in string

      *val = *val * 10 + digit;
      (*str)++;
   }
   return 0;
}



int ReadBlasterEnv(char *name, int *port, int *irq, int *dma8, int *dma16,
 int *midi, int *cardtype)
/* Gets the Blaster environment statement and stores the values in the
 * variables whose addresses were passed to it.
 *
 * Input:
 *   name  - environment name to get, usually "BLASTER"
 *
 * Returns:
 *   0  if successful
 *   1  if there was an error reading the port address.
 *   2  if there was an error reading the IRQ number.
 *   3  if there was an error reading the 8-bit DMA channel.
 *   4  if there was an error reading the 16-bit DMA channel.
 *   5  if there was an error reading the MIDI address.
 *   6  if there was an error reading the card type number.
 */
{
   char     *env;
   unsigned val;
   int      digit;

   env = getenv(name);

   if (!env)
      return 7;   // if there was no environment entry for name.

   while (*env) {
      switch(toupper( *(env++) )) {
         case 'A':
            if (base16(&env, port))
               return 1;
            break;
         case 'I':
            if (base10(&env, irq))
               return 2;
            break;
         case 'D':
            if (base10(&env, dma8))
               return 3;
            break;
         case 'H':
            if (base10(&env, dma16))
               return 4;
            break;
         case 'P':
            if (base16(&env, midi))
               return 5;
            break;
         case 'T':
            if (base10(&env, cardtype))
               return 6;
            break;
         default:
            break;
      }
   }

   return 0;
}



void SegToPhys(unsigned char far *ptr, unsigned long size, unsigned long *physaddr, unsigned long *endaddr)
// Converts a segmented address to a physical memory address.
{
   *physaddr=(unsigned long)(((unsigned long)FP_SEG(ptr) << 4) + FP_OFF(ptr));
   *endaddr = *physaddr+size-1;
}



unsigned int PhysToPage(unsigned long physaddr, unsigned long endaddr,
 unsigned int *page, unsigned int *offset)
/* Converts a physical memory address to a DMA-page address.  Returns the
 * number of bytes in this block of memory that aren't in a different DMA
 * page.
 *  INPUT:
 *   physaddr - a physical (not segmented) address specifying the start
 *              of the DMA buffer.
 *   endaddr  - a physical address specifying the last byte of the DMA
 *              buffer.
 *  OUTPUT:
 *   page     - the DMA page of the buffer.
 *   offset   - the offset of the buffer.
 *  RETURN VALUE:
 *   The minimum of (endaddr-physaddr+1, number of bytes in this page
 *   starting at physaddr).
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



void dspout(cardinfo *card, unsigned int val)
// Output a byte to the Sound Blaster Digital Sound Processor.
{
   while (inp(card->WrBuf) & 0x80)
      ;
   outp(card->WrBuf, val);
}



boolean dspin(cardinfo *card, unsigned int *val)
// Read a byte from the Digital Sound Processor.
{
   while (!(inp(card->DataAvail) & 0x80)) {
      ;
   }
   *val=inp(card->ReadData);

   return true;    // used to be meaningful
}



void SetupDMA(int dmachan, unsigned short page, unsigned short ofs,
 unsigned short DMAcount, unsigned char dmacmd)
// This programs the DMA controller.
// NOTE:  This function supports 8-bit DMA channels ONLY (because this pro-
// gram only uses 8-bit voice mode.  The SetupDMA function in the program
// DELAY2.C supports both 8- and 16-bit DMA channels.
{
   dmaportstruct dmaports;

/**** There MUST be parens around the values to be output, or the compiler
 **** will convert them to unsigned chars BEFORE it does the operations
 **** on them.
 ****/
   dmaports = dmaportarray[dmachan];

   if (dmachan > 3) {
      dmachan -= 4;     // make dmachan local to DMAC
      ofs = (ofs >> 1) + ((page & 1) << 15);
   }

   outp(dmaports.mask, 4 | dmachan);            // mask off dma channel
   outp(dmaports.FF, 0);                        // clear flip-flop
/* This next value to DMAC is different between auto-init and non-auto-init.
 * (0x58 vs. 0x48) */
   outp(dmaports.mode , dmacmd | dmachan);      // set mode for DAC
   outp(dmaports.addr, (ofs & 0xFF));           // low byte base addr
   outp(dmaports.addr, (ofs >> 8));             // high byte base addr
   outp(dmaports.page, page);                   // physical page number
   outp(dmaports.count,((DMAcount-1) & 0xFF));  // count low byte
   outp(dmaports.count,((DMAcount-1) >> 8));    // count high byte
   outp(dmaports.mask, dmachan);                // enable dma channel
}



void SetupDSP(cardinfo *card, unsigned char dspcmd, unsigned short mode,
unsigned short DSPcount, unsigned char tc)
// Programs the Sound Blaster card.
{
   // Program the time constant.
   dspout(card, dspcmdTimeConst);
   dspout(card, tc);

   // Program the card for playback or record using SB16 command format.
   dspout(card, dspcmd);
   dspout(card, mode);
   dspout(card, (DSPcount-1) & 0xFF);
   dspout(card, (DSPcount-1) >> 8);
}



void SetupInputDMA(unsigned int page, unsigned int ofs, int count,
 unsigned char tc)
// Program the DMA Controller and Sound Blaster for DMA input.
{
   SetupDMA(card1.dma16, page, ofs, count, DMAMODEWRITE);
   // Program card for 16-bit signed input.
   SetupDSP(&card1,
    dspSB16DMA16 | dspSB16ADC | dspSB16AI | dspSB16FifoOn,
    dspSB16ModeMono | dspSB16ModeSigned,
    count/2, tc);
}



void SetupOutputDMA(unsigned int page, unsigned int ofs, int count,
 unsigned char tc)
// Program the DMA Controller and Sound Blaster for DMA output.
{
   SetupDMA(card1.dma8, page, ofs, count, DMAMODEREAD);
   // Program card for 8-bit signed output.
   SetupDSP(&card1,
    dspSB16DMA8 | dspSB16DAC | dspSB16AI | dspSB16FifoOn,
    dspSB16ModeMono | dspSB16ModeSigned,
    count/2, tc);
}



void SetMixer(cardinfo *card1)
// Resets the SB card and selects the proper input.
{

   unsigned int val;

   /* reset sb card #1 */
   outp(card1->Reset,1);
   delay(1);
   outp(card1->Reset,0);
   if (dspin(card1, &val))
      if (val!=dspReady)
         printf("Sound Blaster card not ready.");

   outp(card1->MixerAddr, SB16INPUTL);
   outp(card1->MixerData, 1);           // select microphone
   outp(card1->MixerAddr, SB16INPUTR);
   outp(card1->MixerData, 1);           // select microphone

   outp(card1->MixerAddr, SB16OUTPUT);
   outp(card1->MixerData, 0);           // select nothing for output
}



void InitScreen(void)
{
   clrscr();
   printf("Single Sound Blaster 16 Delay Program.\nPress any key to stop.\n");
}



void DrainInputBuffer(void)
// Copies half the DMA buffer to one of the record buffers.
{
   // Copy data from current half of DMA buffer to next input buffer.
   memcpy(inbuffers[NumRecBufToFill], recbuf + HalfBufToEmpty*BUFSIZE/2, BUFSIZE);

   // Make next buffer current
   ++NumRecBufToFill;
   if (NumRecBufToFill >= NUMBUFS)
      NumRecBufToFill =  0;

   // Increment count of filled input buffers.
   ++NumFilledInBufs;

   // Next time use other half of DMA buffer.
   HalfBufToEmpty ^= 1;
}



void FillOutputBuffer(void)
// Copies one of the output buffers to half the DMA buffer.
{
   int  i;
   int  *wptr;
   char *bptr;

// Used to be this when I was using two cards
// memcpy(playbuf + HalfBufToFill*BUFSIZE/2, outbuffers[NumPlayBufToEmpty], BUFSIZE/2);
// Now need to convert from 16-bit words to 8-bit bytes while copying.

   // Get pointer into playback buffer and output DMA buffer.
   wptr = outbuffers[NumPlayBufToEmpty];
   bptr = playbuf + HalfBufToFill*BUFSIZE/2;

   // Convert word in playback buffer to a byte and copy to DMA buffer.
   for (i=0; i<BUFSIZE/2; i++) {
      *bptr = (unsigned char) (*wptr >> 8);
      bptr++;
      wptr++;
   }

   // Make next buffer current
   ++NumPlayBufToEmpty;
   if (NumPlayBufToEmpty >= NUMBUFS)
      NumPlayBufToEmpty =  0;

   // Decrement count of filled output buffers.
   --NumFilledOutBufs;

   // Next time use other half of DMA buffer.
   HalfBufToFill ^= 1;
}



void interrupt DMAend(void)
{
   int intstat;

   // Make sure this wasn't a spurious interrupt.
   if (card1.irq == 7) {
      outp(PIC1MODE, 0xB);                // select In Service Register
      if ((inp(PIC1MODE) & 0x80) == 0)    // if bit 7 == 0, spurious interrupt
         return;
   }

   outp(card1.MixerAddr, INTSTATUS);
   intstat = inp(card1.MixerData);

   if (intstat & DMA16IntStatBit) {
      inp(card1.base + dspoffsetDMA16Ack);     // acknowledge interrupt

#ifdef debug
      ++recintcount;
#endif
      DrainInputBuffer();

   }

   if (intstat & DMA8IntStatBit) {
      inp(card1.DataAvail);                     // acknowledge interrupt
      // same as card1.base + dspoffsetDMA8Ack

#ifdef debug
      ++playintcount;
#endif

      FillOutputBuffer();
   }

   if (card1.irq > 8)                           // If irq 10, send EOI to second PIC.
      outp(PIC2MODE, PICEOI);
   outp(PIC1MODE,PICEOI);                       // Send EOI to first PIC.
}



int GetCardInfo(char *name, cardinfo *card)
// Read a blaster environment string.
{
   int result;
   int dummy;

   result=ReadBlasterEnv(name, &card->base, &card->irq, &card->dma8, &card->dma16,
    &dummy, &card->cardtype);

   if (result != 0) {
      switch (result) {
         case 1:
            printf("Error in %s port address.\n",name);
            break;
         case 2:
            printf("Error in %s IRQ number.\n",name);
            break;
         case 3:
            printf("Error in %s 8-bit DMA channel.\n",name);
            break;
         case 4:
            printf("Error in %s 16-bit DMA channel.\n",name);
            break;
         case 5:
            printf("Error in %s MIDI address.\n",name);
            break;
         case 6:
            printf("Error in %s card type number.\n",name);
            break;
         case 7:
            printf("Error:  %s environment variable not set.\n",name);
            break;
      }
   }

   card->WrBuf     = card->base + dspoffsetWrBuf;
   card->Reset     = card->base + dspoffsetReset;
   card->ReadData  = card->base + dspoffsetReadData;
   card->DataAvail = card->base + dspoffsetDataAvail;
   card->MixerAddr = card->base + dspoffsetMixerAddr;
   card->MixerData = card->base + dspoffsetMixerData;

   return result;
}



void EnableCardInterrupt(cardinfo *card, void interrupt (*newvect)())
{
   int intrmask;

   // calculate interrupt number for IRQ
   if (card->irq < 8)
      card->intrnum = card->irq + 8;          // IRQs 0-7 map to interrupts 8-15.
   else
      card->intrnum = card->irq - 8 + 0x70;   // IRQs 8-15 map to interrupts 70H-78H.

   // generate 16-bit mask for IRQ
   card->intrmask = 1 << card->irq;

   card->intvecsave = getvect(card->intrnum); // save previous interrupt vector

   setvect(card->intrnum, newvect);           // set new interrupt vector

   // enable interrupts at interrupt controllers
   intrmask = card->intrmask;
   outp(PIC1MASK, inp(PIC1MASK) & ~intrmask);
   intrmask >>= 8;
   outp(PIC2MASK, inp(PIC2MASK) & ~intrmask);
}



void DisableCardInterrupt(cardinfo *card)
{
   int intrmask;

   // disable interrupts at interrupt controllers
   intrmask = card->intrmask;
   outp(PIC1MASK, inp(PIC1MASK) | intrmask);
   intrmask >>= 8;
   outp(PIC2MASK, inp(PIC2MASK) | intrmask);

   // Restore previous vector
   setvect(card->intrnum, card->intvecsave);
}



int CheckCard(cardinfo *card)
// Make sure it's an SB16 with different 8- and 16-bit DMA channels.
{
   int result = 0;

   if (card->cardtype != 6) {
      printf("You must have a Sound Blaster 16 to run this program.\n");
      result |= 1;
   }

   if (card->dma8 == card->dma16) {
      printf("Error:  8-bit and 16-bit DMA channels are the same.\n");
      result |= 2;
   }

   return result;
}


void WriteBufsToDisk(int handle)
// Writes all the record buffers that have data to the disk.
{
   unsigned nread;

   // While there are input buffers with data, write them to disk
   while (NumFilledInBufs > 0) {
      _dos_write(handle, inbuffers[NumRecBufToEmpty], BUFSIZE, &nread);
      ++NumBufsWritten;

      --NumFilledInBufs;

      ++NumRecBufToEmpty;
      if (NumRecBufToEmpty >= NUMBUFS)
         NumRecBufToEmpty = 0;
   }
}



void ReadBufsFromDisk(int handle)
// Fills all empty playback buffers from disk.
{
   unsigned nread;

   // While some output buffers are empty, fill them from disk
   while (NumFilledOutBufs < NUMBUFS) {
      _dos_read(handle, outbuffers[NumPlayBufToFill], BUFSIZE, &nread);

      ++NumFilledOutBufs;

      ++NumPlayBufToFill;
      if (NumPlayBufToFill >= NUMBUFS)
         NumPlayBufToFill = 0;
   }
}





void main(void)
{
   int i,b,c;
   unsigned long physaddr,endaddr;
   boolean done=false;
   int result;
   int rech, playh;       // file handles for recording and playing
   char fname[] = FILENAME;


   result=GetCardInfo("BLASTER", &card1);
   if (result != 0)
      return;


   if (CheckCard(&card1))
      return;


   SegToPhys((unsigned char far *)recbuf,2*BUFSIZE,&physaddr,&endaddr);
   if (PhysToPage(physaddr,endaddr,&inpage,&inoffset) < BUFSIZE) {
      printf("The DMA buffer crossed a page boundary.  Sorry, I didn't write the program\n");
      printf("to deal with this.  See comments in program.  Terminating.\n");
      return;
      /*
       * The program should dynamically allocate buffers until a buffer is
       * found that doesn't cross a DMA page boundary.  I took a short cut
       * here by just using an array, which usually works if it's much less
       * than 64K bytes.
       */
   }

   SegToPhys(playbuf,BUFSIZE,&physaddr,&endaddr);
   if (PhysToPage(physaddr,endaddr,&outpage,&outoffset) < BUFSIZE) {
      printf("The DMA buffer crossed a page boundary.  Sorry, I didn't write the program\n");
      printf("to deal with this.  See comments in program.  Terminating.\n");
      return;
   }


   InitScreen();


   // allocate buffers to transfer data between DMA buffer and disk
   for (i=0; i<NUMBUFS; i++) {
      inbuffers[i] = malloc(BUFSIZE);
      if (!inbuffers[i]) {
         printf("Error:  memory couldn't be allocated.\n");
         return;
      }
      outbuffers[i] = malloc(BUFSIZE);
      if (!outbuffers[i]) {
         printf("Error:  memory couldn't be allocated.\n");
         return;
      }
   }

   if (_dos_creat(fname, 0, &rech)) {
      printf("Couldn't open file %s for writing.\n",fname);
      return;
   }

   if (_dos_open(fname, O_RDONLY, &playh)) {
      printf("Couldn't open file %s for reading.\n",fname);
      return;
   }

   SetMixer(&card1);

   EnableCardInterrupt(&card1, DMAend);

   // Fill playback buffers with silence and mark them as filled so the
   // playback of the actual file will start delayed by 2+NUMBUFS buffers.
   for (b=0; b<NUMBUFS; b++)
      for (i=0; i<BUFSIZE/2; ++i) {
         *((int *)outbuffers[b] + i) = 0x0000;
      }
   NumFilledOutBufs = NUMBUFS;

   // fill both halves of output DMA buffer
   FillOutputBuffer();
   FillOutputBuffer();


   // Start recording
   SetupInputDMA(inpage,inoffset,count,TIMECONST);

   // Wait until several buffers have been written to disk before starting
   // playback.  This gives the recording section a head start.  This is
   // necessary because the playback section tries to fill all of its buffers,
   // so there must be that much data already on the disk.
   do {
      WriteBufsToDisk(rech);
   } while (NumBufsWritten < NUMBUFS);


   // Now start playing.
   SetupOutputDMA(outpage,outoffset,count,TIMECONST);

   while (!done) {
      WriteBufsToDisk(rech);     // write all full input buffers
      ReadBufsFromDisk(playh);   // fill all empty output buffers

      gotoxy(1,3);
      printf("Number of buffers written to file %s: %7d\n", fname, NumBufsWritten);
#ifdef debug
      printf("Rec intcount: %7d    Play intcount %7d\n", recintcount, playintcount);
#endif
      if (kbhit()) {
         c=getch();
         switch (c) {
            case ESC:
               done=true;
               break;
            // Used to have other stuff here in a previous program.
            default:
               done=true;
         }
      }
   }

   // Stop recording and playing.
   dspout(&card1,dspcmdHaltDMA16);
   dspout(&card1,dspcmdHaltDMA8);

   DisableCardInterrupt(&card1);
}
