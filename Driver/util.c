/**
 *  Implementation of util.h
 *
 *
 */
#include <time.h>
#include "util.h"

void sleep_us(unsigned long long n)
{
    struct timespec time;

    time.tv_sec     = 0;
    time.tv_nsec    = n * 1000;

    nanosleep(&time, NULL);
}

void __attribute__((optimize("O0"))) wait_us(unsigned long long cycles)
{
    // Do nothing for 'cycles' cycles
    for(unsigned long long i = 0; i < cycles; i++)
        __asm__("nop");


}
