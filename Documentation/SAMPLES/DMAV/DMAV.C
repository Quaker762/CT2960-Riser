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
/*************************************************************************
*
* TITLE: DMAV.C
*
* AUTHOR: Tom Bouril  (October 1994)
*
* DANGER: Niether author of this program nor Creative Labs, Inc. is
*         responsible for any problem(s) that may occur as a result of
*         using this code.
*
* COMPILER: Turbo C/C++ Version 1.01.  (Works with Borland C/C++).  This
*           program uses only C code.  No C++ instructions are used.  The
*           only C++ code used is the double slash (//) comments.
*
* DESCRIPTION:  This program will play 8-bit and 16-bit .VOC files of type
*               PCM and CTADPCM.  Only block types 0, 1, 8, and 9 are
*               supported.  This program will detect the card's DSP version
*               and program the card according to its type.  All that is
*               required is a BLASTER environment variable containing the
*               following information.  Make sure the jumper settings of
*               the card reflect the numbers in the BLASTER environment.
*
*               SET BLASTER=A2w0 Ix Dy Hz
*
*               Where:  A represents the base I/O address of the SB.
*                       I represents the interrupt request number of the SB.
*                       D represents the 8-bit DMA channel number used by SB.
*                       H represents the 16-bit DMA channel number used by SB.
*                       w = 2, 4, 6, or 8
*                       x = 2, 3, 5, 7, 10
*                       y = 0, 1, or 3
*                       z = 5, 6, or 7
*
* NOTE: If you want to play 16-bit CTADPCM files you need a Sound Blaster
*       SB16 or AWE32 with the CSP chip.  In order to use the CSP chip,
*       you must do: #define USE_CSP.  Without that #define statement,
*       the CSP relevant code is not compiled.  The CSP.SYS driver must
*       also be loaded via CONFIG.SYS upon boot up.
*
* INSTRUCTIONS:  Enter the following on the command line.
*
*                >DMAV filename.VOC 2000
*
*                The 2000 is the number of bytes of the DMA buffer.  You
*                may give it any value, but it's value is not a multiple
*                of 8, it will be bumped up to the nearest multiple of 8.
*
*                The file being played can be paused and resumed by
*                hitting the spacebar key.  The program can be exited
*                by pressing the Esc or 'Q' key.
*
*
* RECOMMENDED DMA BUFFER SIZES
* ----------------------------
*
* The following is a chart of recommended DMA buffer sizes required
* for the types of files you may need to play.  On faster systems (i.e., 486)
* very small DMA buffer sizes (less than 50 bytes) may work well for low
* sample rate files (i.e., 11025 samples per sec., mono); however, for
* safe tolerances, make the DMA buffer much larger.  These are suggestions.
* Please experiment.  If the sound you hear is distorted (i.e., skipping,
* clicking, etc.) or the program hangs, try increasing gDMABufSize.
*
*
* Transfer Rate = Channels * Samples Per Second * Bits Per Sample / 8
*
* --------------------------------------------------------------------------
*    Transfer Rate  |                                         |
*  (Bytes Per Sec.) | Example                                 | gDMABufSize
* --------------------------------------------------------------------------
*         176,400   | 16-bit, STEREO, 44,100 Samples Per Sec. | 16,384 bytes
* --------------------------------------------------------------------------
*          88,200   | 16-bit, MONO,   44,100 Samples Per Sec. |  8,192 bytes
*                   |  8-bit, STEREO, 44,100 Samples Per Sec. |
* --------------------------------------------------------------------------
*          44,100   |  8-bit, MONO,   44,100 Samples Per Sec. |  4,096 bytes
* --------------------------------------------------------------------------
*          11,025   |  8-bit, MONO,   11,025 Samples Per Sec. |  1,024 bytes
* --------------------------------------------------------------------------
*
* The minimum DMA Buffer size required is dependent upon the system
* (i.e., size of disk cache, fragmentation of audio files, CPU speed,
* and disk access time.  It's possible to play 8-bit, MONO, 11,025
* samples-per-second files (with no embedded .VOC block types) with no
* audible degradation with a DMA buffer size of only 256 bytes on a
* 486 system.
*
*************************************************************************/
#define CSPSYS_ERR_NOERROR            0
#define CSPSYS_ERR_GENERALFAILURE     1
#define CSPSYS_ERR_ACQUIRED           2
#define CSPSYS_ERR_UNACQUIRED         3
#define CSPSYS_ERR_UNSUPPORTEDMSG     4
#define CSPSYS_ERR_INVALIDUSER        5
#define CSPSYS_ERR_INVALIDDEVICEID    6
#define CSPSYS_ERR_UNSUPPORTEDPARAM   7
#define CSPSYS_ERR_INVALIDPARAMVALUE  8

#define CSPSYS_PARAM_DMAWIDTH      5
#define CSPSYS_PARAM_DRIVERVERSION 1
#define CSPSYS_PARAM_NCHANNELS     4

#define CSPSYS_ACQUIRE    1
#define CSPSYS_RELEASE    2
#define CSPSYS_DOWNLOAD   3
#define CSPSYS_START      4
#define CSPSYS_STOP       5
#define CSPSYS_SETPARAM   6
#define CSPSYS_GETPARAM   7


#define AUTO_INIT                  1
#define BITS_PER_SAMPLE_8     0x0400
#define BITS_PER_SAMPLE_16    0x0800
#define BLOCK_0               0x0001  // Bit 0 = Block Type 0
#define BLOCK_1               0x0002  // Bit 1 = Block Type 1
#define BLOCK_2               0x0004  // Bit 2 = Block Type 2
#define BLOCK_8               0x0100  // Bit 8 = Block Type 8
#define BLOCK_9               0x0200  // Bit 9 = Block Type 9
#define COMMAND_PAUSE           0x01
#define COMMAND_PLAY            0x02
#define COMMAND_QUIT            0x04
#define DMA8_FF_REG           0x000C
#define DMA8_MASK_REG         0x000A
#define DMA8_MODE_REG         0x000B
#define DMA16_FF_REG          0x00D8
#define DMA16_MASK_REG        0x00D4
#define DMA16_MODE_REG        0x00D6
#define DSP_DATA_AVAIL        0x000E
#define DSP_GET_VERSION       0x00E1
#define DSP_READ_PORT         0x000A
#define DSP_READY             0x00AA
#define DSP_RESET_PORT        0x0006
#define DSP_WRITE_PORT        0x000C
#define END_OF_INTERRUPT      0x0020
#define FAIL                       0
#define FALSE                      0
#define FILE_NOT_DONE_PLAYING 0x8000
#define INVALID_BLOCK_TYPE    0x4000
#define INVALID_FILE_FORMAT        1
#define PIC0_COMMAND_REG        0x20
#define PIC1_COMMAND_REG        0xA0
#define PIC0_MASK_REG           0x21
#define PIC1_MASK_REG           0xA1
#define REMEMBER_VOLUME            1
#define RESTORE_VOLUME             2
#define SB2_LO                     1
#define SB2_HI                     2
#define SBPRO                      3
#define SB16                       4
#define SINGLE_CYCLE               2
#define SUCCESS                    1
#define TRUE                  !FALSE
#define UNUSED                     0
#define USE_CSP                        // Comment out if not using CSP chip.
#define VOC_FILE                   2
#define WAVE_FILE                  3

#include <conio.h>
#include <ctype.h>
#include <dos.h>
#include <fcntl.h>
#include <io.h>
#include <mem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys\stat.h>

/*------------------- TYPEDEFS -----------------------------------------*/
/*----------------------------------------------------------------------*/
typedef struct _BLASTER
{
  short int BaseIOPort,
	    DMAChan8Bit,
	    DMAChan16Bit,
	    DSPVersion,
	    IRQNumber,
	    MIDIPort;
} BLASTER;


typedef struct _FILEINFO
{
  unsigned char      BitsPerSample,
		     Channels;      // MONO = 1, STEREO = 2
  unsigned short int FileFormat,    // Determines BitsPerSample in old .VOC
		     TimeConstant;  // Old .VOC version of SampPerSec
  signed long int    SampPerSec;
} FILEINFO;

#ifdef USE_CSP
typedef unsigned long int (far *CSP_FUNC_PTR) (unsigned short int,
					       unsigned short int,
					       unsigned long  int,
					       unsigned long  int,
					       unsigned long  int);
#endif


typedef unsigned long int DWORD;

/*------------------- FUNCTION PROTOTYPES ------------------------------*/
/*----------------------------------------------------------------------*/
char GetBlastEnv(BLASTER *Blast),
     ResetDSP(short int),
#ifdef USE_CSP
     AcquireCSP(CSP_FUNC_PTR, unsigned short int),
     GetCSPDriverAddr(CSP_FUNC_PTR *),
     ReleaseCSP(CSP_FUNC_PTR),
#endif
     VerifyFileType(FILE *);


short int DSPRead(short int),
	  LoadAndPlay(unsigned long int, FILE *, unsigned char *, char
#ifdef USE_CSP
		      , CSP_FUNC_PTR*
#endif
		     );

