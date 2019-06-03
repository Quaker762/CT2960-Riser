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


;----------------------------------------------------------------------------
; Program:	Sound Blaster Utilities
; Filename: SBCUTILS.INC
; Author: 	Scott E. Sindorf
; Language: Borland TASM
; Date: 		30 Jun 93
;
; Copyright (c) 1993-1996 Creative Labs, Inc.
;----------------------------------------------------------------------------

BLASTREC STRUC													 ; BLASTER environment record
	baseAddr		 dw 	 0
	DSPint			 db 	 0
	DMA8				 db 	 0
	model 			 db 	 0
	DMA16 			 db 	 0
	midiPort		 dw 	 0
BLASTREC ENDS

EXTRN C blastInfo:BLASTREC

;------------------------
; FUNCTION DECLARATIONS
;------------------------

EXTRN C GetBlastInfo:PROC
EXTRN C DSPReset:PROC
