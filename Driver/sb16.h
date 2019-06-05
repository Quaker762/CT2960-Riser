/**
 *  Create SoundBlaster 16 (CT2960) related addresses
 *
 *  Provides an interface to the Creative Labs SoundBlaster VIBRA16c sound card.
 *  This file handles all card initilisation, and has a list of important addresses
 *  needed to bring the card up
 *
 *  @date 2-6-2019
 *  @author Jesse Buhagiar
 */
#ifndef _SB16_H_
#define _SB16_H_

#include <stdint.h>

// IO port definitions. DO NOT MODIFY!!!
#define SB16_BASE               0x220
#define SB16_DSP_RESET          SB16_BASE + 0x6
#define SB16_DSP_READ           SB16_BASE + 0xa
#define SB16_DSP_COMMAND        SB16_BASE + 0xc
#define SB16_DSP_STATUS         SB16_BASE + 0xc
#define SB16_DSP_WRITE          SB16_BASE + 0xc
#define SB16_DSP_DATA_AVAIL     SB16_BASE + 0xe

/**
 *  Initialise the VIBRA16c Sound Card
 */
void sb16_init();


/**
 * Write data to the VIBRA16c card
 */
void sb16_write(uint16_t addr, uint16_t data);

/**
 *  Read data from the VIBRA16c card
 */
uint16_t sb16_read(uint16_t addr);

void sb16_playsound();


#endif