unsigned long int AllocateDMABuffer(unsigned char **, unsigned short int *),
		  OnSamePage(unsigned char *, unsigned short int);

void interrupt DmaISR(void);
void ContinueDMA(unsigned char),
     DSPWrite(short int, short int),
     KillVolume(void),
     PauseDMA(unsigned char),
     ProgramDMA(unsigned long int, FILEINFO *, unsigned short int),
     ProgramDSP(unsigned short int, FILEINFO *, unsigned char),
     RestoreOldISR(BLASTER *),
     RestoreOrRememberVolume(char),
     SetDmaISR(BLASTER *),
     SetMixer(void);

/*------------------- GLOBAL VARIABLES ---------------------------------*/
/*----------------------------------------------------------------------*/
BLASTER gBlastSet;
#ifdef USE_CSP
char              gCSPAcquired     = FALSE,
		 *gCSPUserFileName = "CSPUSER.TMP";
#endif

signed short int   gBitsPerSample,
		   gChannels;


unsigned short int gDMABufNowPlaying,
		   gDMABufSize,       // See "WARNING #1" above.
		   gHighSpeed,
		   gIRQMaskSave0,
		   gIRQMaskSave1;

unsigned long int  gUserNo;

signed long int gBytesLeftToPlay;

void interrupt (*gOldISRFuncPtr)(void);

/*************************************************************************
*
* FUNCTION: main()
*
* DESCRIPTION: READ IT!
*
*************************************************************************/
void main(short int argc, char *argv[])
{
  char               Command,
		     KeyPressed;
  FILE              *File;
  short int          Status;
  unsigned char     *DMABuf;
  unsigned long int  DMABufPhysAddr;
#ifdef USE_CSP
  CSP_FUNC_PTR       CSPFunc = 0;
#endif

  clrscr();

  if (argc != 3)
  {
    puts("Must have at exactly 2 parameters--Filename and DMA buffer size");
    goto exit_0;;
  }

  /*--- GET BLASTER ENVIRONMENT VARIABLES AND DSP VERSION --------------*/
  if (GetBlastEnv(&gBlastSet) == FAIL)
  {
    puts("GetBlastEnv() FAILS!");
    goto exit_0;
  }


  /*--- OPEN THE FILE TO PLAY OR RECORD --------------------------------*/
  printf("Filename = %s\n", argv[1]);
  File = fopen(argv[1], "rb");  // Open file for playing.
  if (File == 0)
  {
    puts("fopen() FAILS!");
    goto exit_0;
  }

  /*--- CONVERT COMMAND-LINE PARAMETER TO DMA BUFFER SIZE. -------------*/
  gDMABufSize = (unsigned short int) atoi(argv[2]);


  /*--- ALLOCATE DMA BUFFER --------------------------------------------*/
  /*--------------------------------------------------------------------*/
  DMABufPhysAddr = AllocateDMABuffer(&DMABuf, &gDMABufSize);
  if (DMABufPhysAddr == FAIL)
  {
    puts("AllocateDMABuffer() FAILS!");
    goto exit_1;
  }

    /*--- THE FOLLOWING printf()s ARE FOR DEBUG ONLY! --------------------*/
//  /*
  printf("DMA Buffer = %u bytes.\n", gDMABufSize);
  printf("I/O        = %x (hex)\n",  gBlastSet.BaseIOPort);
  printf("DMA 8-bit  = %d\n",        gBlastSet.DMAChan8Bit);
  printf("DMA 16-bit = %d\n",        gBlastSet.DMAChan16Bit);
  printf("IRQ        = %d\n",        gBlastSet.IRQNumber);
  printf("DSP Ver.   = %d.%02d\n",  (gBlastSet.DSPVersion >> 8) & 0x00FF,
				    (gBlastSet.DSPVersion & 0x00FF));
//  */


  puts("PRESS:  Space Bar  to Pause/Resume");
  puts("PRESS:  Esc or' Q' to Quit");


  SetDmaISR(&gBlastSet);

  /*--- REMOVE THIS FUNCTION!!!  IT FORCES MIXER VOLUME TO MAXIMUM!!! --*/
  /*--------------------------------------------------------------------*/
  SetMixer();

  if (VerifyFileType(File) != VOC_FILE)
  {
    printf("File: %s not a .VOC file--ABORT!\n", argv[1]);
    goto exit_2;
  }

  Command = COMMAND_PLAY;  // "Command" MUST equal this to begin playing!

  do
  {
    Status = LoadAndPlay(DMABufPhysAddr, File, DMABuf, Command
#ifdef USE_CSP
			 , &CSPFunc
#endif
			);

    /*--- DETECT KEYS HIT.  PAUSE, RESUME, AND QUIT SUPPORTED. -------*/
    /*----------------------------------------------------------------*/
    if (kbhit())
    {
      KeyPressed = getch();

      if (KeyPressed == 0)     // If 1st byte is 0, key pressed sent 2 bytes.
	KeyPressed = getch();  // Flush the 2nd byte from the buffer.

      switch(KeyPressed)
      {
	case 32:   // Spacebar
	  if (Command == COMMAND_PLAY)        // If playing, pause.
	  {
	    Command = COMMAND_PAUSE;

	    if (Status & BITS_PER_SAMPLE_8)
	      PauseDMA(8);
	    else  // BitsPerSample = 16
	      PauseDMA(16);
	  }
	  else if (Command == COMMAND_PAUSE)  // If paused, continue play.
	  {
	    Command = COMMAND_PLAY;

	    if (Status & BITS_PER_SAMPLE_8)
	      ContinueDMA(8);
	    else
	      ContinueDMA(16);  // BitsPerSample == 16
	  }
	break;

	case 'Q': // 'Q' is for Quit
	case 'q':
	case 27 : // Escape key
	  Command = COMMAND_QUIT;
	break;
      }
    }

  } while (Status & FILE_NOT_DONE_PLAYING);


#ifdef USE_CSP
  if (gCSPAcquired == TRUE)
  {
    ReleaseCSP(CSPFunc);
    gCSPAcquired = FALSE;
  }
#endif

  /*--- This should never happen, but test for it just in case! --------*/
  /*--------------------------------------------------------------------*/
  if (Status & INVALID_BLOCK_TYPE)
    puts("CRASH!  Unsupported block type detected!");

  RestoreOldISR(&gBlastSet);

exit_2:
  free(DMABuf);
exit_1:
  fclose(File);
exit_0:
  return;
}


