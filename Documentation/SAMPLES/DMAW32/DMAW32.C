/*************************************************************************
*
*  FILE : DMAW32.C  ver 1.00 (15 Dec, 94)
*
*  Copyright (C) 1994-96 Creative Technology.
*
*  DMA DEMO PROGRAM FOR PLAYING WAVE FILES IN PROTECTED MODE USING
*  WATCOM C COMPILER AND DOS4GW DOS EXTENDER.
*
*  PURPOSE      : This protected mode program demonstrates how to play a
*                 .WAV file using DMA auto-init mode.
*
*  LIMITATIONS  : This program only supports DSP version 3.xx and above 
*                 (SBPro or later)
*   
*  DISCLAIMER   : Although this program has been tested with
*                 standard 8/16 bit PCM WAVE files, there could
*                 exist some unknown bugs.
*
*  COMPILE      :   wcl386 /l=dos4g dmaw32.c
*
*  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
*  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
*  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
*  PURPOSE.
*
*  You have a royalty-free right to use, modify, reproduce and
*  distribute the Sample Files (and/or any modified version) in
*  any way you find useful, provided that you agree that Creative
*  has no warranty obligations or liability for any Samples Files.
*
**************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <conio.h>
#include <dos.h>
#include <i86.h>
#include <mem.h>
#include <malloc.h>
#include <graph.h>

typedef unsigned char   BYTE;
typedef unsigned short  WORD;
typedef unsigned long   DWORD;
typedef unsigned char   UCHAR;
typedef unsigned int    UINT;
typedef unsigned long   ULONG;

#define BUFSIZE     (8*1024)    // two 4 KB DMA Buffers
#define MONO        1
#define STEREO      2
#define FALSE       0
#define TRUE        1
#define FAIL        0
#define SUCCESS     1

// ------  Programmable Interrupt Controller (PIC)  ------
#define PIC1MODE        0x20    // for irq 0 - 7
#define PIC1MASK        0x21
#define PIC2MODE        0xA0    // for irq 8 - 15
#define PIC2MASK        0xA1
#define PICEOI          0x20    // End Of Interrupt

// ------  DSP Commands  ------
#define dspTimeConstant         0x0040
#define dspOutputSampleRate     0x0041
#define dspBlockSize            0x0048
#define dspTurnSpeakerON        0x00D1
#define dspTurnSpeakerOFF       0x00D3
#define dspExitAutoInitDMA16    0x00D9
#define dspExitAutoInitDMA8     0x00DA
#define dspGetDSPVersion        0x00E1

// ------  DSP port address offsets  ------
#define dspMixerAddr    0x4     // Mixer register select addr
#define dspMixerData    0x5     // Mixer data port addr
#define dspReset        0x6     // DSP Reset addr
#define dspReadData     0xA     // DSP Read Data addr
#define dspWriteBuf     0xC     // DSP Write Buffer addr
#define dspDataAvail    0xE     // DSP Data Available addr
#define dspDMA8Ack      0xE     // 8-bit DMA intr. acknowledge
#define dspDMA16Ack     0xF     // 16-bit DMA intr. acknowledge
#define dspReady        0xAA

// ------ Mixer Register (common) ------
#define mixMSTRVOL      0x22
#define mixVOCVOL       0x04
#define mixMICVOL       0x0A
#define mixOUTFILTER    0x0E

// ------  DMA controller registers port addresses  ------
#define DMAWRITE        0x54
#define DMAREAD         0x58
#define AUTO_INIT       1
#define SINGLE_CYCLE    0

typedef struct { UCHAR addr, count, page, mask, mode, FF;
                 } dmaportstruct;

const dmaportstruct dmaport[8] = {
          {0x00, 0x01, 0x87, 0x0A, 0x0B, 0x0C},  // chan 0
          {0x02, 0x03, 0x83, 0x0A, 0x0B, 0x0C},  // chan 1
          {0x04, 0x05, 0x81, 0x0A, 0x0B, 0x0C},  // chan 2  DON'T USE
          {0x06, 0x07, 0x82, 0x0A, 0x0B, 0x0C},  // chan 3
          {0x00, 0x00, 0x00, 0xD4, 0xD6, 0xD8},  // chan 4  DON'T USE
          {0xC4, 0xC6, 0x8B, 0xD4, 0xD6, 0xD8},  // chan 5
          {0xC8, 0xCA, 0x89, 0xD4, 0xD6, 0xD8},  // chan 6
          {0xCC, 0xCE, 0x8A, 0xD4, 0xD6, 0xD8}   // chan 7
      };
                
// ------ Function Prototypes ------
DWORD AllocateDMABuffer(void);
void * DOSMemAlloc(DWORD);
void DOSMemFree(WORD);
void MemLock(void *, WORD);
void MemUnlock(void *, WORD);
char ResetDSP(void);
char InitDMADSP();
void SetSBProStereo(dmaportstruct *, int, int);
void SetMixer(void);
void DSPout(UINT);
char DSPin(UINT *);
void SetISR(void __interrupt __far *);
void RestoreISR(void);
void interrupt far DMAISR(void);
UINT FillBuffer(UCHAR **, ULONG *);
void FillPlayBuf(UCHAR *, ULONG);
void Play(UINT, char);
char GetBlasterEnv(void);
int  Chk_hdr(FILE *);

// ------ Global Variables ------
volatile char   DMA_buf_now_playing,
                Last_buf_played;
char    DMA_buf_to_fill,
        End_of_data,
        DMA_16bit,
        Mode, HS_mode = FALSE;
UINT    Intr_num, Intr_mask, Base, IRQ_num,
        DMA_chan8, DMA_chan16, DSP_ver, Output_filter;
UCHAR   *DMA_buffer;
WORD    DMA_buffer_selector;
void (__interrupt __far *IntrSave)(void);

struct WAVEHDR{ BYTE    format[4];
                DWORD   f_len;
                BYTE    wave_fmt[8];
                DWORD   fmt_len;
                WORD    fmt_tag;
                WORD    channel;
                DWORD   samples_per_sec;
                DWORD   avg_bytes_per_sec;
                WORD    blk_align;
                WORD    bits_per_sample;
                BYTE    data[4];
                DWORD   data_len;
                } wavehdr;

// ------ Program starts ------
void main(int argv, char *argc[])
{
    FILE    *fp;
    UCHAR   *data_buf;
    ULONG   bytes_left_in_data_buf;
    UINT    bytes_left_to_play;
    int     temp;
    
    if(argv>1)
    {
        if ((fp = fopen(argc[1], "rb")) == NULL)
        {
            printf("\nError opening file %s -- PROGRAM TERMINATED", argc[1]);
            exit(0);
        }
    }
    else
    {
        printf("\nUsage : DMAW32 wav-file\n");
        exit(0);
    }

    _clearscreen(_GCLEARSCREEN);
    printf("\tProtected Mode DMA Demo Program for playing .WAV Files\n");
    printf("\t------------------------------------------------------\n");

    // ------ Verify WAV format ------
    if(Chk_hdr(fp) == FAIL)
    {
        printf("Header check error - PROGRAM ABORTED\n");
        fclose(fp);
        exit(0);
    }

    Mode = (wavehdr.channel == 1) ? MONO : STEREO;

    // ------ Allocate memory buffers ------
    if( (data_buf = (UCHAR *) malloc((size_t) wavehdr.data_len)) == NULL)
    {
        printf("DATABUF malloc error\n");
        fclose(fp);
        exit(0);
    }
    
    if(AllocateDMABuffer() == FAIL)
    {
        printf("MALLOC ERROR\n");
        fclose(fp);
        exit(0);
    }

    fread(data_buf, wavehdr.data_len, 1, fp);
    fclose(fp);

    // ------ Get environment settings & check for validity ------
    if(GetBlasterEnv() == FAIL)
    {
        printf("BLASTER env. string or parameter(s) missing -- ABORTED!");
        MemUnlock((void *) &DMA_buffer, BUFSIZE);
        DOSMemFree(DMA_buffer_selector);
        exit(0);
    }

    // ------ Reset the DSP ------
    if(ResetDSP() == FAIL)
    {
        printf("Unable to reset DSP\n");
        MemUnlock((void *) &DMA_buffer, BUFSIZE);
        DOSMemFree(DMA_buffer_selector);
        exit(0);
    }
    printf("\n\nDSP version = %d\n\n",DSP_ver);

    if((DSP_ver < 4) && (wavehdr.bits_per_sample == 16))
    {
        printf("\n** DSP version %d.xx does not support 16-bit files.\n",
                DSP_ver);
        MemUnlock((void *) &DMA_buffer, BUFSIZE);
        DOSMemFree(DMA_buffer_selector);
        exit(0);
    }

    // ------ Mixer setting & setup new ISR ------
    SetMixer();
    SetISR(DMAISR);

    // ------ Initialize DMA & DSP chip ------
    if(InitDMADSP() == FAIL)
    {
        printf("InitDMADSP() fails - PROGRAM ABORTED!\n");
        MemUnlock((void *) &DMA_buffer, BUFSIZE);
        DOSMemFree(DMA_buffer_selector);
        exit(0);
    }

    // ------ Fill 1st half buffer ------
    printf("\nPlaying file >> %s\n",argc[1]);
    printf("\n    Mode  = %s\n", (Mode == MONO) ? "MONO" : "STEREO");
    printf("    Type  = %d-bit samples\n", (UINT) wavehdr.bits_per_sample);
    printf("    Freq. = %lu Hz\n", (ULONG) wavehdr.samples_per_sec);
    printf("    Size  = %lu Bytes\n", (ULONG) wavehdr.data_len);
  
    DMA_buf_to_fill             = 0;
    End_of_data                 = FALSE;
    DMA_buf_now_playing         = 0;
    Last_buf_played             = FALSE;
    bytes_left_in_data_buf      = wavehdr.data_len;

    bytes_left_to_play = FillBuffer(&data_buf, &bytes_left_in_data_buf);

    // ------ Begin playing ------
    if(wavehdr.data_len < BUFSIZE/2)
    {
        Play(bytes_left_to_play, AUTO_INIT);
        while(DMA_buf_now_playing == 0) ;
    }
    else
    {
        Play(bytes_left_to_play, AUTO_INIT);
        FillPlayBuf(data_buf, bytes_left_in_data_buf);
    }

    if(HS_mode == TRUE)    // if 8 bit high speed mode is true then reset DSP
        ResetDSP();
    else
        DSPout((wavehdr.bits_per_sample == 8) ?     // Done playing, Halt DMA
                dspExitAutoInitDMA8 : dspExitAutoInitDMA16);
                
    DSPout(dspTurnSpeakerOFF);                  // Turn OFF speaker
    
    if((DSP_ver < 4) && (Mode == STEREO))       // Restore original filter
    {                                           // setting for SBPro & MONO.
        outp(dspMixerAddr, mixOUTFILTER);
        outp(dspMixerData, (int) Output_filter & 0xFD);
    }
    
    // ------ Restore original ISR & do clean up ------
    RestoreISR();
    MemUnlock((void *) &DMA_buffer, BUFSIZE);
    DOSMemFree(DMA_buffer_selector);

    printf("DONE\n");
}// main()


// --------------------------------------------------------
// Function     : Chk_hdr()
// Description  : Check for validity of wave file header
// --------------------------------------------------------
int Chk_hdr(FILE *fp)
{
    char * dummy[80];

    memset(&wavehdr, 0, sizeof(wavehdr));
    fread(&wavehdr, 44, 1, fp);
    
    if( memcmp(wavehdr.format, "RIFF", 4))
    {
        printf("File is not a valid .WAV file\n");
        return (FAIL);
    }

    if( memcmp(wavehdr.wave_fmt, "WAVEfmt ", 8))
    {
        printf("File is not a valid .WAV file\n");
        return (FAIL);
    }

    if( !((wavehdr.channel == 1) || (wavehdr.channel == 2)))
    {
        printf("Unknown number of channels -> %d\n", wavehdr.channel);
        return (FAIL);
    }

    if (memcmp(wavehdr.data, "data", 4))
    {
        if (memcmp(wavehdr.data, "fact", 4))
        {
            printf("Unknown file format\n");
            return (FAIL);
        }
        while(wavehdr.data_len)
        {
            fread(dummy, (wavehdr.data_len%80), 1, fp);  // Get file type description.
            wavehdr.data_len -= wavehdr.data_len%80;
        }
        fread(wavehdr.data, 8, 1, fp);
        if (memcmp(wavehdr.data, "data", 4))
        {
            printf("Unknown file format\n");
            return (FAIL);
        }
    }

    return (SUCCESS);
}//Chk_hdr()

// --------------------------------------------------------
// Function     : DOSMemAlloc()
// Description  : Allocate DOS memory
// --------------------------------------------------------
void * DOSMemAlloc(DWORD size)
{
    union REGS r;

    r.x.eax = 0x0100;
    r.x.ebx = (size + 15) >> 4;
    int386(0x31, &r, &r);
    if(r.x.cflag)
        return ((DWORD) 0);

    DMA_buffer_selector = r.w.dx;
    return ((void *) ((r.x.eax & 0xFFFF) << 4));
}//DOSMemAlloc()

// --------------------------------------------------------
// Function     : DOSMemFree()
// Description  : Free DOS memory
// --------------------------------------------------------
void DOSMemFree(WORD selector)
{
    union REGS r;

    r.x.eax = 0x0101;
    r.w.dx = selector;
    int386(0x31, &r, &r);
}//DOSMemFree()

// --------------------------------------------------------
// Function     : MemLock()
// Description  : Lock Memory
// --------------------------------------------------------
void MemLock(void *mem, WORD size) 
{
    union REGS  r;
    
    r.x.eax = 0x0600;
    r.w.bx = (FP_OFF(mem) & 0xFFFF0000) >> 16;
    r.w.cx = FP_OFF(mem) & 0x0000FFFF;
    r.w.si = 0;
    r.w.di = size;
    int386(0x31, &r, &r);
}//MemLock()

// --------------------------------------------------------
// Function     : MemUnlock
// Description  : Unlock memory region
// --------------------------------------------------------
void MemUnlock(void *mem, WORD size) 
{
    union REGS  r;
    
    r.x.eax = 0x0601;
    r.w.bx = (FP_OFF(mem) & 0xFFFF0000) >> 16;
    r.w.cx = FP_OFF(mem) & 0x0000FFFF;
    r.w.si = 0;
    r.w.di = size;
    int386(0x31, &r, &r);
}//MemUnlock()

// --------------------------------------------------------
// Function     : AllocateDMABuffer()
// Description  : Allocate memory for the DMA buffer within
//                the same memory page
// --------------------------------------------------------
DWORD AllocateDMABuffer(void)
{
    union REGS      r;
    UCHAR           buffer_allocated = FALSE,
                    done = FALSE;
    WORD            buffer_sel[10];
    int i, Index = 0;
    
    do
    {
        if(! (DMA_buffer = (UCHAR *) DOSMemAlloc(BUFSIZE) ) )
            done = TRUE;
        else
        {   // ------ Save every memory allocation performed ------
            buffer_sel[Index] = DMA_buffer_selector;
            Index++;
            if(Index == 10)
               done = TRUE;
            // ------ Check page crossing ------
            if( ((FP_OFF(DMA_buffer) + BUFSIZE+15)  & 0xF0000)
                == (FP_OFF(DMA_buffer) & 0xF0000) )
            {
                buffer_allocated = TRUE;
                done = TRUE;
                MemLock((void *) &DMA_buffer, BUFSIZE);
            }
        }
    } while(!done);

    if (!buffer_allocated)
        Index++;

    // ------ Free all memory blocks crossing a page boundary ------
    for(i=0; i<Index-1; i++)
        DOSMemFree(buffer_sel[i]);
        
    return ((buffer_allocated) ? SUCCESS : FAIL);
}//AllocateDMABuffer()

// --------------------------------------------------------
// Function     : ResetDSP()
// Description  : Resets the DSP chip
// --------------------------------------------------------
char ResetDSP(void)
{
    UINT val;

    outp(Base + dspReset, (int) 1);
    delay(10);
    outp(Base + dspReset, (int) 0);

    // ------ Check for correct DSP ready response ------
    if(DSPin(&val))
        if(val==dspReady)
        {
            // ------ Get DSP version ------
            DSPout(dspGetDSPVersion);
            DSPin(&DSP_ver);
        
            return(SUCCESS);
        }

    return (FAIL);
}//ResetDSP()

// --------------------------------------------------------
// Function     : InitDMADSP()
// Description  : Uses the WAV header information to program
//                the DMA and DSP chips.
//                (Note: The DMA chip is always programmed for
//                 auto-init mode.
//---------------------------------------------------------
char InitDMADSP(void)
{
    dmaportstruct dma;
    int page, offset, temp;

    // ------ Get DMA setting ------        
    if(wavehdr.bits_per_sample == 8)
    {
        DMA_16bit = FALSE;
        if(DMA_chan8 != 2)
            dma = dmaport[DMA_chan8];
        else
        {
            printf("File is 8 bit -- invalid 8-bit channel");
            return (FAIL);
        }
    }
    else
    {
        DMA_16bit = TRUE;
        if((DMA_chan16 >= 5) && (DMA_chan16 <= 7))
            dma = dmaport[DMA_chan16];
        else
        {
            printf("File is 16 bit -- invalid 16-bit channle");
            return (FAIL);
        }
        DMA_chan16 -= 4;
    }

    page = (int) (FP_OFF(DMA_buffer) >> 16);
    offset = (int) (FP_OFF(DMA_buffer) & 0xFFFF);

    if((DSP_ver < 4) && (Mode == STEREO))
        SetSBProStereo(&dma, page, offset);
        
    if(wavehdr.bits_per_sample == 8)    // Program 8 bit DMA controller
    {
        outp(dma.mask, (int) (DMA_chan8 | 4));           // disable DMA
        outp(dma.FF, (int) 0);                          // clear flip-flop

        outp(dma.mode, (int) (DMA_chan8 | 0x58));        // 8 bit auto-init
        outp(dma.count, (int) ((BUFSIZE - 1) & 0xFF));  // LO byte of count
        outp(dma.count, (int) ((BUFSIZE - 1) >> 8));    // HI byte of count
    }
    else        // Program 16 bit DMA controller
    {
    // Offset for 16-bit DMA channel must be calculated differently...
    // Shift Offset 1 bit right, then copy LSB of Page to MSB of Offset.
        temp = page & 0x0001;
        temp <<= 15;
        offset >>= 1;
        offset &= 0x7FFF;
        offset |= temp;

        outp(dma.mask, (int) (DMA_chan16 | 4));         // disable DMA
        outp(dma.FF, (int) 0);                          // clear flip-flop

        outp(dma.mode, (int) (DMA_chan16 | 0x58));      // 16 bit auto-init
        outp(dma.count, (int) ((BUFSIZE/2 - 1) & 0xFF));// LO byte of count
        outp(dma.count, (int) ((BUFSIZE/2 - 1) >> 8));  // HI byte of count
    }

    outp(dma.page, page);                   // Physical page number
    outp(dma.addr, (int) (offset & 0xFF));  // LO byte address of buffer
    outp(dma.addr, (int) (offset >> 8));    // HI byte address of buffer

    // ------ DONE programming DMA, enable it ------
    if(wavehdr.bits_per_sample == 8)
        outp(dma.mask, DMA_chan8);
    else
        outp(dma.mask, DMA_chan16);

    if(DSP_ver < 4)
    {   // Set transfer time constant (HI-byte only)
        DSPout((UINT) dspTimeConstant);
        DSPout((UINT) (256 - 1000000/(Mode*wavehdr.samples_per_sec)));
    }

    DSPout(dspTurnSpeakerON);       // Turn ON speaker before doing D/A conv.
    return (SUCCESS);
}// InitDMADSP()

// --------------------------------------------------------
// Function     : SetSBProStereo()
// Description  : Set up SBPro hardware for stereo output.
//                Send a silent byte to the DSP.
// --------------------------------------------------------
void SetSBProStereo(dmaportstruct *dma, int page, int offset)
{
    outp(dspMixerAddr, mixOUTFILTER);
    Output_filter = (UINT) inp(dspMixerData);
    outp(dspMixerData, (int) Output_filter | 0x02);

    outp(dma->mask, (int) (DMA_chan8 | 4));           // disable DMA
    outp(dma->FF, (int) 0);                           // clear flip-flop

    outp(dma->mode, (int) (DMA_chan8 | 0x48));       // 8 bit single cycle
    outp(dma->count, (int) 1);                       // LO byte of count
    outp(dma->count, (int) 0);                       // HI byte of count

    outp(dma->page, page);                   // Physical page number
    outp(dma->addr, (int) (offset & 0xFF));  // LO byte address of buffer
    outp(dma->addr, (int) (offset >> 8));    // HI byte address of buffer

    outp(dma->mask, DMA_chan8);              // end DMA programming

    DMA_buf_now_playing = 0;
    *DMA_buffer = 0x80;
    DSPout(0x0014);                         // DSP output one silent byte
    DSPout(0);
    DSPout(0);

    while(DMA_buf_now_playing == 0) ;
    outp(dspMixerData, (int) Output_filter | 0x22); // set filter OFF & STEREO
}    

// --------------------------------------------------------
// Function     : SetMixer()
// Description  : Set microphone volume to zero and voice
//                output to maximum.
// --------------------------------------------------------
void SetMixer(void)
{
    outp(Base + dspMixerAddr, (int) mixMICVOL);
    outp(Base + dspMixerData, (int) 0x00);

    outp(Base + dspMixerAddr, (int) mixVOCVOL);
    outp(Base + dspMixerData, (int) 0xFF);
}//SetMixer()

// --------------------------------------------------------
// Function     : Play()
// Description  : Setup the playback depending on the number
//                of bits per sample, MONO/STEREO & DMAMode
// --------------------------------------------------------
void Play(UINT bytes_left, char dma_mode)
{
    // ------ If BytesLeftToPlay is 1 or 0, make sure when DSPout() 
    // ------ if called, the count does not wrap around when 1 sample
    // ------ is subtracted ! ------
    if(bytes_left <= 1 && DMA_16bit)
        bytes_left = 2;
    else if (bytes_left == 0 && !DMA_16bit)
        bytes_left = 1;

    if(DSP_ver < 4) // SBPro (DSP ver 3.xx)
    {
        if (dma_mode == AUTO_INIT)
        {
            DSPout(dspBlockSize);
            DSPout((int) ((bytes_left - 1) & 0x00FF));
            DSPout((int) ((bytes_left - 1) >> 8));
            // ------ set Normal/High-Speed 8 bit Auto-init ------
            if((Mode == STEREO) || (wavehdr.samples_per_sec >= 23000))
            {
                HS_mode = TRUE;
                DSPout(0x0090);
            }
            else    
                DSPout(0x001C);
        }
        else
        {
            // ------ set Normal/High-Speed 8 bit Single-cycle ------
            if((Mode == STEREO) || (wavehdr.samples_per_sec >= 23000))
            {
                HS_mode = TRUE;
                DSPout(0x0091);
            }
            else
                DSPout(0x0014);
                
            DSPout((bytes_left - 1) & 0x00FF);  // LO byte size
            DSPout((bytes_left - 1) >> 8);       // HI byte size
        }
    }
    else if(DSP_ver == 4)// SB16 (DSP ver 4.xx)
    {
        DSPout(dspOutputSampleRate);     // DSP output transfer rate
        DSPout((int) ((wavehdr.samples_per_sec & 0x0000FF00) >> 8));// HI byte
        DSPout((int) (wavehdr.samples_per_sec & 0x000000FF));       // LO byte

        if(dma_mode == AUTO_INIT)    // Auto-init 8/16 bit
            DSPout((wavehdr.bits_per_sample == 8) ? 0x00C6 : 0x00B6);
        else                        // Single-cycle 8/16 bit
            DSPout((wavehdr.bits_per_sample == 8) ? 0x00C0 : 0x00B0);

        if(wavehdr.bits_per_sample == 8)    
            DSPout((Mode == MONO) ? 0x0000 : 0x0020);   // 8 bit MONO/STEREO
        else
            DSPout((Mode == MONO) ? 0x0010 : 0x0030);   // 16 bit MONO/STEREO

        // ------ Program number of samples to play ------
        DSPout((int) ((bytes_left/(wavehdr.bits_per_sample/8) - 1) & 0x00FF));
        DSPout((int) ((bytes_left/(wavehdr.bits_per_sample/8) - 1) >> 8));
    }
    
    return;
}// Play()

// --------------------------------------------------------
// Function     : FillBuffer()
// Description  : Fill each half of the DMA buffer
// --------------------------------------------------------
UINT FillBuffer(UCHAR **buffer, ULONG *bytes_left)
{
    UINT count;
    UCHAR *bufptr;

    // ------ Set the pointer to the desired buffer to fill ------    
    bufptr = DMA_buffer + ((DMA_buf_to_fill == 0) ? 0 : BUFSIZE/2);
    
    if(*bytes_left < BUFSIZE/2)
    {   // fill last block of data and set EndOfData
        memcpy(bufptr, *buffer, *bytes_left);
        if(HS_mode)
        {
            bufptr += *bytes_left;
            memset(bufptr, 0x80, BUFSIZE/2 - *bytes_left);
            count = BUFSIZE/2;
            if(DMA_buf_to_fill == 1)
                End_of_data = TRUE;
        }
        else
        {
            count = *bytes_left;
            End_of_data = TRUE;
        }
        *bytes_left = 0;
    }
    else
    {   // fill data block into buffer
        memcpy(bufptr, *buffer, BUFSIZE/2);
        count = BUFSIZE/2;
        *buffer += BUFSIZE/2;
        *bytes_left -= BUFSIZE/2;
    }

    DMA_buf_to_fill ^= 1;    // Toggle to fill other half of buffer next time
    
    return (count);
}// FillBuffer()

// --------------------------------------------------------
// Function     : FillPlayBuf()
// Description  : Keeps the DMA buffer filled with new data
//                until end of data
// --------------------------------------------------------
void FillPlayBuf(UCHAR *buffer, ULONG bytes_left)
{
    UINT bytes_in_buffer;

    do
    {   // Wait for buffer to finish play
        while( DMA_buf_to_fill == DMA_buf_now_playing) ;
        bytes_in_buffer = FillBuffer(&buffer, &bytes_left);
        if(!HS_mode && (bytes_in_buffer < BUFSIZE/2))
            Play(bytes_in_buffer, SINGLE_CYCLE);
    } while (!End_of_data) ; // End_of_data set in FillBuffer()

    while(Last_buf_played == FALSE) ;    // Wait until done playing
    
    return;
}// FillPlayBuf()

// --------------------------------------------------------
// Function     : GetBlasterEnv()
// Description  : Get the BLASTER environment variable for
//                the I/O port address, DMA channels and
//                IRQ nubmer.
// --------------------------------------------------------
char GetBlasterEnv(void)
{
  char  buffer[5],
        dma_chan_found  = FALSE,
        io_port_found   = FALSE,
        irq_found       = FALSE,
        *env_string,
        save_char;

  int   digit,
        i,
        multiplier;

  env_string = getenv("BLASTER");

  if (env_string == NULL)
    return(FAIL);

  do
  {
    switch(*env_string)
    {
      case 'A':  // I/O base port address found
      case 'a':
    env_string++;
    for (i = 0; i < 3; i++)  // Grab the digits
    {
      buffer[i] = *env_string;
      env_string++;
    }

    // The string is in HEX, convert it to decimal
    multiplier = 1;
    Base = 0;
    for (i -= 1; i >= 0; i--)
    {
      // Convert to HEX
      if (buffer[i] >= '0' && buffer[i] <= '9')
        digit = buffer[i] - '0';
      else if (buffer[i] >= 'A' && buffer[i] <= 'F')
        digit = buffer[i] - 'A' + 10;
      else if (buffer[i] >= 'a' && buffer[i] <= 'f')
        digit = buffer[i] - 'a' + 10;

      Base = Base + digit * multiplier;
      multiplier *= 16;
    }

    io_port_found = TRUE;
      break;

      case 'D': // 8-bit DMA channel
      case 'd':
      case 'H': // 16-bit DMA channel
      case 'h':
    save_char = *env_string;
    env_string++;
    buffer[0] = *env_string;
    env_string++;

    if (*env_string >= '0' && *env_string <= '9')
    {
      buffer[1] = *env_string; // DMA Channel No. is 2 digits
      buffer[2] = NULL;
      env_string++;
    }
    else
      buffer[1] = NULL;       // DMA Channel No. is 1 digit

    if (save_char == 'D' || save_char == 'd')
      DMA_chan8  = atoi(buffer);  // 8-Bit DMA channel
    else
      DMA_chan16 = atoi(buffer);  // 16-bit DMA channel

    dma_chan_found = TRUE;
      break;

      case 'I':  // IRQ number
      case 'i':
    env_string++;
    buffer[0] = *env_string;
    env_string++;

    if (*env_string >= '0' && *env_string <= '9')
    {
      buffer[1] = *env_string; // IRQ No. is 2 digits
      buffer[2] = NULL;
      env_string++;
    }
    else
      buffer[1] = NULL;       // IRQ No. is 1 digit

    IRQ_num  = atoi(buffer);
    irq_found = TRUE;
      break;

      default:
    env_string++;
      break;
    }

  } while (*env_string != NULL);

  if (!dma_chan_found || !io_port_found || !irq_found)
    return(FAIL);

  return(SUCCESS);
}//GetBlasterEnv

// --------------------------------------------------------
// Function     : DSPout()
// Description  : Writes the value out to the DSP chip.
// --------------------------------------------------------
void DSPout(UINT val)
{
    while( inp(Base + dspWriteBuf) & 0x80); // wait for DSP ready
    outp(Base + dspWriteBuf, val);          // write data
}//DSPout()

// --------------------------------------------------------
// Function     : DSPin()
// Description  : Reads the value from the DSP chip
// --------------------------------------------------------
char DSPin(UINT *val)
{
    while( !(inp(Base + dspDataAvail) & 0x80)); // wait for available
    *val = inp(Base + dspReadData);             // data

    return (SUCCESS);
}//DSPin()

// --------------------------------------------------------
// Function     : SetISR()
// Description  : Save the original interrupt and replace
//                with the new ISR
// --------------------------------------------------------
void SetISR(void (__interrupt __far *new_vect)())
{
    union REGS r;
    struct SREGS sr;
    
    Intr_num = (IRQ_num < 8) ? (IRQ_num + 8) : (IRQ_num - 8 + 0x70);
    Intr_mask = 1 << IRQ_num;

    r.x.eax = 0x3500;
    r.h.al = Intr_num & 0xFF;
    sr.ds = sr.es = 0;
    int386x(0x21, &r, &r, &sr);
    IntrSave = MK_FP(sr.es, r.x.ebx);

    r.x.eax = 0x2500;
    r.h.al = Intr_num & 0xFF;
    r.x.edx = FP_OFF(new_vect);
    sr.ds = FP_SEG(new_vect);
    sr.es = 0;
    int386x(0x21, &r, &r, &sr);
    
    outp(PIC1MASK, inp(PIC1MASK) & ~Intr_mask);
    outp(PIC2MASK, inp(PIC2MASK) & ~(Intr_mask >> 8));
}//SetISR()

// --------------------------------------------------------
// Function     : RestoreISR()
// Description  : Restores the saved original ISR
// --------------------------------------------------------
void RestoreISR(void)
{
    union REGS r;
    struct SREGS sr;
    
    outp(PIC1MASK, inp(PIC1MASK) | Intr_mask);
    outp(PIC2MASK, inp(PIC2MASK) | (Intr_mask >> 8));

    r.x.eax = 0x2500;
    r.h.al = Intr_num & 0xFF;
    r.x.edx = FP_OFF(IntrSave);
    sr.ds = FP_SEG(IntrSave);
    sr.es = 0;
    int386x(0x21, &r, &r, &sr);
    
}//RestoreISR()

// --------------------------------------------------------
// Function     : DMAISR()
// Description  : The interrupt service routine which is
//                invoked every time the DSP chip completes
//                playing half of the DMA buffer.
// --------------------------------------------------------
void __interrupt __far DMAISR(void)
{
    static char second_last_buffer_played = FALSE;
    int IntStatus;
    
    if (DMA_16bit)
    {   // Check for correct interrupt status 
        outp(Base + dspMixerAddr, 0x82);    // selects intr. reg. in mixer
        IntStatus = inp(Base + dspMixerData);
        if (IntStatus & 2)
            inp(Base + dspDMA16Ack);        // Acknowledge interrupt (16-bit)
    }
    else
        inp(Base + dspDMA8Ack);           // Acknowledge interrupt (8-bit)

    if(IRQ_num > 8)                          // End of interrupt
        outp(PIC2MODE, PICEOI);
    outp(PIC1MODE, PICEOI);

    DMA_buf_now_playing ^= 1;                    // Toggle for next buffer to play

    if(second_last_buffer_played)
        Last_buf_played = TRUE;

    if(End_of_data)
        second_last_buffer_played = TRUE;

    return;
}//DMAISR()
