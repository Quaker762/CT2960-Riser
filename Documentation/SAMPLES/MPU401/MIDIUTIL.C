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
  Filename: MIDIUTIL.C
  Author:   Scott E. Sindorf
  Language: Borland C
  Date:     30 Jun 93
  
  Copyright (c) 1993-1996 Creative Labs, Inc.
-----------------------------------------------------------------------------*/

#include <conio.h>
#include "midiutil.h"
#include "sbcutils.h"

extern BLASTREC blastInfo;

int MIDIType = MPU401;                    // MIDI type to use flag

/************************************************************************
 * WRITE_MPU401_CMD: sends a command to the MPU401 port.
 *
 * Input: command - command byte
 *
 * Output: none
 ************************************************************************/
void Write_MPU401_Cmd(unsigned char command)
{
  // check write status
  while((inp(blastInfo.midiPort + 1) & MPU401_OK2WR));

  outp(blastInfo.midiPort + 1, command);
}

/************************************************************************
 * WRITE_MPU401_DATA: sends a data byte to the MPU401 port.
 *
 * Input: data - data byte
 *
 * Output: none
 ************************************************************************/
void Write_MPU401_Data(unsigned char data)
{
  // check write status
  while((inp(blastInfo.midiPort + 1) & MPU401_OK2WR));

  outp(blastInfo.midiPort, data);
}

/************************************************************************
 * WRITE_SBMIDI_DATA: sends a byte to the Sound Blaster MIDI port.
 *
 * Input: data - data byte
 *
 * Output: none
 ************************************************************************/
void Write_SBMIDI_Data(unsigned char data)
{
  // wait for write buffer ready
  while(inp(blastInfo.baseAddr + DSP_WR_ST) & MSB);
  // send MIDI write command
  outp(blastInfo.baseAddr + DSP_WR, MIDI_OUT_P);

  // wait for write buffer ready
  while(inp(blastInfo.baseAddr + DSP_WR_ST) & MSB);
  // send MIDI data byte
  outp(blastInfo.baseAddr + DSP_WR, data);
}

/************************************************************************
 * READ_MPU401_DATA: reads a byte from the MPU401 port.
 *
 * Input: none
 *
 * Output: data byte
 ************************************************************************/
unsigned char Read_MPU401_Data(void)
{
  // check read status
  while((inp(blastInfo.midiPort + 1) & MPU401_OK2RD));

  return(inp(blastInfo.midiPort));
}

/************************************************************************
 * READ_SBMIDI_DATA: reads a byte from the Sound Blaster MIDI port.
 *
 * Input: none
 *
 * Output: data - data byte
 ************************************************************************/
unsigned char Read_SBMIDI_Data(void)
{
  unsigned char data;

  // wait for write buffer ready
  while(inp(blastInfo.baseAddr + DSP_WR_ST) & MSB);
  // send MIDI read command
  outp(blastInfo.baseAddr + DSP_WR, MIDI_IN_P);
  // wait for read buffer ready
  while(!(inp(blastInfo.baseAddr + DSP_RD_ST) & MSB));
  // read MIDI data byte
  data = inp(blastInfo.baseAddr + DSP_RD);
  // wait for write buffer ready
  while(inp(blastInfo.baseAddr + DSP_WR_ST) & MSB);
  // stop MIDI read
  outp(blastInfo.baseAddr + DSP_WR, MIDI_IN_P);
  return(data);
}

/************************************************************************
 * SEND_MIDI_MESSAGE: Sends a MIDI message to port
 *
 * Input: message - MIDI message to send
 *        size - length of message
 *
 * Output: none
 ************************************************************************/
void Send_MIDI_Message(char far *message, int size)
{
  int i;

  if(MIDIType == MPU401)
    for(i = 0; i < size; i++)
      Write_MPU401_Data(message[i]);
  else
    for(i = 0; i < size; i++)
      Write_SBMIDI_Data(message[i]);
}

/************************************************************************
 * SET_MIDI_VOL: Sets the global volume on the requested channel
 *
 * Input: channel - set volume on this channel (1 to 16)
 *        volume - set channel to this volume (0 to 127)
 *
 * Output: none
 *
 * note: if inputs are out of range then no action occurs
 ************************************************************************/
void Set_MIDI_Vol(char channel, char volume)
{
  char message[3];

  if((channel <= 16) && (channel > 0))
    if(volume >= 0)
    {
      message[0] = 0xb0 + channel - 1;    // set channel
      message[1] = 7;                     // set global volume command
      message[2] = volume;                // set volume level
      Send_MIDI_Message(message, 3);
    }
}

/************************************************************************
 * SET_MIDI_NOTES_OFF: Turns off all notes on the given channel
 *
 * Input: channel - turns off all notes on this channel (1 to 16)
 *
 * Output: none
 *
 * note: if channel is out of range then no action occurs
 ************************************************************************/
