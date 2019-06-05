/**
 *  Utility functions
 */
#ifndef _UTIL_H_
#define _UTIL_H_

/**
 *  Sleep for n microseconds
 */
void sleep_us(unsigned long long n);

/**
 *  Wait for a set number of cycles by using a for loop 
 *  that generates 'nop' instructions
 *
 *  @note This function is NOT optimized at all. The optimizer will ignore it.
 */
void __attribute__((optimize("O0"))) wait_us(unsigned long long cycles);




#endif
