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
  Program:  Sound Blaster 16 MIDI Utilities
  Filename: MIDIUTIL.H
  Author:   Scott E. Sindorf
  Language: Borland C
  Date:     30 Jun 93
  
  Copyright (c) 1993 Creative Labs, Inc.
-----------------------------------------------------------------------------*/

// MIDI flag choices

#define SBMIDI          0
#define MPU401          1

// MPU-401 definitions

#define MPU401_RESET    0xff
#define MPU401_UART     0x3f
#define MPU401_CMDOK    0xfe
#define MPU401_OK2WR    0x40
#define MPU401_OK2RD    0x80

// SB-MIDI definitions

#define MIDI_IN_P       0x30              // MIDI read (polling mode)
#define MIDI_IN_I       0x31              // MIDI read (interrupt mode)
#define MIDI_UART_P     0x34              // MIDI UART mode (polling mode)
#define MIDI_UART_I     0x35              // MIDI UART mode (interrupt mode)
#define MIDI_UART_TS_P  0x36              // same as 0x34 with timestamp
#define MIDI_UART_TS_I  0x37              // same as 0x35 with timestamp
#define MIDI_OUT_P      0x38              // MIDI write (polling mode)

#define ERROR           -1
#define OK              0
#define ON              1
#define OFF             0

extern int MIDIType;

//------------------------
// FUNCTION DECLARATIONS
//------------------------

void Write_MPU401_Cmd(unsigned char command);
void Write_MPU401_Data(unsigned char data);
void Write_SBMIDI_Data(unsigned char data);
unsigned char Read_MPU401_Data(void);
unsigned char Read_SBMIDI_Data(void);
void Send_MIDI_Message(char far *message, int size);

void Set_MIDI_Vol(char channel, char volume);
void Set_MIDI_Notes_Off(char channel);
void Set_MIDI_Pitch_Bend(char channel, int range);
int Reset_MPU401(void);
int Set_UART_Mode(int state);
int Detect_MIDI(void);
int Init_MIDI(void);
void Exit_MIDI(void);
