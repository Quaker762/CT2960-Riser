                   Sound Blaster 16 MIDI programming
          -----------------------------------------------------
          using the MPU-401 port or the Sound Blaster MIDI port
          -----------------------------------------------------

                               30 Jun 93

                 Copyright (c) 1993-1996 Creative Labs, Inc.
-----------------------------------------------------------------------
Summary:

The MIDI directory on this disk contains various functions to implement
MPU-401 MIDI on the Sound Blaster 16.

File List:
            SBCUTILS.C
            SBCUTILS.H
            MIDIUTIL.H
            MIDIUTIL.C
            MIDITEST MAK
            MIDITEST.C
            MIDITEST.EXE


SBCUTILS.C, SBCUTILS.H
----------------------
Contains functions, data structures, and constants that are universal
across all the Sound Blaster cards.

Functions:
    int GetBlastInfo(BLASTREC *blastInfo)
        description: retrieves and parses the BLASTER environment variable.

    int DSPReset(void)
        description: resets the Sound Blaster DSP.

MIDIUTIL.C, MIDIUTIL.H
----------------------
Contains functions and constants to access the MPU-401 port on the SB16.

Functions:
    void Write_MPU401_Cmd(unsigned char command)
        description: writes a command to the MPU-401 port.

    void Write_MPU401_Data(unsigned char data)
        description: writes data to the MPU-401 port.

    void Write_SBMIDI_Data(unsigned char data)
        description: writes data to the Sound Blaster MIDI port.

    unsigned char Read_MPU401_Data(void)
        description: reads data from the MPU-401 port.

    unsigned char Read_SBMIDI_Data(void)
        description: reads data from the Sound Blaster MIDI port.

    void Send_MIDI_Message(char *message, int size)
        description: sends a variable length message to the MIDI port.

    void Set_MIDI_Vol(char channel, char volume)
        description: sets a MIDI channel's volume.

    void Set_MIDI_Notes_Off(char channel)
        description: turns off all notes on a given channel.

    void Set_MIDI_Pitch_Bend(char channel, int range)
        description: sets the pitch bend value for a given channel.

    int Reset_MPU401(void)
        description: resets the MPU-401 port.

    int Set_UART_Mode(int state)
        description: switches UART mode on and off on the MPU-401 port.

    int Detect_MIDI(void)
        description: verifies the existence of the MIDI hardware
                     defined in the Blaster Information Record (blastInfo).

    int Init_MIDI(void)
        description: for the MPU-401 port, resets the port and selects
                     UART mode.  For both MPU-401 and SBMIDI, sets the
                     channel volumes to 100, turns off any notes that were
                     left hanging, and sets the pitch bend values to 0.

    void Exit_MIDI(void)
        description: provides house keeping before exiting the program.
                     This function turns off all notes and if using the
                     MPU-401 port then it turns off UART mode.

MIDITEST.C
----------
Test routine demonstrating the use of

    GetBlastInfo()
    Detect_MIDI()
    Init_MIDI()
    Exit_MIDI()

All other routines are utilized in these functions.
