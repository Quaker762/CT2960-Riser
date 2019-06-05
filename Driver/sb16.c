/**
 *  Implementation of sb16.h
 *
 */
#include "sb16.h"
#include "bridge.h"
#include "register.h"
#include "util.h"

#include <stdio.h>
#include <unistd.h>


void sb16_write(uint16_t addr, uint16_t data)
{
    isa_write_port(ISA_ADDRESS_WRITE, addr);
    isa_write_port(ISA_DATA_WRITE, data);
    isa_write_port(ISA_CONTROL_WRITE, ISA_CONTROL_WRITE_BIT);

    while(isa_read_port(ISA_CONTROL_READ) & ISA_CONTROL_WRITE_BIT);
}

uint16_t sb16_read(uint16_t addr)
{
    isa_write_port(ISA_ADDRESS_WRITE, addr);
    isa_write_port(ISA_CONTROL_WRITE, ISA_CONTROL_READ_BIT);

    while(isa_read_port(ISA_CONTROL_READ) & ISA_CONTROL_READ_BIT);

    return isa_read_port(ISA_DATA_READ);
}

void sb16_init()
{
    sb16_write(SB16_DSP_RESET, 1);
    wait_us(10);
    sb16_write(SB16_DSP_RESET, 0);
    
    uint16_t ret = 0;    
    
    for(int i = 0; i < 200; i++)
    {
        ret = sb16_read(SB16_DSP_STATUS);
        if(ret & 0x80)
            break; 
    }

    ret = sb16_read(SB16_DSP_READ);
    if(ret == 0xaa)
    {
        printf("%s: card initialised!\n", __PRETTY_FUNCTION__);
        return;
    }


    printf("%s: failed to initialise card! (0xff stands for fucking fuck!) 0x%x\n", __PRETTY_FUNCTION__, ret);

}

void sb16_playsound()
{
    sb16_write(0xD1, SB16_DSP_WRITE);

    sb16_write(0xF0, SB16_DSP_WRITE);

    while(1)
    {
        printf("Atetmpting to play 0x00\n");
        sb16_write(0x10, SB16_DSP_WRITE);
        sb16_write(0x00, SB16_DSP_WRITE);
        wait_us(300);

        printf("Attempting to play 0xff\n");
        sb16_write(0x10, SB16_DSP_WRITE);
        sb16_write(0xee, SB16_DSP_WRITE);
        wait_us(300);
    }
}