/*************************************************************************
*
* FUNCTION: LoadAndPlay()
*
* DESCRIPTION:
*
* RETURN: "ReturnStatus", where the bits (bit 0 to bit 15) have the
*         significance shown.
*
*         Bit No.     | If bit set HI (equals 1), it means...
*         ---------------------------------------------------
*          0 (LSBit)  | Block 0 detected (File Done Playing)
*          1          | Block 1 detected
*          2 - 7      | Unused
*          8          | Block 8 detected
*          9          | Block 9 detected
*          10         | BitsPerSample = 8
*          11         | BitsPerSample = 16
*          12 - 13    | Unused
*          14         | Invalid Block Type detected (Fatal Error)
*          15 (MSBit) | File NOT done playing
*
*************************************************************************/
short int LoadAndPlay(unsigned long int   DMABufPhysAddr,
		      FILE               *File,
		      unsigned char      *DMABuf,
		      char                Command
#ifdef USE_CSP
		      , CSP_FUNC_PTR*     CSPFunc
#endif
		      )
{
  static FILEINFO           FileHdrInfo;
  static signed long int    BlockByteCount;
  static char               FirstTimeFuncCalled = TRUE;
  static unsigned short int Count,
			    DMABufToLoad;
  char                      BlockType;
  short int                 ReturnStatus = FILE_NOT_DONE_PLAYING;

  /*--- CHECK FOR COMMANDS SENT AFTER FILE HAS BEGUN PLAYING. ----------*/
  /*--------------------------------------------------------------------*/
  switch(Command)
  {
    case COMMAND_PAUSE:
      if (FileHdrInfo.BitsPerSample == 8)
	ReturnStatus |= BITS_PER_SAMPLE_8;
      else
	ReturnStatus |= BITS_PER_SAMPLE_16;
    return(ReturnStatus);

    case COMMAND_PLAY:
      if (FileHdrInfo.BitsPerSample == 8)
	ReturnStatus |= BITS_PER_SAMPLE_8;
      else
	ReturnStatus |= BITS_PER_SAMPLE_16;
    break;

    case COMMAND_QUIT:
      ResetDSP(gBlastSet.BaseIOPort); // Stops DMA transfer!
      ReturnStatus &= ~FILE_NOT_DONE_PLAYING;  // File IS DONE playing!
      FirstTimeFuncCalled = TRUE;     // Reset for next file.
    return(ReturnStatus);             // RETURN!
  }


  if (FirstTimeFuncCalled == TRUE)
  {
    FirstTimeFuncCalled = FALSE;
    goto read_new_block;
  }
  else
  {
    // Prevent loading DMA buffer when 1/2 is loaded and other 1/2 is playing.
    while (DMABufToLoad == gDMABufNowPlaying);  // WAIT!
  }


  /*--- Point DMABuf to top 1/2 of DMA buffer if needed. ---------------*/
  /*--------------------------------------------------------------------*/
  if (DMABufToLoad == 1)
    DMABuf += (gDMABufSize / 2);  // Load top 1/2 of DMA buffer.

  if (BlockByteCount > gDMABufSize / 2)
  {
    /*--- LOAD EXACTLY 1/2 DMA BUFFER FROM THE FILE. -------------------*/
    /*------------------------------------------------------------------*/
    Count = gDMABufSize / 2;
    fread(DMABuf, Count, 1, File);
    DMABufToLoad   ^= 1;             // Get ready to load next 1/2 DMA buffer.
  }
  else if (BlockByteCount >= 0)
  {
    /*--- LOAD ALL DATA REMAINING IN CURRENT BLOCK INTO DMA BUFFER. ----*/
    /*------------------------------------------------------------------*/
    Count = (unsigned short int) BlockByteCount;
    if (Count > 0)
    {
      fread(DMABuf, Count, 1, File);
      if (Count == gDMABufSize / 2)
	DMABufToLoad ^= 1;  // Get ready to load next 1/2 DMA buffer.

      ProgramDSP(Count, &FileHdrInfo, SINGLE_CYCLE);
    }

read_new_block:

    /*--- READ A NEW BLOCK TYPE AND STORE ITS INFORMATION IN THE -----*/
    /*--- THE LOCAL STATIC STRUCTURE "FileHdrInfo".              -----*/
    /*----------------------------------------------------------------*/
    fread(&BlockType, 1, 1, File);
    switch(BlockType)
    {
      case 0:  // TERMINATOR BLOCK
	puts("BLOCK 0");
	while (gBytesLeftToPlay > 0);   // WAIT for buffer to finish playing.
	ReturnStatus |= BLOCK_0;
	ReturnStatus &= ~FILE_NOT_DONE_PLAYING;  // File IS DONE playing!
	FirstTimeFuncCalled = TRUE;     // Reset for next file.
      break;


      case 1:  // DIGITIZED SOUND BLOCK
	puts("BLOCK 1");
	BlockByteCount = 0L;  // Clears MSByte
	fread(&BlockByteCount,           3, 1, File);
	fread(&FileHdrInfo.TimeConstant, 1, 1, File);
	fread(&FileHdrInfo.FileFormat,   1, 1, File);
	BlockByteCount           -= 2;
	FileHdrInfo.Channels      = 1;  // MONO
	FileHdrInfo.BitsPerSample = 8;
	FileHdrInfo.SampPerSec    = -1000000L /
				    ((long) FileHdrInfo.TimeConstant - 256L);
	ReturnStatus |= BLOCK_1;
      break;


      case 8:  // DIGITIZED SOUND BLOCK
	puts("BLOCK 8");
	fseek(File, 3, SEEK_CUR);        // Skip Block 8 BlockLen field.
	fread(&FileHdrInfo.TimeConstant, 2, 1, File);
	fread(&FileHdrInfo.FileFormat,   1, 1, File);
	fread(&FileHdrInfo.Channels,     1, 1, File);
	FileHdrInfo.Channels++;          // Make MONO = 1, STEREO = 2
	FileHdrInfo.TimeConstant >>= 8;  // Only HI byte is used.


	/*--- BLOCK TYPE 8 IS IMMEDIATELY FOLLOWED BY BLOCK TYPE 1. --*/
	fseek(File, 1, SEEK_CUR);            // Skip Block Type 1 ID.
	BlockByteCount = 0L;                 // Clears MSByte
	fread(&BlockByteCount, 3, 1, File);
	fseek(File, 2, SEEK_CUR);            // Skip next 2 bytes
	BlockByteCount -= 2;

	FileHdrInfo.BitsPerSample = 8;
	FileHdrInfo.SampPerSec    = (-1000000L /
				    ((long) FileHdrInfo.TimeConstant - 256L))
				    / FileHdrInfo.Channels;

	ReturnStatus |= BLOCK_8;
      break;


      case 9:  // DIGITIZED SOUND BLOCK
	puts("BLOCK 9");
	BlockByteCount = 0L;  // Clears MSByte
	fread(&BlockByteCount,            3, 1, File);
	fread(&FileHdrInfo.SampPerSec,    4, 1, File);
	fread(&FileHdrInfo.BitsPerSample, 1, 1, File);
	fread(&FileHdrInfo.Channels,      1, 1, File);
	fread(&FileHdrInfo.FileFormat,    2, 1, File);
	fseek(File, 4, SEEK_CUR);  // Skip reserved bytes.
	BlockByteCount -= 12;
	FileHdrInfo.TimeConstant = (unsigned short int) ((256L - 1000000L)
				   / ((long) FileHdrInfo.Channels *
				   FileHdrInfo.SampPerSec) & 0x00FF);
	ReturnStatus |= BLOCK_9;
      break;

      default:
	ReturnStatus |= INVALID_BLOCK_TYPE;
	ReturnStatus &= ~FILE_NOT_DONE_PLAYING;  // File IS DONE playing!
	FirstTimeFuncCalled = TRUE;              // Reset for next file.
      break;
    }

    switch (BlockType)
    {
      case 0: // Blocktype is block terminator.
	/*--- THE FOLLOWING printf()s ARE FOR DEBUG ONLY. --------------*/
	/*--------------------------------------------------------------*/
	printf("BlockType     = %d\n",  BlockType);
	printf("BitsPerSample = %u\n",  FileHdrInfo.BitsPerSample);
	printf("Channels      = %u\n",  FileHdrInfo.Channels);
	printf("FileFormat    = %u\n",  FileHdrInfo.FileFormat);
	printf("TimeConstant  = %u\n",  FileHdrInfo.TimeConstant);
	printf("SampPerSec    = %ld\n", FileHdrInfo.SampPerSec);
      break;


      case 1:  // Block types 1, 8, 9 are digital sound blocks.
      case 8:
      case 9:
	ResetDSP(gBlastSet.BaseIOPort);  // Always avoid problems--reset!
	gBitsPerSample = FileHdrInfo.BitsPerSample;
	gChannels      = FileHdrInfo.Channels;

#ifdef USE_CSP
	/*--- GET CSP DRIVER ADDRESS AND LOAD CSP CODE. ----------------*/
	/*--------------------------------------------------------------*/
	if (gCSPAcquired == FALSE)
	{
	  switch(FileHdrInfo.FileFormat)
	  {
	    case 0x0006:  // A-LAW     16-bit 2:1 compression
	    case 0x0007:  // MU-LAW    16-bit 2:1 compression
	    case 0x0200:  // CT-ADPCM  16-bit 4:1 compression
	      if (GetCSPDriverAddr(CSPFunc) == FAIL)
	      {
		puts("Can't open CSP.SYS driver.  ABORT!");
		ReturnStatus &= ~FILE_NOT_DONE_PLAYING;
		return(ReturnStatus);
	      }

	      AcquireCSP(*CSPFunc, FileHdrInfo.FileFormat);
	      gCSPAcquired = TRUE;
	    break;
	  }
	}
#endif

	DMABufToLoad      = 1;     // Next 1/2 DMA buffer to load is top 1/2.
	gBytesLeftToPlay  = BlockByteCount;  // Altered by DmaISR().
	gDMABufNowPlaying = 0;     // Altered by ISR when 1/2 buffer done playing.
	gHighSpeed        = FALSE; // Initialize to NOT high-speed DMA.

	/*--- LOAD DMA BUFFER AND BEGIN PLAYING THE FILE. --------*/
	/*--------------------------------------------------------*/
	ProgramDMA(DMABufPhysAddr, &FileHdrInfo, gDMABufSize);

	if (BlockByteCount > gDMABufSize / 2)
	{
	  Count = gDMABufSize / 2;
	  fread(DMABuf, Count, 1, File);
	  ProgramDSP(Count, &FileHdrInfo, AUTO_INIT);  // Begin audio.
	}
	else
	{
	  Count = (int) BlockByteCount;
	  fread(DMABuf, Count, 1, File);
	  ProgramDSP(Count, &FileHdrInfo, SINGLE_CYCLE);  // Begin audio.
	}
      break;
    }

  } // End: else if (BlockByteCount >= 0)

  BlockByteCount -= (long) Count;  // Update No. of bytes left in block.

  return(ReturnStatus);
}

