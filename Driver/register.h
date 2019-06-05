/**
 *  Register definition file.
 *
 *  Contains all register offsets, as well as number of registers
 *  contained in the SUPERIO ISA controller.
 *
 *  @date   2-6-2019
 *  @author Jesse Buhagiar
 */
#ifndef _REGISTER_H_
#define _REGISTER_H_

/**
 *  Control register write port  
 */
#define ISA_CONTROL_WRITE       0x00
#define ISA_CONTROL_READ_BIT    0x01
#define ISA_CONTROL_WRITE_BIT   0x02


/**
 *  Control register read port
 */
#define ISA_CONTROL_READ        0x01

/**
 *  Address register write port
 */
#define ISA_ADDRESS_WRITE       0x02

/**
 * Data register write port
 */
#define ISA_DATA_WRITE          0x03

/**
 * Data register read port
 */
#define ISA_DATA_READ           0x04

#endif
