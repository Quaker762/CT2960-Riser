/* -------------------------------------------------------------------------- */
/*                                                                            */
/* FM synthesizer low-level interface demo program.                           */
/* Copyright (c) 1993-96 Creative Labs, Inc.                                  */
/*                                                                            */
/* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY      */
/* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE        */
/* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR      */
/* PURPOSE.                                                                   */
/*                                                                            */
/* You have a royalty-free right to use, modify, reproduce and                */
/* distribute the Sample Files (and/or any modified version) in               */
/* any way you find useful, provided that you agree that                      */
/* Creative has no warranty obligations or liability for any Sample Files.    */
/*                                                                            */
/*----------------------------------------------------------------------------*/
  
/*
 *  This Program is written using Borland C++ Ver 3.1.
 *  To Compile : BCC FMSAMPLE.C
 * ---------------------------------------------------------------------
 *
 * This program is not intended to explain all the aspects of FM sound
 * generation on Sound Blaster cards.  The chips are too complicated for
 * that.  This program is just to demonstrate how to generate a tone and
 * control the left and right channels.  For more information on the FM
 * synthesizer chip, contact Yamaha.
 *
 * Here's a brief description of FM:  Each sound is created by two operator
 * cells (called "slots" in the Yamaha documentation), a modulator and a
 * carrier.  When FM synthesis was invented, the output value of the
 * modulator affected the frequency of the carrier.  In the Yamaha chips, the
 * modulator output actually affects the phase of the carrier instead of
 * frequency, but this has a similar  effect.
 *
 * Normally the modulator and carrier would probably be connected in series
 * for complex sounds.  For this program, I wanted a pure sine wave, so I
 * connected them in parallel and turned the modulator output down all the
 * way and used just the carrier.
 *
 * Sound Blaster 1.0 - 2.0 cards have one OPL-2 FM synthesis chip at
 * addresses 2x8 and 2x9 (base + 8 and base + 9).  Sound Blaster Pro version
 * 1 cards (CT-1330) achieve stereo FM with two OPL-2 chips, one for each
 * speaker.  The left channel FM chip is at addresses 2x0 and 2x1.  The right
 * is at 2x2 and 2x3.  Addresses 2x8 and 2x9 address both chips
 * simultaneously, thus maintaining compatibility with the monaural Sound
 * Blaster cards.  The OPL-2 contains 18 operator cells which make up the
 * nine 2-operator channels.  Since the CT-1330 SB Pro has two OPL-2 chips,
 * it is therefore capable of generating 9 voices in each speaker.
 *
 * Sound Blaster Pro version 2 (CT-1600) and Sound Blaster 16 cards have one
 * OPL-3 stereo FM chip at addresses 2x0 - 2x3.  The OPL-3 is separated into
 * two "banks."  Ports 2x0 and 2x1 control bank 0, while 2x2 and 2x3 control
 * bank 1.  Each bank can generate nine 2-operator voices.  However, when the
 * OPL-3 is reset, it switches into OPL-2 mode.  It must be put into OPL-3
 * mode to use the voices in bank 1 or the stereo features.  For stereo
 * control, each channel may be sent to the left, the right, or both
 * speakers, controlled by two bits in registers C0H - C8H.
 *
 * The FM chips are controlled through a set of registers.  The following
 * table shows how operator cells and channels are related, and the register
 * offsets to use.
 *
 * FUNCTION  MODULATOR-  -CARRIER--  MODULATOR-  -CARRIER--  MODULATOR-  -CARRIER--
 * OP CELL    1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18
 * CHANNEL    1   2   3   1   2   3   4   5   6   4   5   6   7   8   9   7   8   9
 * OFFSET    00  01  02  03  04  05  08  09  0A  0B  0C  0D  10  11  12  13  14  15
 *
 * An example will make the use of this table clearer:  suppose you want to
 * set the attenuation of both of the operators of channel 4.  The KSL/TOTAL LEVEL
 * registers (which set the attenuation) are 40H - 55H.  The modulator for
 * channel 4 is op cell 7, and the carrier for channel 4 is op cell 10.  The
 * offsets for the modulator and carrier cells are 08H and 0BH, respectively.
 * Therefore, to set the attenuation of the modulator, you would output a
 * value to register 40H + 08H == 48H, and to set the carrier's attenuation,
 * you would output to register 40H + 0BH == 4BH.
 *
 * In this program, I used just channel 1, so the registers I used were 20H,
 * 40H, 60H, etc., and 23H, 43H, 63H, etc.
 *
 * The frequencies of each channel are controlled with a frequency number and
 * a multiplier.  The modulator and carrier of a channel both get the same
 * frequency number, but they may be given different multipliers.  Frequency
 * numbers are programmed in registers A0H - A8H (low 8 bits) and B0H - B8H
 * (high 2 bits).  Those registers control entire channels (2 operators), not
 * individual operator cells.  To turn a note on, the key-on bit in the
 * appropriate channel register is set.  Since these registers deal with
 * channels, you do not use the offsets listed in the table above.  Instead,
 * add (channel-1) to A0H or B0H.  For example, to turn channel 1 on,
 * program the frequency number in registers A0H and B0H, and set the key-on
 * bit to 1 in register B0H.  For channel 3, use registers A2H and B2H.
 *
 * Bits 2 - 4 in registers B0H - B8H are the block (octave) number for the
 * channel.
 *
 * Multipliers for each operator cell are programmed through registers 20H -
 * 35H.  The table below shows what multiple number to program into the
 * register to get the desired multiplier.  The multiple number goes into
 * bits 0 - 3 in the register.  Note that it's a bit strange at the end.
 *
 *   multiple number     multiplier        multiple number     multiplier
 *          0                1/2                   8               8
 *          1                 1                    9               9
 *          2                 2                    10              10
 *          3                 3                    11              10
 *          4                 4                    12              12
 *          5                 5                    13              12
 *          6                 6                    14              15
 *          7                 7                    15              15
 *
 * This equation shows how to calculate the required frequency number (to
 * program into registers A0H - A8H and B0H - B8H) to get the desired
 * frequency:
 *                fn=(long)f * 1048576 / b / m /50000L
 * where f is the frequency in Hz,
 *       b is the block (octave) number between 0 and 7 inclusive, and
 *       m is the multiple number between 0 and 15 inclusive.
 *
 */