/*************************************************************************
*
* FUNCTION: ProgramDMA()
*
* DESCRIPTION:  This function programs the DMA chip to use a single
*               8-bit or 16-bit DMA channel (specified by the BLASTER
*               environment string) for audio transfer.  It also programs
*               the size of the DMA transfer and the DMA buffer address
*               used for the audio transfer.
*
*************************************************************************/
void ProgramDMA(unsigned long int DMABufPhysAddr, FILEINFO *FileHdrInfo,
		unsigned short int Count)
{
  short int Command,
	    DMAAddr,
	    DMACount,
	    DMAPage,
	    Offset,
	    Page,
	    Temp;

  Page   = (short int) (DMABufPhysAddr >> 16);
  Offset = (short int) (DMABufPhysAddr & 0xFFFF);

  if (FileHdrInfo->FileFormat < 4)  // 8-BIT FILE
  {
    switch(gBlastSet.DMAChan8Bit)
    {
      case 0:
	DMAAddr  = 0x0000;
	DMACount = 0x0001;
	DMAPage  = 0x0087;
      break;

      case 1:
	DMAAddr  = 0x0002;
	DMACount = 0x0003;
	DMAPage  = 0x0083;
      break;

      case 3:
	DMAAddr  = 0x0006;
	DMACount = 0x0007;
	DMAPage  = 0x0082;
      break;
    }

    outp(DMA8_MASK_REG, gBlastSet.DMAChan8Bit | 4);    // Disable DMA
    outp(DMA8_FF_REG, 0x0000);                         // Clear F-F
    outp(DMA8_MODE_REG, gBlastSet.DMAChan8Bit | 0x58); // 8-bit AI
    outp(DMACount, ((Count - 1) & 0xFF));              // LO byte
    outp(DMACount, ((Count - 1) >> 8));                // HI byte
  }
  else  // 16-BIT FILE
  {
    switch(gBlastSet.DMAChan16Bit)
    {
      case 5:
	DMAAddr  = 0x00C4;
	DMACount = 0x00C6;
	DMAPage  = 0x008B;
      break;

      case 6:
	DMAAddr  = 0x00C8;
	DMACount = 0x00CA;
	DMAPage  = 0x0089;
      break;

      case 7:
	DMAAddr  = 0x00CC;
	DMACount = 0x00CE;
	DMAPage  = 0x008A;
      break;
    }

    // Offset for 16-bit DMA must be calculated different than 8-bit.
    // Shift Offset 1 bit right.  Then copy LSBit of Page to MSBit of Offset.
    Temp = Page & 0x0001;  // Get LSBit of Page and...
    Temp <<= 15;           // move it to MSBit of Temp.
    Offset >>= 1;          // Divide Offset by 2.
    Offset &= 0x7FFF;      // Clear MSBit of Offset.
    Offset |= Temp;        // Put LSBit of Page into MSBit of Offset.

    outp(DMA16_MASK_REG, (gBlastSet.DMAChan16Bit - 4) | 4);    // Disable DMA
    outp(DMA16_FF_REG, 0x0000) ;                               // Clear F-F
    outp(DMA16_MODE_REG, (gBlastSet.DMAChan16Bit - 4) | 0x58); // 16-bit AI
    outp(DMACount, ((Count/2 - 1) & 0xFF));                    // LO byte
    outp(DMACount, ((Count/2 - 1) >> 8));                      // HI byte
  }

  // Program the starting address of the DMA buffer.
  outp(DMAPage, Page);             // Page number of DMA buffer.
  outp(DMAAddr, Offset & 0x00FF);  // LO byte offset address of DMA buffer.
  outp(DMAAddr, (Offset >> 8));    // HI byte offset address of DMA buffer.

  // Reenable 8-bit or 16-bit DMA.
  if (FileHdrInfo->FileFormat < 4)
    outp(DMA8_MASK_REG,  gBlastSet.DMAChan8Bit);
  else
    outp(DMA16_MASK_REG, gBlastSet.DMAChan16Bit - 4);

  return;
}


/*************************************************************************
*
* FUNCTION: ProgramDSP()
*
* DESCRIPTION: This function programs the DSP chip on the Sound Blaster
*              card.  The card type is identified by the DSP version
*              number.  Each type of Sound Blaster card is programmed
*              differently unless an 8-bit ADPCM file is played.  In that
*              case, all SB cards are programmed identically.
*
*************************************************************************/
void ProgramDSP(unsigned short int Count, FILEINFO *FileHdrInfo,
		unsigned char DMAMode)
{
  unsigned char Card;
  short int     Command,
		Mode;

  if (gHighSpeed == TRUE)  // Once in high-speed mode, DSP can only be reset!
    return;

  // Make sure Count is >= 2, so when the DSP is programmed for a block
  // tranfer, Count doesn't wrap around to a large number when 1 is
  // subtracted from it.
  if (Count <= 1)
    Count = 2;


  /*--- DETERMINE SOUND BLASTER CARD TYPE ----------------------------*/
  /*------------------------------------------------------------------*/
  if (gBlastSet.DSPVersion >= 0x0400)       // DSP version >= 4.00
    Card = SB16;
  else if (gBlastSet.DSPVersion >= 0x0300)  // DSP version = 3.xx
  {
    // Set SBPRO mixer register default to MONO, Output filter to OFF.
    outp(gBlastSet.BaseIOPort + 4, 0x000E);  // Select mixer reg. 0x0E.
    outp(gBlastSet.BaseIOPort + 5, 0x0000);  // MONO, Output filter off.
    Card = SBPRO;
  }
  else if (gBlastSet.DSPVersion >= 0x0201)  // 2.01 <= DSP version < 3.00
    Card = SB2_HI;
  else if (gBlastSet.DSPVersion == 0x0200)  // DSP version = 2.00
    Card = SB2_LO;


  /*--- FILE IS 8-BIT ADPCM PLAYBACK. IN THIS CASE, ALL SB CARDS  ------*/
  /*--- ARE PROGRAMMED IDENTICALLY.  DETERMINE THE COMMAND.       ------*/
  /*--------------------------------------------------------------------*/
  if (FileHdrInfo->FileFormat > 0 && FileHdrInfo->FileFormat < 4)
  {
    switch(FileHdrInfo->FileFormat)
    {
      case 1:  // 8-bit 2:1 compression (4-bit ADPCM)
	Command = 0x0074;  // default to single-cycle
      break;

      case 2:  // 8-bit 8:3 compression (2.6-bit ADPCM)
	Command = 0x0076;  // default to single-cycle
      break;

      case 3:  // 8-bit 4:1 compression (2-bit ADPCM)
	Command = 0x0016;  // default to single-cycle
      break;
    }

    if (DMAMode == AUTO_INIT)
      Command |= 0x0009;   // Set to auto-init mode.
  }
  else
  {
    /*--- FILE IS 8-BIT OR 16-BIT UNCOMPRESSED AUDIO.  PROGRAM EACH ----*/
    /*--- SOUND BLASTER CARD DIFFERENTLY.                           ----*/
    /*------------------------------------------------------------------*/
    switch(Card)
    {
      case SB16:
	// Program sample rate HI and LO byte.
	DSPWrite(gBlastSet.BaseIOPort, 0x0041);
	DSPWrite(gBlastSet.BaseIOPort, (FileHdrInfo->SampPerSec & 0xFF00) >> 8);
	DSPWrite(gBlastSet.BaseIOPort, (FileHdrInfo->SampPerSec & 0xFF));

	/*--- DETERMINE 8-bit OR 16-bit, MONO OR STEREO ----------------*/
	/*--------------------------------------------------------------*/
	if (FileHdrInfo->BitsPerSample == 8)
	{
	  Command = 0x00C0;  // 8-bit transfer (default: single-cycle, D/A)

	  if (FileHdrInfo->Channels == 1)
	    Mode = 0x0000;   // MONO, unsigned PCM data
	  else
	    Mode = 0x0020;   // STEREO, unsigned PCM data
	}
	else  // 16-BIT AUDIO
	{
	  Command = 0x00B0;  // 16-bit transfer (default: single-cycle, D/A)
	  Count  /= 2;       // Set Count to transfer 16-bit words.

	  if (FileHdrInfo->Channels == 1)
	    Mode = 0x0010;   // MONO, signed PCM data
	  else
	    Mode = 0x0030;   // STEREO, signed PCM data
	}

	/*--- CHANGE COMMAND TO AUTO-INIT, IF NEEDED. ------------------*/
	/*--------------------------------------------------------------*/
	if (DMAMode == AUTO_INIT)
	  Command |= 0x0004;     // Auto-init


	/*--- PROGRAM THE DSP CHIP (BEGIN DMA TRANSFER) AND RETURN! ----*/
	/*--------------------------------------------------------------*/
	DSPWrite(gBlastSet.BaseIOPort, Command);
	DSPWrite(gBlastSet.BaseIOPort, Mode);
	DSPWrite(gBlastSet.BaseIOPort, (Count - 1) & 0xFF);  // LO byte
	DSPWrite(gBlastSet.BaseIOPort, (Count - 1) >> 8);    // HI byte
      return;    // RETURN!


      case SBPRO:
	DSPWrite(gBlastSet.BaseIOPort, 0x00A0);  // Default to MONO.

	if (FileHdrInfo->Channels == 2)
	{
	  // HI-SPEED, STEREO
	  gHighSpeed = TRUE;

	  DSPWrite(gBlastSet.BaseIOPort, 0x00A8);  // STEREO MODE
	  outp(gBlastSet.BaseIOPort + 4, 0x000E);  // Select mixer reg. 0x0E.
	  outp(gBlastSet.BaseIOPort + 5, 0x0002);  // STEREO, output filter off.

	  if (DMAMode == AUTO_INIT)
	    Command = 0x0090;  // HIGH-SPEED, AUTO-INIT MODE
	  else
	    Command = 0x0091;  // HIGH-SPEED, SINGLE-CYCLE MODE
	}
	else if (FileHdrInfo->SampPerSec >= 23000)
	{
	  // HI-SPEED, MONO
	  gHighSpeed = TRUE;

	  if (DMAMode == AUTO_INIT)
	    Command = 0x0090;  // HIGH-SPEED, AUTO-INIT MODE
	  else
	    Command = 0x0091;  // HIGH-SPEED, SINGLE-CYCLE MODE
	}
	else if (DMAMode == AUTO_INIT)
	  Command = 0x001C;   // NORMAL, AUTO-INIT
	else
	  Command = 0x0014;   // NORMAL, SINGLE-CYCLE
      break;


      case SB2_HI:
	if (FileHdrInfo->SampPerSec > 13000 || FileHdrInfo->Channels == 2)
	{
	  // HI-SPEED
	  gHighSpeed = TRUE;

	  if (DMAMode == AUTO_INIT)
	    Command = 0x0090;  // HIGH-SPEED, AUTO-INIT MODE
	  else
	    Command = 0x0091;  // HIGH-SPEED, SINGLE-CYCLE MODE
	}
	else if (DMAMode == AUTO_INIT)
	  Command = 0x001C;  // NORMAL, MONO, AUTO-INIT
	else
	  Command = 0x0014;  // NORMAL, MONO, SINGLE-CYCLE
      break;


      case SB2_LO:  // DSP VERSION == 2.00.  HIGH-SPEED MODE NOT AVAILABLE.
	if (DMAMode == AUTO_INIT)
	  Command = 0x001C;  // NORMAL, MONO, AUTO-INIT
	else
	  Command = 0x0014;  // NORMAL, MONO, SINGLE-CYCLE
      break;
    }
  }


  /*--- IF FILE IS 8-BIT ADPCM (REGARDLESS OF CARD TYPE), OR CARD IS ---*/
  /*--- AN 8-BIT AUDIO CARD (DSP VERSION < 4.xx), BEGIN DMA TRANFER. ---*/
  /*--------------------------------------------------------------------*/
  DSPWrite(gBlastSet.BaseIOPort, 0x00D1);  // Turn speaker on.
  DSPWrite(gBlastSet.BaseIOPort, 0x0040);  // Program Time Constant
  DSPWrite(gBlastSet.BaseIOPort, FileHdrInfo->TimeConstant);

  /*--- NOTE: If in high-speed mode, single-cycle DMA is programmed ----*/
  /*--- using the same initial DSP command as auto-init (0x0048).   ----*/
  /*--------------------------------------------------------------------*/
  if (DMAMode == AUTO_INIT || gHighSpeed == TRUE)
  {
    // Program block tranfer size LO and HI byte and begin tranfer.
    DSPWrite(gBlastSet.BaseIOPort, 0x0048);
    DSPWrite(gBlastSet.BaseIOPort, (Count - 1) & 0x00FF);  // LO byte
    DSPWrite(gBlastSet.BaseIOPort, (Count - 1) >> 8);      // HI byte
    DSPWrite(gBlastSet.BaseIOPort, Command);               // Begin Xfer
  }
  else  // DMAMode == SINGLE_CYCLE  If mode is high-speed, execute above code.
  {
    // Program size of last block and begin transfer.
    DSPWrite(gBlastSet.BaseIOPort, Command);
    DSPWrite(gBlastSet.BaseIOPort, (Count - 1) & 0x00FF);  // LO byte
    DSPWrite(gBlastSet.BaseIOPort, (Count - 1) >> 8);      // HI byte
  }

  return;
}


