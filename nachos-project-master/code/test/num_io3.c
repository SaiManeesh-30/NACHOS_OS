/* num_io.c
 *      Simple program to test whether the systemcall interface works.
 *
 *      Just do a syscall that read an interger and print it
 *
 */

#include "syscall.h"

int main(){

        int i;
        while(1){
                for(i=0;i<300000;i++);
                PrintString("In prog: [num_io3]\n");
        }
}

