; --------------------------------------------------------------------------
; THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
; KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
; IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
; PURPOSE.
;
; You have a royalty-free right to use, modify, reproduce and
; distribute the Sample Files (and/or any modified version) in
; any way you find useful, provided that you agree that
; Creative has no warranty obligations or liability for any Sample Files.
;----------------------------------------------------------------------------

; ---------------------------------------------------------------------------
; Program:  Sound Blaster 16 MIDI Utilities
; Filename: MIDIUTIL.ASM
; Author:   Scott E. Sindorf
; Language: Borland TASM
; Date:     30 Jun 93
;
; Copyright (c) 1993-1996 Creative Labs, Inc.
; ---------------------------------------------------------------------------

SBMIDI         equ   0
MPU401         equ   1

; MPU-401 definitions

MPU401_RESET   equ   0ffh
MPU401_UART    equ   03fh
MPU401_CMDOK   equ   0feh
MPU401_OK2WR   equ   040h
MPU401_OK2RD   equ   080h

; SB-MIDI definitions

MIDI_IN_P      equ   30h                  ; MIDI read (polling mode)
MIDI_IN_I      equ   31h                  ; MIDI read (interrupt mode)
MIDI_UART_P    equ   34h                  ; MIDI UART mode (polling mode)
MIDI_UART_I    equ   35h                  ; MIDI UART mode (interrupt mode)
MIDI_UART_TS_P equ   36h                  ; same as 0x34 with timestamp
MIDI_UART_TS_I equ   37h                  ; same as 0x35 with timestamp
MIDI_OUT_P     equ   38h                  ; MIDI write (polling mode)

; SB Port definitions

DSP_RST        equ   06h                  ; DSP reset port
DSP_RD_ST      equ   0eh                  ; DSP read buffer status port
DSP_RD         equ   0ah                  ; DSP read port
DSP_WR_ST      equ   0ch                  ; DSP write buffer status port
DSP_WR         equ   0ch                  ; DSP write port
DSP_OK2WR      equ   80h
DSP_OK2RD      equ   80h


ON             equ   1
OFF            equ   0
ERROR	       equ   -1
OK	       equ   0

	       .MODEL small, C
	       INCLUDE SBCUTILS.INC

	       PUBLIC Write_MPU401_Cmd
	       PUBLIC Write_MPU401_Data
	       PUBLIC Write_SBMIDI_Data
	       PUBLIC Read_MPU401_Data
	       PUBLIC Read_SBMIDI_Data
	       PUBLIC Send_MIDI_Message
	       PUBLIC Set_MIDI_Vol
	       PUBLIC Set_MIDI_Notes_Off
	       PUBLIC Set_MIDI_Pitch_Bend
	       PUBLIC Reset_MPU401
	       PUBLIC Set_UART_Mode
	       PUBLIC Detect_MIDI
	       PUBLIC Init_MIDI
	       PUBLIC Exit_MIDI
	       PUBLIC MIDIType

;-----------------------------------------------------------------------------
;				   DATA
;-----------------------------------------------------------------------------
               .DATA
MIDIType      db    MPU401		 ; MIDI type to use flag
msg            db    3 dup(?)

;-----------------------------------------------------------------------------
;                                  CODE
;-----------------------------------------------------------------------------
               .CODE
;------------------------------------------------------------------------
;  WRITE_MPU401_CMD: sends a command to the MPU401 port.
;
;  Input: command - command byte
;
;  Output: none
;------------------------------------------------------------------------
Write_MPU401_Cmd    PROC command:BYTE

	       push ax
               push dx

	       mov dx, blastInfo.midiPort ; MPU-401 base I/O address
               inc dx                     ; status port
Busy1:
               in al, dx                  ; read status port
               test al, MPU401_OK2WR      ; ready for output?
               jnz Busy1                  ; nope

	       mov al, command		  ; get command byte
               out dx, al                 ; write command byte

               pop dx
               pop ax
               ret

Write_MPU401_Cmd    ENDP

;------------------------------------------------------------------------
;  WRITE_MPU401_DATA: sends a data byte to the MPU401 port.
;
;  Input: data - data byte
;
;  Output: none
;------------------------------------------------------------------------
Write_MPU401_Data    PROC data:BYTE

               push ax
               push dx

	       mov dx, blastInfo.midiPort ; MPU-401 base I/O address
               inc dx                     ; status port
Busy2:
               in al, dx                  ; read status port
               test al, MPU401_OK2WR      ; ready for output?
               jnz Busy2                  ; nope

	       mov al, data		  ; get data byte
	       dec dx			  ; data port
	       out dx, al		  ; write data byte

               pop dx
               pop ax
               ret

Write_MPU401_Data    ENDP