void Set_MIDI_Notes_Off(char channel)
{
  char message[3];

  if((channel <= 16) && (channel > 0))
  {
    message[0] = 0xb0 + channel - 1;      // set channel
    message[1] = 0x7b;                    // clear all notes command
    message[2] = 0;
    Send_MIDI_Message(message, 3);
  }
}

/************************************************************************
 * SET_MIDI_PITCH_BEND: Sets pitch bend (0 = OFF).
 *
 * Input: channel - sets pitch bend on this channel (1 to 16)
 *        bend - pitch bend value (-8192 to 8191)
 *
 * Output: none
 *
 * note: if inputs are out of range then no action occurs
 ************************************************************************/
void Set_MIDI_Pitch_Bend(char channel, int range)
{
  char message[3];

  if((channel <= 16) && (channel > 0))
    if((range >= -8192) && (range < 8192))
    {
      range += 8192;
      message[0] = 0xe0 + channel - 1;    // set channel
      message[1] = range >> 7;            // set pitch bend (MSB)
      message[2] = range & 0x7f;          // set pitch bend (LSB)
      Send_MIDI_Message(message, 3);
    }
}

/************************************************************************
 * RESET_MPU401: resets the MPU401 port.
 *
 * Input: none
 *
 * Output: result - (0 = OK, -1 = ERROR)
 *
 * note: Error will occur if UART mode is not turned off first.
 ************************************************************************/
int Reset_MPU401(void)
{
  int i = 100;

  Write_MPU401_Cmd(MPU401_RESET);         // send reset command
  while((i > 0) && (Read_MPU401_Data() != MPU401_CMDOK))
    i--;                                  // offer timeout instead of lockup

  if(i == 0)
    return(ERROR);
  else
    return(OK);
}

/************************************************************************
 * SET_UART_MODE: puts the MPU401 port in UART mode.
 *
 * Input: state - ON or OFF (Reset will return error if UART mode is on
 *                when reset occurs)
 *
 * Output: result - (0 = OK, -1 = ERROR)
 ************************************************************************/
int Set_UART_Mode(int state)
{
  int i = 100;

  if(state == ON)
  {
    Write_MPU401_Cmd(MPU401_UART);        // turn on UART mode
    while((i > 0) && (Read_MPU401_Data() != MPU401_CMDOK))
      i--;                                // offer timeout instead of lockup
  }
  else
    Write_MPU401_Cmd(MPU401_RESET);       // turn off UART mode

  if(i == 0)
    return(ERROR);
  else
    return(OK);
}

/************************************************************************
 * DETECT_MIDI: Detects the presence of the MPU-401 port or the SBMIDI
 *              port.
 *
 * Input: none
 *
 * Output: result - (0 = OK if found, -1 = ERROR if not found)
 ************************************************************************/
int Detect_MIDI(void)
{
  unsigned char status;
  int result = ERROR;

  if(MIDIType == MPU401)
  {
    // check write status
    status = inp(blastInfo.midiPort + 1);

    if(!(status & MPU401_OK2WR))
    {
      Set_UART_Mode(OFF);                 // just in case it was left on
      result = Reset_MPU401();            // does reset work?
    }
  }
  else
    result = DSPReset();                  // reset the DSP

  return(result);
}

/************************************************************************
 * INIT_MIDI: Initializes the MIDI port.  If MPU-401 port then reset
 *            the port and select UART mode.  For either MPU-401 or
 *            SBMIDI ports set all master volumes to 100, turn off all
 *            notes, and reset all pitch bends.
 *
 * Input: none
 *
 * Output: result - (0 = OK, -1 = ERROR)
 ************************************************************************/
int Init_MIDI(void)
{
  int i;

  if(MIDIType == MPU401)
  {
    if(Reset_MPU401() == ERROR)           // reset port
      return(ERROR);

    if(Set_UART_Mode(ON) == ERROR)        // set UART mode
      return(ERROR);
  }

  for(i = 1; i <= 16; i++)
  {
    Set_MIDI_Vol(i, 100);                 // set channel volume
    Set_MIDI_Notes_Off(i);                // turn off all notes
    Set_MIDI_Pitch_Bend(i, 0);            // reset pitch bends
  }
  return(OK);
}

/************************************************************************
 * EXIT_MIDI: Terminates MIDI driver by turning off all notes (and
 *            turning off UART mode if using MPU-401 port).
 *
 * Input: none
 *
 * Output: none
 ************************************************************************/
void Exit_MIDI(void)
{
  int i;

  for(i = 0; i < 16; i++)
    Set_MIDI_Notes_Off(i);                // turn off all notes

  if(MIDIType == MPU401)
    Set_UART_Mode(OFF);                   // exit UART mode
}
