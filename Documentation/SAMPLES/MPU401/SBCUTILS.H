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
  Program:  Sound Blaster Utilities
  Filename: SBCUTILS.H
  Author:   Scott E. Sindorf
  Language: Borland C
  Date:     30 Jun 93
  
  Copyright (c) 1993-1996 Creative Labs, Inc.
-----------------------------------------------------------------------------*/
#define ERROR       -1
#define OK           0
#define ON           1
#define OFF          0

#define MSB          0x80

#define SBC          1                    // Card types
#define SBMCV        1
#define SBPRO        2
#define SB20         3
#define SBPRO2       4
#define SBMCVPRO     5
#define SB16         6

#define DSP_RST      0x06                 // DSP reset port
#define DSP_RD_ST    0x0e                 // DSP read buffer status port
#define DSP_RD       0x0a                 // DSP read port
#define DSP_WR_ST    0x0c                 // DSP write buffer status port
#define DSP_WR       0x0c                 // DSP write port

#define DSP_RST_OK   0xaa                 // DSP reset OK

typedef struct                            // BLASTER environment record
{
  int baseAddr;
  unsigned char DSPint;
  unsigned char DMA8;
  unsigned char model;
  unsigned char DMA16;
  int midiPort;
} BLASTREC;

//------------------------
// FUNCTION DECLARATIONS
//------------------------

int GetBlastInfo(BLASTREC *blastInfo);
int DSPReset(void);