;------------------------------------------------------------------------
;  WRITE_SBMIDI_DATA: sends a byte to the Sound Blaster MIDI port.
;
;  Input: data - data byte
;
;  Output: none
;------------------------------------------------------------------------
Write_SBMIDI_Data    PROC data:BYTE

	       push ax
               push dx

	       mov dx, blastInfo.baseAddr ; SB base I/O address
               add dl, DSP_WR_ST          ; status port / data port
Busy3:
               in al, dx                  ; read status port
               test al, DSP_OK2WR         ; ready for output?
               jnz Busy3                  ; nope

               mov al, MIDI_OUT_P         ; "MIDI out" command
               out dx, al                 ; send "MIDI out" command
Busy4:
               in al, dx                  ; read status port
               test al, DSP_OK2WR         ; ready for output?
               jnz Busy4                  ; nope

	       mov al, data		  ; get data byte
               out dx, al                 ; write data byte

               pop dx
               pop ax
               ret

Write_SBMIDI_Data    ENDP

;------------------------------------------------------------------------
;  READ_MPU401_DATA: reads a byte from the MPU401 port.
;
;  Input: none
;
;  Output: AL - data byte
;------------------------------------------------------------------------
Read_MPU401_Data    PROC

	       push dx

	       mov dx, blastInfo.midiPort ; MPU-401 base I/O address
               inc dx                     ; status port
Busy5:
               in al, dx                  ; read status port
               test al, MPU401_OK2RD      ; MIDI data available?
               jnz Busy5                  ; nope

               dec dx                     ; data port
               in al, dx                  ; read data byte / clear MIDI int.

               pop dx
               ret

Read_MPU401_Data    ENDP

;------------------------------------------------------------------------
;  READ_SBMIDI_DATA: reads a byte from the Sound Blaster MIDI port.
;
;  Input: none
;
;  Output: AL - data byte
;------------------------------------------------------------------------
Read_SBMIDI_Data    PROC

               push dx

	       mov dx, blastInfo.baseAddr ; SB base I/O address
               add dl, DSP_WR_ST          ; status port / data port
Busy6:
               in al, dx                  ; read status port
               test al, DSP_OK2WR         ; ready for output?
               jnz Busy6                  ; nope

               mov al, MIDI_IN_P          ; "MIDI in" command
               out dx, al                 ; send "MIDI in" command

	       mov dx, blastInfo.baseAddr ; SB base I/O address
               add dl, DSP_RD_ST          ; status port
Busy7:
               in al, dx                  ; read status port
               test al, DSP_OK2RD         ; MIDI data available?
               jz Busy7                   ; nope

	       mov dx, blastInfo.baseAddr ; SB base I/O address
               add dl, DSP_RD             ; data port
               in al, dx                  ; read data byte
               mov ah, al                 ; save it

               mov dx, blastInfo.baseAddr ; SB base I/O address
               add dl, DSP_WR_ST          ; status port / data port
Busy8:
               in al, dx                  ; read status port
               test al, DSP_OK2WR         ; ready for output?
               jnz Busy8                  ; nope

               mov al, MIDI_IN_P          ; "MIDI in" command
               out dx, al                 ; turn off "MIDI in" command

               mov al, ah                 ; retrieve data byte read

               pop dx
               ret

Read_SBMIDI_Data    ENDP

;------------------------------------------------------------------------
;  SEND_MIDI_MESSAGE: Sends a MIDI message to port
;
;   Input: message - MIDI message to send
;	   size - length of message
;
;  Output: none
;------------------------------------------------------------------------
Send_MIDI_Message    PROC message:FAR PTR BYTE, msgSize:WORD

	       push cx
	       push si
	       push es

               mov cx, msgSize
	       mov si, SEG message
	       mov es, si
	       mov si, OFFSET message

	       cld			  ; count up

	       cmp MIDIType, MPU401	  ; are we using MPU-401?
               jne WrSBMIDI               ; nope

WrMPU401:      push es:[si]               ; get byte
	       call Write_MPU401_Data	  ; send it via MPU-401
	       add sp, 2		  ; cleanup stack
	       inc si			  ; next byte in message
               loop WrMPU401              ; do it again
               jmp short ExitSend
WrSBMIDI:
	       push es:[si]		  ; get byte
	       call Write_SBMIDI_Data	  ; send it via SBMIDI
	       add sp, 2		  ; cleanup stack
	       inc si			  ; next byte in message
               loop WrSBMIDI              ; do it again
ExitSend:
	       pop es
	       pop si
	       pop cx
               ret

Send_MIDI_Message    ENDP