#define STEREO         // Define this for SBPro CT-1330 or later card.
#define OPL3           // Also define this for SBPro CT-1600 or later card.


#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <ctype.h>
#include <dos.h>

#define KEYON	 0x20     // key-on bit in regs b0 - b8

 /* These are offsets from the base I/O address. */
#define FM       8        // SB (mono) ports (e.g. 228H and 229H)
#define PROFM1   0        // On CT-1330, this is left OPL-2.  On CT-1600 and
                          // later cards, it's OPL-3 bank 0.
#define PROFM2   2        // On CT-1330, this is right OPL-2.  On CT-1600 and
                          // later cards, it's OPL-3 bank 1.



#ifdef OPL3
  #define LEFT     0x10
  #define RIGHT	   0x20
#endif


unsigned IOport;        // Sound Blaster port address



void mydelay(unsigned long clocks)
/*
 * "clocks" is clock pulses (at 1.193180 MHz) to elapse, but remember that
 * normally the system timer runs in mode 3, in which it counts down by twos,
 * so delay3(1193180) will only delay half a second.
 *
 *   clocks = time * 2386360
 *
 *     time = clocks / 2386360
 */
{
   unsigned long elapsed=0;
   unsigned int last,next,ncopy,diff;

   /* Read the counter value. */
   outp(0x43,0);                              /* want to read timer 0 */
   last=inp(0x40);                            /* low byte */
   last=~((inp(0x40)<< 8) + last);            /* high byte */

   do {
      /* Read the counter value. */
      outp(0x43,0);                           /* want to read timer 0 */
      next=inp(0x40);                         /* low byte */
      ncopy=next=~((inp(0x40)<< 8) + next);   /* high byte */

      next-=last;      /* this is now number of elapsed clock pulses since last read */

      elapsed += next; /* add to total elapsed clock pulses */
      last=ncopy;
   } while (elapsed<clocks);
}



