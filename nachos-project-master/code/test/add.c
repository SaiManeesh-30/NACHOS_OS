/* add.c
 *	Simple program to test whether the systemcall interface works.
 *
 *	Just do a add syscall that adds two values and returns the result.
 *
 */

#include "syscall.h"

int main() {
    int result;
    int result1;
    result = Add(42, 23);
    result1 = Mul(10,3);
    GetProcessList(10);
    MySleep(200);
    PrintNum(result); 
    PrintString("Hello World  ");
    // Halt();
    /* not reached */
}