;------------------------------------------------------------------------
;  SET_MIDI_VOL: Sets the global volume on the requested channel
;
;  Input: channel - set volume on this channel (1 to 16)
;	  volume - set channel to this volume (0 to 127)
;
;  Output: none
;
;  note: if inputs are out of range then no action occurs
;------------------------------------------------------------------------
Set_MIDI_Vol	PROC channel:BYTE, volume:BYTE

	       push ax

	       cmp channel, 16		  ; test channel number for
               ja BadVal1                 ;  proper range
	       cmp channel, 0
               jbe BadVal1
	       cmp volume, 127		  ; test volume value for
               ja BadVal1                 ;  proper range
	       cmp volume, 0
               jb BadVal1

	       mov al, channel		  ; create MIDI message
	       mov [msg], al
	       add [msg], 0b0h
	       dec [msg]
	       mov [msg + 1], 7
	       mov al, volume
	       mov [msg + 2], al

	       push 3			  ; get message size
	       push SEG msg		  ; get MIDI message pointer
	       push OFFSET msg
	       call Send_MIDI_Message
	       add sp, 6		  ; cleanup stack
BadVal1:
	       pop ax
               ret

Set_MIDI_Vol	ENDP

;------------------------------------------------------------------------
;  SET_MIDI_NOTES_OFF: Turns off all notes on the given channel
;
;  Input: channel - turns off all notes on this channel (1 to 16)
;
;  Output: none
;
;  note: if channel is out of range then no action occurs
;------------------------------------------------------------------------
Set_MIDI_Notes_Off    PROC channel:BYTE

	       push ax

	       cmp channel, 16		  ; test channel number for
               ja BadVal2                 ;  proper range
	       cmp channel, 0
               jbe BadVal2

	       mov al, channel		  ; create MIDI message
	       mov [msg], al
	       add [msg], 0b0h
	       dec [msg]
	       mov [msg + 1], 7bh
	       mov [msg + 2], 0

	       push 3			  ; get message size
	       push SEG msg		  ; get MIDI message pointer
	       push OFFSET msg
	       call Send_MIDI_Message
	       add sp, 6		  ; cleanup stack
BadVal2:
	       pop ax
               ret

Set_MIDI_Notes_Off    ENDP

;------------------------------------------------------------------------
;  SET_MIDI_PITCH_BEND: Sets pitch bend (0 = OFF).
;
;  Input: channel - sets pitch bend on this channel (1 to 16)
;	  bend - pitch bend value (-8192 to 8191)
;  Input: AH - sets pitch bend on this channel (1 to 16)
;         BX - pitch bend value (-8192 to 8191)
;
;  Output: none
;
;  note: if inputs are out of range then no action occurs
;------------------------------------------------------------------------
Set_MIDI_Pitch_Bend    PROC channel:BYTE, bend:WORD

	       push ax

	       cmp channel, 16		  ; test channel number for
               ja BadVal3                 ;  proper range
	       cmp channel, 0
               jbe BadVal3
	       cmp bend, 8191		  ; test volume value for
               jg BadVal3                 ;  proper range
	       cmp bend, -8192
               js BadVal3

	       mov al, channel		  ; create MIDI message
	       mov [msg], al
	       add [msg], 0e0h
	       dec [msg]
	       mov ax, bend		  ; get bend value
	       add ax, 8192		  ; make it two 7 bit values
	       shl ax, 1
	       shr al, 1
	       mov WORD PTR [msg + 1], ax

	       push 3			  ; get message size
	       push SEG msg		  ; get MIDI message pointer
	       push OFFSET msg
	       call Send_MIDI_Message
	       add sp, 6		  ; cleanup stack
BadVal3:
	       pop ax
               ret

Set_MIDI_Pitch_Bend    ENDP

;------------------------------------------------------------------------
;  RESET_MPU401: resets the MPU401 port.
;
;  Input: none
;
;  Output: AX - (0 = OK, -1 = ERROR)
;
;  note: Error will occur if UART mode is not turned off first.
;------------------------------------------------------------------------
Reset_MPU401	PROC

	       push cx

	       push MPU401_RESET	  ; Reset command
	       call Write_MPU401_Cmd	  ; send it via MPU-401
	       add sp, 2		  ; cleanup stack

               sub cx, cx                 ; Maximum of 65536 tries
Empty1:
	       call Read_MPU401_Data
               cmp al, MPU401_CMDOK       ; successful reset?
               je ResetOK                 ; success!
               loop Empty1                 ; try again

	       mov ax, ERROR
               jmp short ExitReset
ResetOK:
	       mov ax, OK
ExitReset:
               pop cx
               ret

Reset_MPU401	ENDP