/*************************************************************************
*
* FUNCTION: VerifyFileType()
*
* DESCRIPTION:  1) Verifies that the file represented by the file pointer
*                  passed to this function is either WAVE, Creative VOC,
*                  or invalid.  FileType is returned.
*
*************************************************************************/
char VerifyFileType(FILE *File)
{
  char               Buffer[20],
		     FileType;
  unsigned short int VocIDCode,
		     VocOffset,
		     VocVersion;

  //--- DETERMINE IF FILE IS VOC, WAVE, OR INVALID ---------------------
  fread(Buffer, 20, 1, File);

  if (!memcmp(Buffer, "Creative Voice File", 19) && Buffer[19] == 0x1A)
  {
    fread(&VocOffset,  2, 1, File);
    fread(&VocVersion, 2, 1, File);
    fread(&VocIDCode,  2, 1, File);

    if ((~VocVersion + 0x1234) == VocIDCode)  // Verify that file is VOC.
    {
      FileType = VOC_FILE;
      fseek(File, (long) VocOffset, SEEK_SET);  // Go to 1st Data Block
    }
  }
  else if (!memcmp(Buffer, "RIFF", 4) && !memcmp(&Buffer[8], "WAVEfmt ", 8))
    FileType = WAVE_FILE;
  else
    FileType = INVALID_FILE_FORMAT;

  return(FileType);
}


/*************************************************************************
*
* FUNCTION:  DmaISR()
*
* DESCRIPTION: If the interrupt was a DMA generated by the Sound Blaster,
*              acknowledge the interrupt, update the global variables,
*              and send the end-of-interrupt command(s).
*
*              If the interrupt was NOT generated by the Sound Blaster,
*              call the ISR (gOldISRFuncPtr) saved by SetDMAISR() or
*              return.
*
*************************************************************************/
void interrupt DmaISR(void)
{
  unsigned char InterruptStatus;

  // Read Sound Blaster mixer interrupt register to determine interrupt type.
  outp(gBlastSet.BaseIOPort + 4, 0x0082);           // Select interrupt reg.
  InterruptStatus = inp(gBlastSet.BaseIOPort + 5);  // Read interrupt reg.

  if (InterruptStatus & 0x01)            // Interrupt is from 8-bit DMA.
    inp(gBlastSet.BaseIOPort + 0x000E);  // Acknowledge the interrupt.
  else if (InterruptStatus & 0x02)       // Interrupt is from 16-bit DMA.
    inp(gBlastSet.BaseIOPort + 0x000F);  // Acknowledge the interrupt.
  else
  {
    // Interrupt is NOT SB DMA.  Call ISR saved by SetDmaISR() or return.
    if (gOldISRFuncPtr)
      (*gOldISRFuncPtr)();
    return;
  }

  gBytesLeftToPlay  -= (long) (gDMABufSize / 2);
  gDMABufNowPlaying ^= 1;  // Keep track of which 1/2 DMA buffer is playing.

  // Send end-of-interrupt command(s).
  if (gBlastSet.IRQNumber > 7)
    outp(PIC1_COMMAND_REG, END_OF_INTERRUPT);

  outp(PIC0_COMMAND_REG, END_OF_INTERRUPT);

  return;
}


/*************************************************************************
*
* FUNCTION: RestoreOldISR()
*
* DESCRIPTION:  1) Disable all interrupts.
*               2) Restore IRQ mask for IRQs 0 to 7.
*               3) If necessary, restore IRQ mask for IRQs 8 to 15.
*               4) Restore the original ISR (which was saved to a global
*                  varaiable function pointer in SetDmaISR()) for the
*                  interrupt vector number associated with
*                  BlastSet->IRQNumber.
*               5) Enable all interrupts.
*
*************************************************************************/
void RestoreOldISR(BLASTER *BlastSet)
{
  short int IntVectorNumber;

  disable();  // Temporarily disable interrupts

  outp(PIC0_MASK_REG, gIRQMaskSave0);    // Restore IRQ mask for IRQs 0 to 7.

  if (BlastSet->IRQNumber > 7)
  {
    outp(PIC1_MASK_REG, gIRQMaskSave1);  // Restore IRQ mask for IRQs 8 to 15.
    IntVectorNumber = BlastSet->IRQNumber - 8 + 0x70;
  }
  else  // BlastSet->IRQNumber is 0 to 7.
    IntVectorNumber = BlastSet->IRQNumber + 8;

  // Restore the old ISR to the interrupt vector number.
  setvect(IntVectorNumber, gOldISRFuncPtr);

  enable();   // Enable interrupts
  return;
}


