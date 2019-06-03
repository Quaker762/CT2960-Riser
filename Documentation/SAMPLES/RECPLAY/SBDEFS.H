/*
 * SBDEFS.H
 *
 * Constants for Sound Blaster, PIC, and DMAC.
 */

/* constants for 8250 Peripheral Interrupt Controller */
#define PIC1MODE     0x20       // for irq 0 - 7
#define PIC1MASK     0x21
#define PIC2MODE     0xA0       // for irq 8 - 15
#define PIC2MASK     0xA1
#define PICEOI       0x20       // End Of Interrupt, for either PIC



/* DSP commands */
#define dspcmdDirect8DAC  0x10   /* DSP Direct 8-bit DAC command */
#define dspcmdDMADAC      0x14   /* DSP 8-bit DMA DAC command */
#define dspcmdAUTODMADAC  0x1C   /* DSP 8-bit ADC Auto-init DMA transfer */

#define dspcmdDirectADC   0x20   /* DSP Direct ADC command */
#define dspcmdDMAADC      0x24   /* DSP 8-bit DMA ADC command */
#define dspcmdAUTODMAADC  0x2C   /* DSP 8-bit ADC Auto-init DMA transfer */

#define dspcmdHaltDMA8    0xD0   /* pause 8-bit DMA */
#define dspcmdContDMA8    0xD4   /* continue 8-bit DMA */
#define dspcmdExitAuto8   0xDA   /* stop auto-init DMA transfer */

#define dspcmdHaltDMA16   0xD5   /* pause 16-bit DMA */
#define dspcmdContDMA16   0xD6   /* continue 16-bit DMA */
#define dpscmdExitDMA16   0xD9   /* stop 16-bit DMA */

#define dspcmdSpeakerOn   0xD1   /* turn speaker on */
#define dspcmdSpeakerOff  0xD3   /* turn speaker off */
#define dspcmdSpeakerStat 0xD8   /* get speaker status */

#define dspcmdTimeConst   0x40   /* Set time constant */
#define dspcmdDASampRate  0x41   /* D/A Sample Rate (SB16 only) */
#define dspcmdADSampRate  0x42   /* A/D Sample Rate (SB16 only) */
#define dspcmdBlockSize   0x48   /* Set block size */

#define dspSB16DMA16      0xB0   /* PARTIAL command for 16-bit DMA on SB16 */
#define dspSB16DMA8       0xC0   /* PARTIAL command for 8-bit DMA on SB16 */
#define dspSB16DAC     (0 << 3)  /* bit for SB16 D/A */
#define dspSB16ADC     (1 << 3)  /* bit for SB16 A/D */
#define dspSB16SC      (0 << 2)  /* bit for SB16 Single Cycle */
#define dspSB16AI      (1 << 2)  /* bit for SB16 Auto Init */
#define dspSB16FifoOff (0 << 1)  /* bit for SB16 FIFO off */
#define dspSB16FifoOn  (1 << 1)  /* bit for SB16 FIFO on */

#define dspSB16ModeMono     (0 << 5)  // SB16 mono voice I/O
#define dspSB16ModeStereo   (1 << 5)  // SB16 stereo voice I/O
#define dspSB16ModeUnsigned (0 << 4)  // SB16 unsigned voice I/O
#define dspSB16ModeSigned   (1 << 4)  // SB16 signed voice I/O



/* DSP port address offsets from base port */
#define dspoffsetWrBuf     0xC    /* DSP Write Buffer address */
#define dspoffsetReadData  0xA    /* DSP Read Data address */
#define dspoffsetDataAvail 0xE    /* DSP Data Available address */
#define dspoffsetReset     0x6    /* dsp reset port address */
#define dspoffsetDMA8Ack   0xE    /* 8-bit DMA  interrupt acknowledge (same
 as read data port */
#define dspoffsetDMA16Ack  0xF    /* 16-bit DMA interrupt acknowledge */
#define dspoffsetMixerAddr 0x4    /* mixer address (register select) port */
#define dspoffsetMixerData 0x5    /* mixer data port */


/* Mixer registers */
#define MASTERVOL    0X22
#define VOCVOL       0X04
#define FMVOL        0X26
#define CDVOL        0X28
#define MICVOL       0X0A
#define ADCSELECT    0X0C
#define INTSTATUS    0X82
#define SB16OUTPUT   0X3C
#define SB16INPUTL   0X3D
#define SB16INPUTR   0X3E

/* SB16 Interrupt status bits */
#define DMA8IntStatBit   1
#define DMA16IntStatBit  2
#define MPU401IntStatBit 4


#define dspReady 0xAA     /* byte returned by dsp when ready after reset */


#define DMAMODEWRITE  0x54    // partial mode.  fill in with channel
#define DMAMODEREAD   0x58    // same.