;------------------------------------------------------------------------
;  SET_UART_MODE: puts the MPU401 port in UART mode.
;
;  Input: state - ON or OFF (Reset will return error if UART mode is on
;                 when reset occurs)
;
;  Output: AX - (0 = OK, -1 = ERROR)
;------------------------------------------------------------------------
Set_UART_Mode	 PROC state:WORD

               push cx

	       cmp state, OFF		  ; turn UART mode off?
               je TurnOff                 ; yes

	       push MPU401_UART 	  ; UART mode command
	       call Write_MPU401_Cmd	  ; send it via MPU-401
	       add sp, 2		  ; cleanup stack

               sub cx, cx                 ; Maximum of 65536 tries
Empty2:
	       call Read_MPU401_Data
	       cmp al, MPU401_CMDOK	  ; successful mode change?
               je UARTModeOK              ; success!
               loop Empty2                ; try again

	       mov ax, ERROR
               jmp short ExitSetUART
TurnOff:
               push MPU401_RESET          ; turn off UART mode with reset
               call Write_MPU401_Cmd      ; (will not return acknowledge)
	       add sp, 2		  ; cleanup stack
UARTModeOK:
	       mov ax, OK
ExitSetUART:
               pop cx
               ret

Set_UART_Mode	 ENDP

;------------------------------------------------------------------------
;  DETECT_MIDI: Detects the presence of the MPU-401 port or the SBMIDI
;		port.
;
;  Input: none
;
;  Output: AX - (0 = OK, -1 = ERROR)
;------------------------------------------------------------------------
Detect_MIDI    PROC

               push dx

	       cmp MIDIType, MPU401	  ; are we using MPU-401?
               jne DtSBMIDI               ; nope

	       mov dx, blastInfo.midiPort ; MPU-401 base I/O address
               inc dx                     ; status port
               in al, dx                  ; read status port
               test al, MPU401_OK2WR      ; ready for output?
               jnz DtError                ; nope

	       push OFF 		  ; turn off UART mode just in case
	       call Set_UART_Mode
	       add sp, 2		  ; cleanup stack

	       call Reset_MPU401	  ; reset the MPU-401 port
	       or ax, ax		  ; was reset OK?
               jnz DtError                ; nope

	       mov ax, OK
               jmp short DtExit
DtSBMIDI:
	       call DSPReset		  ; try to reset DSP
               jmp short DtExit
DtError:
	       mov ax, ERROR
DtExit:
               pop dx
               ret

Detect_MIDI    ENDP

;------------------------------------------------------------------------
;  INIT_MIDI: Initializes the MIDI port.  If MPU-401 port then reset
;	      the port and select UART mode.  For either MPU-401 or
;	      SBMIDI ports set all master volumes to 100, turn off all
;	      notes, and reset all pitch bends.
;
;  Input: none
;
;  Output: AX - (0 = OK, -1 = ERROR)
;------------------------------------------------------------------------
Init_MIDI    PROC
               push cx

	       cmp MIDIType, MPU401	  ; are we using MPU-401?
               jne ItSBMIDI               ; nope

	       call Reset_MPU401	  ; reset MPU401 port
	       or ax, ax		  ; was reset OK?
               jnz ItError                ; nope

	       push ON			  ; turn on UART mode
	       call Set_UART_Mode
	       add sp, 2		  ; cleanup stack
	       or ax, ax		  ; was mode change OK?
               jnz ItError                ; nope
ItSBMIDI:
               mov cx, 16                 ; for sixteen channels
ItNextChl:
	       push 100 		  ; set volume to 100 (GM default)
	       push cx
	       call Set_MIDI_Vol

               call Set_MIDI_Notes_Off    ; turn off all notes
	       add sp, 4		  ; cleanup stack
	       push 0			  ; clear all pitch bends
	       push cx
	       call Set_MIDI_Pitch_Bend
	       add sp, 4		  ; cleanup stack
               loop ItNextChl             ; do next channel

	       mov ax, OK
               jmp short ItExit
ItError:
	       mov ax, ERROR
ItExit:
               pop cx
               ret

Init_MIDI    ENDP

;------------------------------------------------------------------------
;  EXIT_MIDI: Terminates MIDI driver by turning off all notes (and
;	      turning off UART mode if using MPU-401 port).
;
;  Input: none
;
;  Output: none
;------------------------------------------------------------------------
Exit_MIDI    PROC
               push cx

               mov cx, 16                 ; for sixteen channels
ExNextChl:
	       push cx
	       call Set_MIDI_Notes_Off	  ; turn off all notes
	       add sp, 2		  ; cleanup stack
               loop ExNextChl             ; do next channel

	       cmp MIDIType, MPU401	  ; are we using MPU-401?
               jne ExExit                 ; nope

	       push OFF 		  ; turn off UART mode
	       call Set_UART_Mode
	       add sp, 2		  ; cleanup stack
ExExit:
               pop cx
               ret

Exit_MIDI    ENDP

             END