int base16(char **str, unsigned *val)
/* Takes a double pointer to a string, interprets the characters as a
 * base-16 number, and advances the pointer.
 * Returns 0 if successful, 1 if not.
 */
{
   char c;
   int digit;
   *val = 0;

   while ( **str != ' ') {
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



int base10(char **str, unsigned *val)
/* Takes a double pointer to a string, interprets the characters as a
 * base-10 number, and advances the pointer.
 * Returns 0 if successful, 1 if not.
 */
{
   char c;
   int digit;
   *val = 0;

   while ( **str != ' ') {
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



unsigned ReadBlasterEnv(unsigned *port, unsigned *irq, unsigned *dma8,
 unsigned *dma16)
/* Gets the Blaster environment statement and stores the values in the
 * variables whose addresses were passed to it.
 * Returns:
 *   0  if successful
 *   1  if there was an error reading the port address.
 *   2  if there was an error reading the IRQ number.
 *   3  if there was an error reading the 8-bit DMA channel.
 *   4  if there was an error reading the 16-bit DMA channel.
 */
{
   char     *env;
   unsigned val;
   int      digit;

   env = getenv("BLASTER");

   while (*env) {
      switch(toupper( *(env++) )) {
         case 'A':
            if (base16(&env, port))     // interpret port value as hex
               return 1;                // error
            break;
         case 'I':
            if (base10(&env, irq))      // interpret IRQ as decimal
               return 2;                // error
            break;
         case 'D':
            if (base10(&env, dma8))     // 8-bit DMA channel is decimal
               return 3;
            break;
         case 'H':
            if (base10(&env, dma16))    // 16-bit DMA channel is decimal
               return 4;
            break;
         default:
            break;
      }
   }

   return 0;
}



void FMoutput(unsigned port, int reg, int val)
/* This outputs a value to a specified FM register at a specified FM port. */
{
   outp(port, reg);
   mydelay(8);          /* need to wait 3.3 microsec */
   outp(port+1, val);
   mydelay(55);         /* need to wait 23 microsec */
}



void fm(int reg, int val)
/* This function outputs a value to a specified FM register at the Sound
 * Blaster (mono) port address.
 */
{
   FMoutput(IOport+FM, reg, val);
}


void Profm1(int reg, int val)
/* This function outputs a value to a specified FM register at the Sound
 * Blaster Pro left FM port address (or OPL-3 bank 0).
 */
{
   FMoutput(IOport+PROFM1, reg, val);
}


void Profm2(int reg, int val)
/* This function outputs a value to a specified FM register at the Sound
 * Blaster Pro right FM port address (or OPL-3 bank 1).
 */
{
   FMoutput(IOport+PROFM2, reg, val);
}




void main(void)
{
   int i,val1,val2;

   int block,b,m,f,fn;

   unsigned dummy;


   clrscr();

   ReadBlasterEnv(&IOport, &dummy, &dummy, &dummy);

#ifdef STEREO
 #ifdef OPL3
   printf("Program compiled for Sound Blaster Pro ver. 2 (CT-1600) and SB16 cards.\n");
 #else
   printf("Program compiled for Sound Blaster Pro ver. 1 (CT-1330) cards.\n");
 #endif
#else
   printf("Program compiled for Sound Blaster 1.0 - 2.0 cards (monaural).\n");
#endif


   fm(1,0);        /* must initialize this to zero */

#ifdef OPL3
   Profm2(5, 1);  /* set to OPL3 mode, necessary for stereo */
   fm(0xC0,LEFT | RIGHT | 1);     /* set both channels, parallel connection */
#else
   fm(0xC0,               1);     /* parallel connection */
#endif

   /***************************************
    * Set parameters for the carrier cell *
    ***************************************/

   fm(0x23,0x21);  /* no amplitude modulation (D7=0), no vibrato (D6=0),
                    * sustained envelope type (D5=1), KSR=0 (D4=0),
                    * frequency multiplier=1 (D4-D0=1)
                    */

   fm(0x43,0x0);   /* no volume decrease with pitch (D7-D6=0),
                    * no attenuation (D5-D0=0)
                    */

   fm(0x63,0xff);  /* fast attack (D7-D4=0xF) and decay (D3-D0=0xF) */
   fm(0x83,0x05);  /* high sustain level (D7-D4=0), slow release rate (D3-D0=5) */


   /*****************************************
    * Set parameters for the modulator cell *
    *****************************************/

   fm(0x20,0x20);  /* sustained envelope type, frequency multiplier=0    */
   fm(0x40,0x3f);  /* maximum attenuation, no volume decrease with pitch */

   /* Since the modulator signal is attenuated as much as possible, these
    * next two values shouldn't have any effect.
    */
   fm(0x60,0x44);  /* slow attack and decay */
   fm(0x80,0x05);  /* high sustain level, slow release rate */


   /*************************************************
    * Generate tone from values looked up in table. *
    *************************************************/

   printf("440 Hz tone, values looked up in table.\n");
   fm(0xa0,0x41);  /* 440 Hz */
   fm(0xb0,0x32);  /* 440 Hz, block 0, key on */

   getche();

   fm(0xb0,0x12);  /* key off */


   /******************************************
    * Generate tone from a calculated value. *
    ******************************************/

   printf("440 Hz tone, values calculated.\n");
   block=4;        /* choose block=4 and m=1 */
   m=1;		       /* m is the frequency multiple number */
   f=440;          /* want f=440 Hz */
   b= 1 << block;

   /* This is the equation to calculate frequency number from frequency. */

   fn=(long)f * 1048576 / b / m /50000L;

   fm(0x23,0x20 | (m & 0xF));   /* 0x20 sets sustained envelope, low nibble
                                 * is multiple number
                                 */
   fm(0xA0,(fn & 0xFF));
   fm(0xB0,((fn >> 8) & 0x3) + (block << 2) | KEYON);

   getche();


   /*********************************************************
    * Generate a range of octaves by changing block number. *
    *********************************************************/

   printf("Range of frequencies created by changing block number.\n");
   for (block=0; block<=7; block++) {
      printf("f=%5ld Hz (press Enter)\n",(long)440*(1 << block)/16);
      fm(0xB0,((fn >> 8) & 0x3) + (block << 2) | KEYON);
      getche();
   }


   /*****************************************************************
    * Generate a range of frequencies by changing frequency number. *
    *****************************************************************/

   printf("Range of frequencies created by changing frequency number.\n");
   block=4;
   for (fn=0; fn<1024; fn++) {
      fm(0xA0,(fn & 0xFF));
      fm(0xB0,((fn >> 8) & 0x3) + (block << 2) | KEYON);
      delay(1);
   }


   /********************************************************************
    * Single tone again.  Both channels, then if on stereo board,      *
    * play tone in just the left channel, then just the right channel. *
    ********************************************************************/

   printf("440 Hz again, both channels.\n");
   block=4;
   fn=577;                /* This number makes 440 Hz when block=4 and m=1 */
   fm(0xA0,(fn & 0xFF));
   fm(0xB0,((fn >> 8) & 0x3) + (block << 2) | KEYON);

#ifdef STEREO
 #ifdef OPL3
    /* This left and right channel stuff is the only part of this program
     * that uses OPL3 mode.  Everything else is available on the OPL2.
     */

    getche();
    printf("Left channel only\n");
    fm(0xC0,LEFT | 1);      /* set left channel only, parallel connection */

    getche();
    printf("Right channel only\n");
    fm(0xC0,RIGHT | 1);     /* set right channel only, parallel connection */
 #else
    getche();
    fm(0xB0,((fn >> 8) & 0x3) + (block << 2));       // key off

    printf("Left channel only\n");
    Profm1(0xB0,((fn >> 8) & 0x3) + (block << 2) | KEYON);

    getche();
    Profm1(0xB0,((fn >> 8) & 0x3) + (block << 2));   // key off

    printf("Right channel only\n");
    Profm2(0xB0,((fn >> 8) & 0x3) + (block << 2) | KEYON);


 #endif
#endif


   /*********************************
    * Attenuate the signal by 3 dB. *
    *********************************/

   getche();
   fm(0xB0,((fn >> 8) & 0x3) + (block << 2) | KEYON);
   printf("Attenuated by 3 dB.\n");
   fm(0x43,4);     /* attenuate by 3 dB */
   getche();

   fm(0xB0,((fn >> 8) & 0x3) + (block << 2));

#ifdef OPL3
   /* Set OPL-3 back to OPL-2 mode, because if the next program to run was
    * written for the OPL-2, then it won't set the LEFT and RIGHT bits to
    * one, so no sound will be heard.
    */
   Profm2(5, 0);   /* set back to OPL2 mode */
#endif
}
