/* -------------------------------------------------------------------------- */
/* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY      */
/* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE        */
/* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR      */
/* PURPOSE.                                                                   */
/*                                                                            */
/* You have a royalty-free right to use, modify, reproduce and                */
/* distribute the Sample Files (and/or any modified version) in               */
/* any way you find useful, provided that you agree that                      */
/* Creative has no warranty obligations or liability for any Sample Files.    */
/*----------------------------------------------------------------------------*/


/* ---------------------------------------------------------------------------
  Program:  Sound Blaster 16 MIDI test program
  Filename: MIDITEST.C
  Author:   Scott E. Sindorf
  Date:     30 Jun 93
  Language: Borland C
  
  Copyright (c) 1993-1996 Creative Labs, Inc.
-----------------------------------------------------------------------------*/

#include <stdio.h>
#include <conio.h>
#include "sbcutils.h"
#include "midiutil.h"

extern BLASTREC blastInfo;

const char blastErr[] = "\n\007Error - BLASTER environment variable not found.";
const char MPUAddrErr[] = "\n\007Error - MPU-401 port address field is missing in \
BLASTER environment variable.";

const char BasAddrErr[] = "\n\007Error - Base address field is missing in \
BLASTER environment variable.";

const char noPortErr[] = "\n\007Error - MIDI hardware not detected.";
const char initErr[] = "\n\007Error - Unable to initialize MIDI port";

void main(void)
{
  char keyboard;

  printf("Do you have a MIDI input device connected? (Y,N) ");
  keyboard = getch();
  printf("\n\n");

  if(GetBlastInfo(&blastInfo) == OK)      // get BLASTER environment variable
  {
    printf("\n\tBLASTER environment variable was found\n\n");

                          //----------------------//
                          // MPU-401 Example Code //
                          //----------------------//
    MIDIType = MPU401;
    printf("\tMIDI interface = MPU-401\n");
    if(blastInfo.midiPort != 0)           // was MPU-401 port found?
    {
      printf("\tMPU-401 port address is %Xh\n", blastInfo.midiPort);
      if(Detect_MIDI() == OK)             // was hardware detected?
      {
        printf("\tHardware was detected\n");
        if(Init_MIDI() == OK)             // initialize MPU-401 port
        {
          printf("\tMPU-401 port initialized properly\n");

          if((keyboard == 'Y') || (keyboard == 'y'))
          {
            printf("\n\tMIDI Thru test  -  Hit any key to continue\n\n");
            while(!kbhit())
              Write_MPU401_Data(Read_MPU401_Data());
            getch();                      // clear keystroke
          }

          Exit_MIDI();
          printf("\tMIDI driver has been terminated\n\n");
        }
        else
          printf(initErr);
      }
      else
        printf(noPortErr);
    }
    else
      printf(MPUAddrErr);

                          //---------------------//
                          // SBMIDI Example Code //
                          //---------------------//
    MIDIType = SBMIDI;
    printf("\tMIDI interface = SBMIDI\n");
    if(blastInfo.baseAddr != 0)           // was base Address found?
    {
      printf("\tSound Blaster base address is %Xh\n", blastInfo.baseAddr);
      if(Detect_MIDI() == OK)             // was hardware detected?
      {
        printf("\tHardware was detected\n");
        if(Init_MIDI() == OK)             // initialize SBMIDI
        {
          printf("\tSBMIDI initialized properly\n");

          if((keyboard == 'Y') || (keyboard == 'y'))
          {
            printf("\n\tMIDI Thru test  -  Hit any key to continue\n\n");
            while(!kbhit())
              Write_SBMIDI_Data(Read_SBMIDI_Data());
            getch();                      // clear keystroke
          }

          Exit_MIDI();
          printf("\tMIDI driver has been terminated\n");
        }
        else
          printf(initErr);
      }
      else
        printf(noPortErr);
    }
    else
      printf(BasAddrErr);
	}
	else
		printf(blastErr);
}

