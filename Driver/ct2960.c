/**
 *
 *
 */
#include <stdio.h>
#include <time.h>
#include <sys/resource.h>

#include "bridge.h"
#include "register.h"
#include "util.h"

#include "sb16.h"

#define PRIORITY_NORMAL 0
#define PRIORITY_HIGH   -20

/**
 *  Set the process of this priority so that the scheduler.
 *
 *  A value of PRIORITY_HIGH will ensure that the scheduler (which is not
 *  real time on this distribution) 
 */
void pid_set_priority(int priority)
{
    int     which = PRIO_PROCESS;
    int     ret;
    id_t    pid;

    pid = getpid(); // Get the PID of this process
    ret = setpriority(which, pid, priority); // Set the process priority

    if(ret != 0)
    {
        printf("%s: Unable to set process priority to %d!\n Aborting...", __PRETTY_FUNCTION__, priority);
    }
}

int main()
{
    // Set the process priority to the highest available!
    pid_set_priority(PRIORITY_HIGH);

    // Initialise the bridge in C
    bridge_init();

    sb16_init();

    return 0;
}