/*************************************************************************
*
* FUNCTION: SetDmaISR()
*
* DESCRIPTION:  1) Disable all interrupts.
*               2) Save current interrupt mask(s) to global variable(s).
*               3) Set new interrupt mask(s).
*               4) Save ISR associated with BlastSet->IRQNumber to
*                  a global variable function pointer.
*               5) Set the new ISR associated with BlastSet->IRQNumber.
*               6) Enable all interrupts.
*
*************************************************************************/
void SetDmaISR(BLASTER *BlastSet)
{
  short int IntVectorNum,
	    IRQMaskNew;

  disable();  // Temporarily disable interrupts.

  /*--- Save current interrupt masks and set the new ones. -------------*/
  gIRQMaskSave0 = inp(PIC0_MASK_REG);    // Save IRQ 0 to 7 mask.
  if (BlastSet->IRQNumber > 7)
  {
    IntVectorNum  = BlastSet->IRQNumber - 8 + 0x70;
    gIRQMaskSave1 = inp(PIC1_MASK_REG);  // Save IRQ 8 to 15 mask.

    // Set new IRQ mask for IRQs 8 to 15.
    IRQMaskNew = ~(((short int) 0x0001) << (BlastSet->IRQNumber - 8));
    outp(PIC1_MASK_REG, gIRQMaskSave1 & IRQMaskNew);

    // Setting IRQ mask 2 on PIC 0 enables IRQs 8 to 15 on PIC 1.
    outp(PIC0_MASK_REG, gIRQMaskSave0 & ~0x0004);
  }
  else  // BlastSet->IRQNumber is 0 to 7.
  {
    IntVectorNum = BlastSet->IRQNumber + 8;

    // Set new IRQ mask for IRQs 0 to 7.
    IRQMaskNew = ~(((short int) 0x0001) << BlastSet->IRQNumber);
    outp(PIC0_MASK_REG, gIRQMaskSave0 & IRQMaskNew);
  }

  /*--- Save current ISR and set the new one. --------------------------*/
  gOldISRFuncPtr = getvect(IntVectorNum);
  setvect(IntVectorNum, DmaISR);

  enable();  // Enable interrupts.

  return;
}


/************************************************************************
*
* FUNCTION: GetBlastEnv()
*
* DESCRIPTION: Search the BLASTER environment string for:
*
*              A) Base IO Port Address
*              B) LO (8 bit)  DMA Channel
*              C) HI (16 bit) DMA Channel
*              D) IRQ Number
*              E) MIDI Port address
*
*              The Base I/O port address and the MIDI address are stored
*              in the environment string in hex--convert them to integer.
*              These numbers from the environment string are then placed
*              in the BLASTER struct passed to this function as a pointer.
*              The BLASTER struct is defined as:
*
*              typedef struct _BLASTER
*              {
*                short int BaseIOPort,
*                          DMAChan8Bit,
*                          DMAChan16Bit,
*                          DSPVersion,
*                          IRQNumber,
*                          MIDIPort;
*              } BLASTER;
*
*              Then, get the DSP version of the Sound Blaster DSP chip.
*              This is used to determine the Sound Blaster's capabilities.
*
* RETURN: FAIL - BLASTER environment string is not found or any of
*                the BLASTER structure members aren't found.
*
*         SUCCESS - All 5 members of BLASTER struct are found in the
*                   BLASTER environment string.
*
************************************************************************/
char GetBlastEnv(BLASTER *Blast)
{
  char  Buffer[5],
	DMAChannelNotFound = TRUE,
       *EnvString,
	IOPortNotFound     = TRUE,
	IRQNotFound        = TRUE,
	SaveChar;

  short int digit,
	    i,
	    Major,
	    Minor,
	    multiplier;


  EnvString = getenv("BLASTER");

  if (EnvString == NULL)
    return(FAIL);  // BLASTER environment variable not found.

  do
  {
    switch(*EnvString)
    {
      case 'A':  // I/O base port address found
      case 'a':
	EnvString++;
	for (i = 0; i < 3; i++)  // Grab the digits
	{
	  Buffer[i] = *EnvString;
	  EnvString++;
	}

	// The string is in ASCII HEX, convert it to decimal
	multiplier = 1;
	Blast->BaseIOPort = 0;
	for (i -= 1; i >= 0; i--)
	{
	  // Convert to HEX
	  if (Buffer[i] >= '0' && Buffer[i] <= '9')
	    digit = Buffer[i] - '0';
	  else if (Buffer[i] >= 'A' && Buffer[i] <= 'F')
	    digit = Buffer[i] - 'A' + 10;
	  else if (Buffer[i] >= 'a' && Buffer[i] <= 'f')
	    digit = Buffer[i] - 'a' + 10;

	  Blast->BaseIOPort += digit * multiplier;
	  multiplier *= 16;
	}

	IOPortNotFound = FALSE;
      break;


      case 'D': // 8-bit DMA channel
      case 'd':
      case 'H': // 16-bit DMA channel
      case 'h':
	SaveChar = *EnvString;
	EnvString++;
	Buffer[0] = *EnvString;
	EnvString++;

	if (*EnvString >= '0' && *EnvString <= '9')
	{
	  Buffer[1] = *EnvString; // DMA Channel No. is 2 digits
	  Buffer[2] = NULL;
	  EnvString++;
	}
	else
	  Buffer[1] = NULL;       // DMA Channel No. is 1 digit

	if (SaveChar == 'D' || SaveChar == 'd')
	  Blast->DMAChan8Bit  = atoi(Buffer);  // 8-Bit DMA channel
	else
	  Blast->DMAChan16Bit = atoi(Buffer);  // 16-bit DMA channel

	DMAChannelNotFound = FALSE;
      break;


      case 'I':  // IRQ number
      case 'i':
	EnvString++;
	Buffer[0] = *EnvString;
	EnvString++;

	if (*EnvString >= '0' && *EnvString <= '9')
	{
	  Buffer[1] = *EnvString; // IRQ No. is 2 digits
	  Buffer[2] = NULL;
	  EnvString++;
	}
	else
	  Buffer[1] = NULL;       // IRQ No. is 1 digit

	Blast->IRQNumber  = atoi(Buffer);
	IRQNotFound = FALSE;
      break;


      default:
	EnvString++;
      break;
    }

  } while (*EnvString != NULL);

  if (DMAChannelNotFound || IOPortNotFound || IRQNotFound)
    return(FAIL);

  /*--- Get the DSP version number.  The next read from the DSP will ---*/
  /*--- return the major version number.  The following read will    ---*/
  /*--- return the minor version number.                             ---*/
  ResetDSP(gBlastSet.BaseIOPort);
  DSPWrite(Blast->BaseIOPort, DSP_GET_VERSION);
  Major = DSPRead(Blast->BaseIOPort);  /* Read Major DSP version no. */
  Minor = DSPRead(Blast->BaseIOPort);  /* Read Minor DSP version no. */
  Blast->DSPVersion = (Major << 8) | Minor;

  return(SUCCESS);
}


/*************************************************************************
*
* FUNCTION: DSPRead()
*
* DESCRIPTION: Reads a value from the DSP Read Port.
*
* Entry: BaseIOPort - The Sound Blaster's base I/O address.
*
*************************************************************************/
short int DSPRead(short int BaseIOPort)
{
  /* Wait until DSP is ready before reading from the DSP. */
  while ((inp(BaseIOPort + DSP_DATA_AVAIL) & 0x80) == 0);

  /* Return value read from the Read Port. */
  return(inp(BaseIOPort + DSP_READ_PORT));
}


/*************************************************************************
*
* FUNCTION: DSPWrite()
*
* DESCRIPTION: Writes the value passed to this function to the DSP Write
*              Port.
*
* Entry: BaseIOAddr - The Sound Blaster's base I/O address.
*
*************************************************************************/
void DSPWrite(short int BaseIOAddr, short int WriteValue)
{
  /* Wait until DSP is ready before writing to the DSP. */
  while ((inp(BaseIOAddr + DSP_WRITE_PORT) & 0x80) != 0);

  outp(BaseIOAddr + DSP_WRITE_PORT, WriteValue);
  return;
}


/*************************************************************************
*
* FUNCTION: ResetDSP()
*
* DESCRIPTION: Self explanatory
*
* Entry: BaseIOAddr - The Sound Blaster's base I/O address.
*
*************************************************************************/
char ResetDSP(short int IOBasePort)
{
  outp(IOBasePort + DSP_RESET_PORT, 0x0001);  /* Write "1" to Reset Port. */
  delay(10);                                  /* Wait 10 mS.              */
  outp(IOBasePort + DSP_RESET_PORT, 0x0000);  /* Write "0" to Reset port. */

  /* Wait until data is available. (Wait while BIT-7 == 0.) */
  while ((inp(IOBasePort + DSP_DATA_AVAIL) & 0x80) == 0);

  if (inp(IOBasePort + DSP_READ_PORT) == DSP_READY)
    return(SUCCESS);
  return(FAIL);
}

