#include <setjmp.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <common_types.h>

/*
***************************
* Global Variables
***************************
*/
static volatile sig_atomic_t segv_flag;
static jmp_buf jmp_env;

/*
********************************************
* Pre: none
* Post: restore the original process state
* and return 1 to setjmp
********************************************
*/
static void segv_handler(int dummy) {
    segv_flag = 1;
    longjmp(jmp_env, 1);
}

/*
********************************************
* Pre: A pointer and it's length
* Post: Return true is the pointer is bad
* or false if otherwise
********************************************
*/

int IsReadBadPtr(void *ptr_buffer, unsigned long buffer_size) {
    struct sigaction oldaction, newaction;
    volatile struct customerRequest dummy;
    struct customerRequest *ptr = (struct customerRequest *)ptr_buffer;

    if (!buffer_size)
        return 0;

    /* storing the old signal environment and trapping SIGSEGV */
    newaction.sa_handler = segv_handler;
    newaction.sa_flags = 0;
    sigemptyset(&newaction.sa_mask);

    sigaction(SIGSEGV, &newaction, &oldaction);

    segv_flag = 0;

    /* Storing the process state so if any failure happens
    we can restore it to the original state */
    if (setjmp(jmp_env) == 0) {
        /* testing the pointer: only the first and the end are needed here since any failure to any of this would indicate there would an error on the entire range */
        dummy = ptr[0];
        dummy = ptr[buffer_size - 1];
    }

    /* restoring the original signal environment */
    sigaction(SIGSEGV, &oldaction, NULL);

    return segv_flag;
}