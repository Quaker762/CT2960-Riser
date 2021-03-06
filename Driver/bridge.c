/**
 *  Implementation of bridge.h
 *
 */
#include "bridge.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "register.h"

#define ALT_LWFPGA_BRIDGE   0xff200000
#define REGFILE_BASE        0x00000000
#define REGFILE_SPAN        0x0000001f

uint32_t* regfile_base;

void bridge_init()
{
    int memdev;

    memdev = open("/dev/mem", (O_RDWR | O_SYNC));
    regfile_base = mmap(NULL, 
                        REGFILE_SPAN, 
                        (PROT_READ | PROT_WRITE), 
                        MAP_SHARED, memdev, 
                        (ALT_LWFPGA_BRIDGE + REGFILE_BASE));

    if(regfile_base == MAP_FAILED)
    {
        printf("%s:%d: Failed to map register file to address space! Aborting...\n", __PRETTY_FUNCTION__, __LINE__);
        exit(-1);
    }

    close(memdev);
}


void isa_write_port(uint8_t port, uint32_t value)
{   
    *(regfile_base + port) = (uint32_t)value;
}

uint16_t isa_read_port(uint8_t port)
{
    return *(regfile_base + port);
}

void dump_ram()
{
    for(int i = 0; i < REGFILE_SPAN; i++)
    {
        if(i % 4 == 0)
            printf("\n");
        else
            printf("0x%x ", regfile_base[i * 4]);
    }
}
