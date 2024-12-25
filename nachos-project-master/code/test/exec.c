/* exec.c
 *	Simple program to test the exec system call.
 */

#include "syscall.h"
#define stdin 0
#define stdout 1

struct processinfo2 {  // Ensure that processinfo is defined here if not in
                       // utility.h
    int pid;
    char name[30];
    int status;
};
int main() {
    int numProcesses;
    int pid, pid2, pid3;
    int i;
    struct processinfo2 buf[10];  // Buffer to hold the process list
    pid = Exec("num_io", 11);
    if (pid < 0) {
        Write("Exec failed: ", 14, stdout);
        PrintNum(pid);
    }
    pid3 = Exec("num_io3", -3);
    if (pid3 < 0) {
        Write("Exec failed: ", 14, stdout);
        PrintNum(pid3);
    }
    // pid2 = Exec("num_io2", 10);
    // if (pid2 < 0) {
    //     Write("Exec failed: ", 14, stdout);
    //     PrintNum(pid2);
    // }

    // PrintNum(numProcesses);
    WaitUntil(pid);
    PrintString("Before\n");
    // PrintString("After\n");


    while (1) {
        for (i = 0; i < 300000; i++);
        // numProcesses = GetProcessList((int)buf);

        // for (i = 0; i < numProcesses; i++) {
        //     PrintString("Name: ");
        //     PrintString(buf[i].name);
        //     PrintString(", ");
        //     PrintString("status: ");
        //     PrintNum(buf[i].status);
        //     PrintString(", ");
        //     PrintString("pid: ");
        //     PrintNum(buf[i].pid);
        //     PrintString("\n");
        //     // printf("Process %d: PID = %d, Name = %s, Status = %d\n",
        //     //        i, buf[i].pid, buf[i].name, buf[i].status);
        // }
        PrintString("In prog: [exec]\n");
    }
}
