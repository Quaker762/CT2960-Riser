BORLAND = YES
#BORLAND = NO

ASSEMBLY = YES
#ASSEMBLY = NO

TARGET : MIDITEST.EXE

!if "$(BORLAND)" == "YES"
MIDITEST.OBJ : MIDITEST.C MIDIUTIL.H SBCUTILS.H
   bcc -ms -c -v -IC:\BORLANDC\INCLUDE miditest.c

!if "$(ASSEMBLY)" == "NO"
MIDIUTIL.OBJ : MIDIUTIL.C MIDIUTIL.H
	 bcc -ms -c -v -IC:\BORLANDC\INCLUDE midiutil.c
!else
MIDIUTIL.OBJ : MIDIUTIL.ASM SBCUTILS.INC
    tasm -ml -w2 -zi midiutil;
!endif

SBCUTILS.OBJ : SBCUTILS.C SBCUTILS.H
   bcc -ms -c -v -IC:\BORLANDC\INCLUDE sbcutils.c

MIDITEST.EXE : MIDITEST.OBJ MIDIUTIL.OBJ SBCUTILS.OBJ
   tlink /v /Td /c /x /LC:\BORLANDC\LIB C:\BORLANDC\LIB\c0s miditest \
   midiutil sbcutils,miditest,,cs;
#-------------------------------------------------------------------------
!else
MIDITEST.OBJ : MIDITEST.C MIDIUTIL.H SBCUTILS.H
   cl /AS /c /Zi miditest.c

!if "$(ASSEMBLY)" == "NO"
MIDIUTIL.OBJ : MIDIUTIL.C MIDIUTIL.H
   cl /AS /c /Zi midiutil.c
!else
MIDIUTIL.OBJ : MIDIUTIL.ASM SBCUTILS.INC
    masm /Ml /w2 /Zi midiutil;
!endif

SBCUTILS.OBJ : SBCUTILS.C SBCUTILS.H
   cl /AS /c /Zi sbcutils.c

MIDITEST.EXE : MIDITEST.OBJ MIDIUTIL.OBJ SBCUTILS.OBJ
   link /CO /NOI miditest midiutil sbcutils,miditest;
!endif
