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

/* DELAY.C
 *
 * D. Kaden 6/22/94
 *
 * This program records and plays simultaneously using two Sound Blaster
 * cards.
 *
 * In addition to the normal BLASTER environment variable, you should also
 * have a BLASTER2 environment variable for the second Sound Blaster card.
 *
 * For example, your environment should look something like this:
 *
 *    BLASTER=A220 I5 D1 H5 P330 T6
 *    BLASTER2=A240 I7 D3 H6 P330 T6
 *
 * The first card (BLASTER) does the recording.  The second (BLASTER2) does
 * the playback.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <conio.h>
#include <dos.h>
#include <mem.h>
#include <fcntl.h>
#include "mydefs.h"
#include "sbdefs.h"

#define FILENAME "delay.raw"


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


cardinfo      card1, card2;

              // It would be better to dynamically allocate the DMA buffers
              // to guarantee that they don't cross a DMA page boundary.
              // This is quick and dirty, but it usually works.
unsigned char recbuf[BUFSIZE],         // recording DMA buffer
              playbuf[BUFSIZE];        // playback DMA buffer

unsigned int  inpage,   outpage,       // DMA pages for input and output buffers
              inoffset, outoffset,     // offsets for DMA input and output buffers
              count = BUFSIZE;         // size of the DMA buffers

char          *inbuffers[NUMBUFS];     // Input (recording) buffers to
                                       //  transfer data from DMA buffer
                                       //  to disk.
char          *outbuffers[NUMBUFS];    // Output (playback) buffers to
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



unsigned int PhysToPage(unsigned long physaddr, unsigned long endaddr, unsigned int *page, unsigned int *offset)
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

   outp(dmaports.mask, 4 | dmachan);            // mask off dma channel
   outp(dmaports.FF, 0);                        // clear flip-flop
/* This next value to DMAC is different between auto-init and non-auto-init.
 * (0x58 vs. 0x48) */
   outp(dmaports.mode , dmacmd | dmachan);      // set mode
   outp(dmaports.addr, (ofs & 0xFF));           // low byte base addr
   outp(dmaports.addr, (ofs >> 8));             // high byte base addr
   outp(dmaports.page, page);                   // physical page number
   outp(dmaports.count,((DMAcount-1) & 0xFF));  // count low byte
   outp(dmaports.count,((DMAcount-1) >> 8));    // count high byte
   outp(dmaports.mask, dmachan);                // enable dma channel
}



void SetupDSP(cardinfo *card, unsigned char dspcmd, unsigned short DSPcount,
 unsigned char tc)
// Programs the Sound Blaster card.
{
   // Program the time constant.
   dspout(card, dspcmdTimeConst);
   dspout(card, tc);

   // Program the DMA buffer size.
   dspout(card, dspcmdBlockSize);
   dspout(card, (DSPcount-1) & 0xFF);
   dspout(card, (DSPcount-1) >> 8);

   // Send the play or record command.
   dspout(card, dspcmd);
}



void SetupInputDMA(unsigned int page, unsigned int ofs, int count,
 unsigned char tc)
// Program the DMA Controller and Sound Blaster for DMA input.
{
   SetupDMA(card1.dma8, page, ofs, count, DMAMODEWRITE);
   SetupDSP(&card1, dspcmdAUTODMAADC, count/2, tc);
}



void SetupOutputDMA(unsigned int page, unsigned int ofs, int count,
 unsigned char tc)
// Program the DMA Controller and Sound Blaster for DMA output.
{
   SetupDMA(card2.dma8, page, ofs, count, DMAMODEREAD);
   SetupDSP(&card2, dspcmdAUTODMADAC, count/2, tc);
}



void SetMixer(cardinfo *card1, cardinfo *card2)
// Resets the SB cards and selects the proper input.
{

   unsigned int val;

   /* reset sb card #1 */
   outp(card1->Reset,1);
   delay(1);
   outp(card1->Reset,0);
   if (dspin(card1, &val))
      if (val!=dspReady)
         printf("Sound Blaster card #1 not ready.");

   switch (card1->cardtype) {
      case 2:
      case 4:     // SBPRO
         outp(card1->MixerAddr, ADCSELECT);
         outp(card1->MixerData, 0);             // select microphone
         dspout(card1, 0xD3);                   // turn speaker off
         break;
      case 6:     // SB16
         outp(card1->MixerAddr, SB16INPUTL);
         outp(card1->MixerData, 1);             // select microphone
         outp(card1->MixerAddr, SB16INPUTR);
         outp(card1->MixerData, 1);             // select microphone
         break;
   }

   /* reset sb card #2 */
   outp(card2->Reset,1);
   delay(1);
   outp(card2->Reset,0);
   if (dspin(card2,&val))
      if (val!=dspReady)
         printf("Sound Blaster card #2 not ready.");

   switch (card2->cardtype) {
      case 2:
      case 4:     // SBPRO
         dspout(card2,0xD1);        // turn speaker on
         break;
      case 6:
         outp(card2->MixerAddr, SB16INPUTL);
         outp(card2->MixerData, 0);             // select nothing
         outp(card2->MixerAddr, SB16INPUTR);
         outp(card2->MixerData, 0);             // select nothing
         break;
   }
}