/*************************************************************************
*
* FUNCTION: AllocateDMABuffer()
*
* DESCRIPTION : Allocate memory for the DMA buffer.  After memory is
*               allocated for the buffer, call OnSamePage() to verify
*               that the entire buffer is located on the same page.
*               If the buffer crosses a page boundary, allocate another
*               buffer. Continue this process until the DMA buffer resides
*               entirely within the same page.
*
*               For every malloc() called, save a pointer that points to
*               the block of memory allocated.  Deallocate ALL memory blocks
*               allocated that cross a page boundary.  Once a memory block
*               is allocated that does NOT cross a page boudary, this block
*               will be used for the DMA buffer--any previously allocated
*               memory blocks will be deallocated.
*
* ENTRY: **DMABuffer is the address of the pointer that will point to
*        the memory allocated.
*
* EXIT: If a buffer is succesfully allocated, *DMABuffer will point to
*       the buffer and the physical address of the buffer pointer will
*       be returned.
*
*       If a buffer is NOT successfully allocated, return FAIL.
*
*************************************************************************/
unsigned long int AllocateDMABuffer(unsigned char **DMABuffer,
				    unsigned short int *DMABufSize)
{
  unsigned char     BufferNotAllocated = TRUE,
		    Done = FALSE,
		   *PtrAllocated[50];
  short int         i,
		    Index = 0;
  unsigned long int PhysAddress;


  /*--- BUMP *DMABufSize UP TO NEXT 8-BYTE BOUNDARY. -------------------*/
  /*--------------------------------------------------------------------*/
  if (*DMABufSize == 0 || *DMABufSize % 8)
    *DMABufSize = *DMABufSize + (8 - (*DMABufSize % 8));

  do
  {
    *DMABuffer = (unsigned char *) malloc(*DMABufSize);

    if (*DMABuffer != NULL)
    {
      /*--- Save the ptr for every malloc() performed ---*/
      PtrAllocated[Index] = *DMABuffer;
      Index++;

      /*--- If entire buffer is within one page, we're out of here! ---*/
      PhysAddress = OnSamePage(*DMABuffer, *DMABufSize);
      if (PhysAddress != FAIL)
      {
	BufferNotAllocated = FALSE;
	Done = TRUE;
      }
    }
    else
      Done = TRUE;  // malloc() couldn't supply requested memory

  } while (!Done);


  if (BufferNotAllocated)
  {
    Index++;             // Incr. Index so most recent malloc() gets free()d
    PhysAddress = FAIL;  // return FAIL
  }

  /*--- Deallocate all memory blocks crossing a page boundary ---*/
  for (i= 0; i < Index - 1; i++)
    free(PtrAllocated[i]);

  return(PhysAddress);
}


/**************************************************************************
*
* FUNCTION: OnSamePage()
*
* DESCRIPTION: Check the memory block pointed to by the parameter
*              passed to make sure the entire block of memory is on the
*              same page.  If a buffer DOES cross a page boundary,
*              return FAIL. Otherwise, return the physical address
*              of the beginning of the DMA buffer.
*
*              A page corresponds to the following addresses:
*
*              PAGE NO.   SEG:OFF ADDRESS          PHYSICAL ADDRESS
*              --------   ----------------------   ----------------
*                 0       0000:0000 to 0000:FFFF   00000 to 0FFFF
*                 1       1000:0000 to 1000:FFFF   10000 to 1FFFF
*                 .                 .                    .
*                 .                 .                    .
*                 E       E000:0000 to E000:FFFF   E0000 to EFFFF
*                 F       F000:0000 to F000:FFFF   F0000 to FFFFF
*
*              NOTE: The upper nibble of the physical address is the
*                    same as the page number!
*
* ENTRY: *DMABuffer - Points to beginning of DMA buffer.
*
* EXIT: If the buffer is located entirely within one page, return the
*       physical address of the buffer pointer.  Otherwise return FAIL.
*
**************************************************************************/
unsigned long int OnSamePage(unsigned char *DMABuffer,
			     unsigned short int DMABufSize)
{
  unsigned long int BegBuffer,
		    EndBuffer,
		    PhysAddress;

  /*----- Obtain the physical address of DMABuffer -----*/
  BegBuffer = ((unsigned long) (FP_SEG(DMABuffer)) << 4) +
	       (unsigned long) FP_OFF(DMABuffer);
  EndBuffer   = BegBuffer + DMABufSize - 1;
  PhysAddress = BegBuffer;

  /*-- Get page numbers for start and end of DMA buffer. --*/
  BegBuffer >>= 16;
  EndBuffer >>= 16;

  if (BegBuffer == EndBuffer)
    return(PhysAddress);  // Entire buffer IS on same page!
  return(FAIL); // Entire buffer NOT on same page.  Thanks Intel!
}


/*************************************************************************
*
* FUNCTION: KillVolume()
*
*************************************************************************/
void KillVolume(void)
{
  // Only SB 2 with CD interface has a mixer chip.
  if (gBlastSet.DSPVersion < 0x0300)         // Select master volume reg.
    outp(gBlastSet.BaseIOPort + 4, 0x0002);
  else  // SB Pro or SB16/AWE32 mixer.
    outp(gBlastSet.BaseIOPort + 4, 0x0022);  // Select master volume reg.

  outp(gBlastSet.BaseIOPort + 5, 0x0000);    // KILL the volume.

  return;
}


/*************************************************************************
*
* FUNCTION: RestoreOrRememberVolume()
*
*************************************************************************/
void RestoreOrRememberVolume(char Command)
{
  static short int MasterVolume = 0;


  if (gBlastSet.DSPVersion < 0x0300)         // Only SB 2 with CD has a mixer.
    outp(gBlastSet.BaseIOPort + 4, 0x0002);  // Select master volume reg.
  else // SB Pro or SB16/AWE32 mixer.
    outp(gBlastSet.BaseIOPort + 4, 0x0022);  // Select master volume reg.


  if (Command == REMEMBER_VOLUME)
    MasterVolume = inp(gBlastSet.BaseIOPort + 5);  // Save master volume.
  else if (Command == RESTORE_VOLUME)
    outp(gBlastSet.BaseIOPort + 5, MasterVolume);  // Restore master volume.

  return;
}


/*************************************************************************
*
* FUNCTION: ContinueDMA()
*
* DESCRIPTION: Continues the DMA transfer that was halted.  The DMA
*              tranfer can be halted by calling PauseDMA().
*
* SEE ALSO: PauseDMA()
*
*************************************************************************/
void ContinueDMA(unsigned char BitsPerSample)
{

  if (gBlastSet.DSPVersion >= 0x0200 && gBlastSet.DSPVersion < 0x0400)
    RestoreOrRememberVolume(RESTORE_VOLUME);

  /*--- IF IN HIGH-SPEED MODE, CAN'T REPROGRAM DSP CHIP--RETURN! -------*/
  /*--- OTHERWISE, RESUME THE DMA.                               -------*/
  /*--------------------------------------------------------------------*/
  if (gHighSpeed == TRUE)
    return;
  else if (BitsPerSample == 8)
    DSPWrite(gBlastSet.BaseIOPort, 0x00D4);  // Continue SB 8-bit DMA xfer.
  else  // BitsPerSample == 16
    DSPWrite(gBlastSet.BaseIOPort, 0x00D6);  // Continue SB 16-bit DMA xfer.

  return;
}


/*************************************************************************
*
* FUNCTION: PauseDMA()
*
* DESCRIPTION: Halts the DMA tranfer.  The DMA tranfer can be resumed by
*              calling ContinueDMA().
*
* SEE ALSO: ContinueDMA()
*
*************************************************************************/
void PauseDMA(unsigned char BitsPerSample)
{

  if (gBlastSet.DSPVersion >= 0x0200 && gBlastSet.DSPVersion < 0x0400)
  {
    RestoreOrRememberVolume(REMEMBER_VOLUME);
    KillVolume();
  }

  /*--- IF IN HIGH-SPEED MODE, CAN'T REPROGRAM DSP CHIP--RETURN. -------*/
  /*--- OTHERWISE, HALT THE DMA.                                 -------*/
  /*--------------------------------------------------------------------*/
  if (gHighSpeed == TRUE)
    return;
  else if (BitsPerSample == 8)
    DSPWrite(gBlastSet.BaseIOPort, 0x00D0);  // Pause SB 8-bit DMA xfer.
  else  // BitsPerSample == 16
    DSPWrite(gBlastSet.BaseIOPort, 0x00D5);  // Pause SB 16-bit DMA xfer.

  return;
}


/*************************************************************************
*
* FUNCTION: SetMixer()
*
* DESCRIPTION: Sets mixer to maximum volume.
*
*************************************************************************/
void SetMixer(void)
{
  outp(gBlastSet.BaseIOPort + 4, (int) 0x0A);
  outp(gBlastSet.BaseIOPort + 5, (int) 0x00);
  outp(gBlastSet.BaseIOPort + 4, (int) 0x04);
  outp(gBlastSet.BaseIOPort + 5, (int) 0xFF);
  outp(gBlastSet.BaseIOPort + 4, (int) 0x22);
  outp(gBlastSet.BaseIOPort + 5, (int) 0xFF);

  return;
}


