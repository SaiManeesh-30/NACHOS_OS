/* num_io.c
 *      Simple program to test whether the systemcall interface works.
 *
 *      Just do a syscall that read an interger and print it
 *
 */

#include "syscall.h"

int main(){

        int i;
        // i = Exec("exec", 10);
        MySleep(100);
        while(1){
                for(i=0;i<30000;i++);
                PrintString("In prog: [num_io2]\n");
        }
}