void InitScreen(void)
{
   clrscr();
   printf("Two Sound Blaster Delay program.\nPress any key to stop.\n");
}



void DrainInputBuffer(void)
// Copies half the DMA buffer to one of the record buffers.
{
   // Copy data from current half of DMA buffer to next input buffer. 
   memcpy(inbuffers[NumRecBufToFill], recbuf + HalfBufToEmpty*BUFSIZE/2, BUFSIZE/2);

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
   // Copy data from next output buffer to current half of DMA buffer.
   memcpy(playbuf + HalfBufToFill*BUFSIZE/2, outbuffers[NumPlayBufToEmpty], BUFSIZE/2);

   // Make next buffer current
   ++NumPlayBufToEmpty;
   if (NumPlayBufToEmpty >= NUMBUFS)
      NumPlayBufToEmpty =  0;

   // Decrement count of filled output buffers.
   --NumFilledOutBufs;

   // Next time use other half of DMA buffer.
   HalfBufToFill ^= 1;
}



void interrupt InputSBISR(void)
// Service interrupts from SB doing input.
{
   // Make sure this wasn't a spurious interrupt.
   if (card1.irq == 7) {
      outp(PIC1MODE, 0xB);                // select In Service Register
      if ((inp(PIC1MODE) & 0x80) == 0)    // if bit 7 == 0, spurious interrupt
         return;
   }

   inp(card1.DataAvail);                  // acknowledge interrupt

   DrainInputBuffer();

   if (card1.irq > 8)                     // If irq 10, send EOI to second PIC.
      outp(PIC2MODE, PICEOI);
   outp(PIC1MODE,PICEOI);                 // Send EOI to first PIC.
}



void interrupt OutputSBISR(void)
// Service interrupts from SB doing output.
{
   // Make sure this wasn't a spurious interrupt.
   if (card2.irq == 7) {
      outp(PIC1MODE, 0xB);                // select In Service Register
      if ((inp(PIC1MODE) & 0x80) == 0)    // if bit 7 == 0, spurious interrupt
         return;
   }

   inp(card2.DataAvail);                  // acknowledge interrupt

   FillOutputBuffer();

   if (card2.irq > 8)                     // If irq 10, send EOI to second PIC.
      outp(PIC2MODE, PICEOI);
   outp(PIC1MODE,PICEOI);                 // Send EOI to first PIC.
}





int GetCardInfo(char *name, cardinfo *card)
// Read a blaster environment string.
{
   int result;
   int dummy;

   result=ReadBlasterEnv(name, &card->base, &card->irq, &card->dma8, &dummy,
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



int CompareCards(cardinfo *card1, cardinfo *card2)
// Make sure the two SB cards don't have the same settings.
{
   int result = 0;

   if (card1->base == card2->base) {
      printf("Error:  Cards are at the same I/O address: %X\n",card1->base);
      result |= 1;
   }

   if (card1->irq == card2->irq) {
      printf("Error:  Cards are using the same interrupt: %d\n",card1->irq);
      result |= 2;
   }

   if (card1->dma8 == card2->dma8) {
      printf("Error:  Cards are using the same DMA channel: %d\n",card1->dma8);
      result |= 4;
   }

   return result;
}


void WriteBufsToDisk(int handle)
// Writes all the record buffers that have data to the disk.
{
   unsigned nread;

   // While there are input buffers with data, write them to disk
   while (NumFilledInBufs > 0) {
      _dos_write(handle, inbuffers[NumRecBufToEmpty], BUFSIZE/2, &nread);
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
      _dos_read(handle, outbuffers[NumPlayBufToFill], BUFSIZE/2, &nread);

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

   result=GetCardInfo("BLASTER2", &card2);
   if (result != 0)
      return;

   if (CompareCards(&card1, &card2))
      return;

   SegToPhys(recbuf,BUFSIZE,&physaddr,&endaddr);
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
      inbuffers[i] = malloc(BUFSIZE/2);
      if (!inbuffers[i]) {
         printf("Error:  memory couldn't be allocated.\n");
         return;
      }
      outbuffers[i] = malloc(BUFSIZE/2);
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

   SetMixer(&card1, &card2);

   EnableCardInterrupt(&card1, InputSBISR);
   EnableCardInterrupt(&card2, OutputSBISR);


   // Fill playback buffers with silence and mark them as filled so the
   // playback of the actual file will start delayed by 2+NUMBUFS buffers.
   for (b=0; b<NUMBUFS; b++)
      for (i=0; i<BUFSIZE/2; ++i) {
         *((char *)outbuffers[b] + i) = 0x80;
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
      printf("Number of buffers written to disk: %5d\n",NumBufsWritten);

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
   dspout(&card1,dspcmdHaltDMA8);
   dspout(&card2,dspcmdHaltDMA8);

   DisableCardInterrupt(&card1);
   DisableCardInterrupt(&card2);
}