/*************************************************************************
*
* FUNCTION: AcquireCSP()
*
* DESCRIPTION: Get the CSP and download code from file into it.
*
**************************************************************************/
#ifdef USE_CSP
char AcquireCSP(CSP_FUNC_PTR CSPFunc, unsigned short int FileFormat)
{
  char          FileName[80],
	       *SoundDir;
  int           FileLength,
		Handle;
  DWORD         RetValue;
  unsigned int  NoOfBytesRead,
		Segment,
		Temp;
  void far     *Buffer;


  /*--- GET FILE-PATH NAME OF FILE CONTAINING CSP CODE -----------------*/
  /*--------------------------------------------------------------------*/
  SoundDir = getenv("SOUND");
  if (SoundDir == 0)
  {
    puts("SOUND environment variable missing!");
    return(FAIL);
  }
  strcpy(FileName, SoundDir);

  switch(FileFormat)
  {
    case 0x0006:
      strcat(FileName, "\\CSP\\WO0006.CSP");
    break;

    case 0x0007:
      strcat(FileName, "\\CSP\\WO0007.CSP");
    break;

    case 0x0200:
      strcat(FileName, "\\CSP\\WO0200.CSP");
    break;

    default:
      puts("Invalid FileFormat in AcquireCSP().");
    break;
  }

  /*--- OPEN FILE CONTAINING CSP CODE ----------------------------------*/
  /*--------------------------------------------------------------------*/
  Handle = open(FileName, O_RDONLY);
  if (Handle == -1)
  {

    // Check for file in current directory.
    switch(FileFormat)
    {
      case 0x0006:
	Handle = open("WO0006.CSP", O_RDONLY);
      break;

      case 0x0007:
	Handle = open("WO0007.CSP", O_RDONLY);
      break;

      case 0x0200:
	Handle = open("WO0200.CSP", O_RDONLY);
      break;

      default:
	puts("Invalid FileFormat in AcquireCSP().");
      break;
    }

    if (Handle == -1)
    {
      printf("Can't open file: %s\n", FileName);
      return(FAIL);
    }
  }

  /*--- ALLOCATE MEMORY TO LOAD CSP CODE. ------------------------------*/
  /*--------------------------------------------------------------------*/
  FileLength = filelength(Handle);
  if (allocmem(FileLength / 16 + 1, &Segment) != -1)
  {
    puts("allocmem() failed!");
    close(Handle);
    return(FAIL);
  }

  /*--- COPY CODE FROM FILE INTO ALLOCATED MEMORY. ---------------------*/
  /*--------------------------------------------------------------------*/
  FP_SEG(Buffer) = Segment;
  FP_OFF(Buffer) = 0;

  if (_dos_read(Handle, Buffer, FileLength, &NoOfBytesRead) != 0)
  {
    puts("read() FAILS!");
    freemem(Segment);
    close(Handle);
    return(FAIL);
  }

  close(Handle);

  /*--- ACQUIRE CSP AND GET THE USER NUMBER ----------------------------*/
  /*--------------------------------------------------------------------*/
  RetValue = CSPFunc(0, CSPSYS_ACQUIRE, UNUSED, (DWORD) &gUserNo, UNUSED);
  if (RetValue != CSPSYS_ERR_NOERROR)
  {
    freemem(Segment);
    puts("CSPFunc() CSPSYS_ACQUIRE fails!");
    return(FAIL);
  }

  /*--- OPEN TEMPORARY FILE TO SAVE USER NUMBER ------------------------*/
  /*--------------------------------------------------------------------*/
  Handle = open(gCSPUserFileName, O_CREAT | O_WRONLY | O_TRUNC,
		S_IWRITE | S_IWRITE);
  if (Handle == -1)
  {
    printf("Can't create file: %s\n", gCSPUserFileName);
    goto exit_fail;
  }

  /*--- SAVE USER NUMBER TO TEMPORARY FILE -----------------------------*/
  /*--------------------------------------------------------------------*/
  if (write(Handle, &gUserNo, sizeof(gUserNo)) == -1)
  {
    puts("write() FAILS!");
    close(Handle);
    goto exit_fail;
  }

  close(Handle);  // Close the temporary file.

  /*--- DOWNLOAD CSP CODE FROM BUFFER TO CSP CHIP ----------------------*/
  /*--------------------------------------------------------------------*/
  RetValue = CSPFunc(0, CSPSYS_DOWNLOAD, gUserNo, (DWORD) Buffer, FileLength);
  if (RetValue != CSPSYS_ERR_NOERROR)
  {
    puts("CSPFunc() CSPSYS_DOWNLOAD fails!");
    goto exit_fail;
  }

  /*--- SET CSP NUMBER OF CHANNELS -------------------------------------*/
  /*--------------------------------------------------------------------*/
  RetValue = CSPFunc(0, CSPSYS_SETPARAM, gUserNo, CSPSYS_PARAM_NCHANNELS,
		     gChannels);
  if (RetValue != CSPSYS_ERR_NOERROR)
  {
    puts("CSPFunc() CSPSYS_SETPARM NCHANNELS fails!");
    goto exit_fail;
  }

  /*--- SET CSP DMA WIDTH ----------------------------------------------*/
  /*--------------------------------------------------------------------*/
  if (FileFormat == 0x0200)
    Temp = 16;  // Force to 16 because CTADPCM files have BitsPerSample = 4.
  else
    Temp = gBitsPerSample;

  RetValue = CSPFunc(0, CSPSYS_SETPARAM, gUserNo, CSPSYS_PARAM_DMAWIDTH, Temp);
  if (RetValue != CSPSYS_ERR_NOERROR)
  {
    puts("CSPFunc() CSPSYS_SETPARM DMAWIDTH fails!");
    goto exit_fail;
  }

  /*--- START CSP RUNNING ----------------------------------------------*/
  /*--------------------------------------------------------------------*/
  RetValue = CSPFunc(0, CSPSYS_START, gUserNo, UNUSED, UNUSED);
  if (RetValue != CSPSYS_ERR_NOERROR)
  {
    puts("CSPFunc() CSPSYS_START fails!");
    goto exit_fail;
  }

  freemem(Segment);
  puts("CSP ACQUIRED");
  return(SUCCESS);


exit_fail:

  freemem(Segment);

  /*--- RELEASE THE CSP ------------------------------------------------*/
  /*--------------------------------------------------------------------*/
  RetValue = CSPFunc(0, CSPSYS_RELEASE, gUserNo, UNUSED, UNUSED);
  if (RetValue != CSPSYS_ERR_NOERROR)
    puts("AcquireCSP() call to CSPFunc() CSPSYS_RELEASE fails!");

  return(FAIL);
}
#endif


/**************************************************************************
*
* FUNCTION: ReleaseCSP()
*
* DESCRIPTION:
*
**************************************************************************/
#ifdef USE_CSP
char ReleaseCSP(CSP_FUNC_PTR CSPFunc)
{
  int   Handle;
  DWORD RetValue;

  /*--- OPEN TEMPORARY FILE CREATED IN AcquireCSP() ---------------------*/
  /*---------------------------------------------------------------------*/
  Handle = open(gCSPUserFileName, O_RDONLY);
  if (Handle == -1)
  {
    printf("ReleaseCSP() can't open file: %s\n", gCSPUserFileName);
    return(FAIL);
  }

  /*--- READ THE USER NUMBER FROM THE TEMPORARY FILE. ------------------*/
  /*--------------------------------------------------------------------*/
  if (read(Handle, &gUserNo, sizeof(gUserNo)) == -1)
  {
    printf("ReleaseCSP() can't read: %s\n", gCSPUserFileName);
    return(FAIL);
  }

  /*--- CLOSE AND DELETE THE TEMPORARY FILE. ---------------------------*/
  /*--------------------------------------------------------------------*/
  close(Handle);
  unlink(gCSPUserFileName);


  /*--- STOP THE CSP ---------------------------------------------------*/
  /*--------------------------------------------------------------------*/
  RetValue = CSPFunc(0, CSPSYS_STOP, gUserNo, UNUSED, UNUSED);
  if (RetValue != CSPSYS_ERR_NOERROR)
  {
    puts("ReleaseCSP() call to CSPFunc() CSPSYS_STOP fails!");
    return(FAIL);
  }


  /*--- RELEASE THE CSP ------------------------------------------------*/
  /*--------------------------------------------------------------------*/
  RetValue = CSPFunc(0, CSPSYS_RELEASE, gUserNo, UNUSED, UNUSED);
  if (RetValue != CSPSYS_ERR_NOERROR)
  {
    puts("ReleaseCSP() call to CSPFunc() CSPSYS_RELEASE fails!");
    return(FAIL);
  }

  puts("CSP RELEASED");
  return(SUCCESS);
}
#endif



/************************************************************************
*
* FUNCTION: GetCSPDriverAddr()
*
* DESCRIPTION: Points CSPFunc() function pointer to the driver's address
*              so the CSP.SYS driver can be called later.
*
*************************************************************************/
#ifdef USE_CSP
char GetCSPDriverAddr(CSP_FUNC_PTR *CSPFunc)
{
  int Handle;

  /*--- OPEN CSP.SYS DRIVER --------------------------------------------*/
  /*--------------------------------------------------------------------*/
  Handle = open("CSPXXXX0", O_RDONLY);
  if (Handle == -1)
  {
    puts("CSP.SYS driver did not open!");
    return (FAIL);
  }

  /*--- Point CSPFunc to the CSP driver's address. ---------------------*/
  /*--------------------------------------------------------------------*/
  if (ioctl(Handle, 2, CSPFunc, 4) == -1)
  {
    puts("ioctl() FAILS!");
    close(Handle);
    return(FAIL);
  }

  close(Handle);

  return (SUCCESS);
}
#endif
