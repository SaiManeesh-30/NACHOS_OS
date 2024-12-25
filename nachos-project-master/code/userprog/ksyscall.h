/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__
#define __USERPROG_KSYSCALL_H__

#include "kernel.h"
#include "synchconsole.h"
#include "ksyscallhelper.h"
#include <stdlib.h>
#include "list.h"

void SysHalt() { 
	
	clock_t diff = clock()-kernel->currentThread->start;
	printf("Time : %f", ((float)diff/CLOCKS_PER_SEC));
	kernel->interrupt->Halt(); }


int SysAdd(int op1, int op2) { return op1 + op2; }

int SysMul(int op1, int op2) { return op1 * op2; }

int SysReadNum() {
    readUntilBlank();

    int len = strlen(_numberBuffer);
    // Read nothing -> return 0
    if (len == 0) return 0;

    // Check comment below to understand this line of code
    if (strcmp(_numberBuffer, "-2147483648") == 0) return INT32_MIN;

    bool nega = (_numberBuffer[0] == '-');
    int zeros = 0;
    bool is_leading = true;
    int num = 0;
    for (int i = nega; i < len; ++i) {
        char c = _numberBuffer[i];
        if (c == '0' && is_leading)
            ++zeros;
        else
            is_leading = false;
        if (c < '0' || c > '9') {
            DEBUG(dbgSys, "Expected number but " << _numberBuffer << " found");
            return 0;
        }
        num = num * 10 + (c - '0');
    }

    // 00            01 or -0
    if (zeros > 1 || (zeros && (num || nega))) {
        DEBUG(dbgSys, "Expected number but " << _numberBuffer << " found");
        return 0;
    }

    if (nega)
        /**
         * This is why we need to handle -2147483648 individually:
         * 2147483648 is larger than the range of int32
         */
        num = -num;

    // It's safe to return directly if the number is small
    if (len <= MAX_NUM_LENGTH - 2) return num;

    /**
     * We need to make sure that number is equal to the number in the buffer.
     *
     * Ask: Why do we need that?
     * Answer: Because it's impossible to tell whether the number is bigger
     * than INT32_MAX or smaller than INT32_MIN if it has the same length.
     *
     * For example: 3 000 000 000.
     *
     * In that case, that number will cause an overflow. However, C++
     * doens't raise interger overflow, so we need to make sure that the input
     * string and the output number is equal.
     *
     */
    if (compareNumAndString(num, _numberBuffer))
        return num;
    else
        DEBUG(dbgSys,
              "Expected int32 number but " << _numberBuffer << " found");

    return 0;
}


int SysGetProcessList(int bufAddr){
    int processCount = 0;

    kernel->pTab->bmsem->P();

    // Iterate through the process table and fill the buffer with process info
    for (int i = 1; i < kernel->pTab->psize; i++) {
        if (kernel->pTab->pcb[i] != NULL) {
            processinfo pinfo;
            pinfo.pid = kernel->pTab->pcb[i]->thread->processID;
            strncpy(pinfo.name, kernel->pTab->pcb[i]->GetFileName(), sizeof(pinfo.name));
            pinfo.status = kernel->pTab->pcb[i]->thread->status; // or another status indicator

            // Copy process info to user space
            
            for (unsigned int j = 0; j < sizeof(processinfo); ++j) {
                int byteToWrite = ((char*)&pinfo)[j];  // Get the byte value
                if (!kernel->machine->WriteMem(bufAddr + processCount * sizeof(processinfo) + j, 1, byteToWrite)) {
                    kernel->pTab->bmsem->V(); // Release lock before returning

                    return -1; // Error writing to user space
                }
            }
            processCount++;
        }
    }

    kernel->pTab->bmsem->V(); // Release the lock


    // IntStatus currentLevel = kernel->interrupt->SetLevel(IntOff);
    // processCount = kernel->scheduler->getProcesses(bufAddr);
    // kernel->interrupt->SetLevel(currentLevel);
    return processCount;
}

void SysPrintNum(int num) {
    if (num == 0) return kernel->synchConsoleOut->PutChar('0');

    if (num == INT32_MIN) {
        kernel->synchConsoleOut->PutChar('-');
        for (int i = 0; i < 10; ++i)
            kernel->synchConsoleOut->PutChar("2147483648"[i]);
        return;
    }

    if (num < 0) {
        kernel->synchConsoleOut->PutChar('-');
        num = -num;
    }
    int n = 0;
    while (num) {
        _numberBuffer[n++] = num % 10;
        num /= 10;
    }
    for (int i = n - 1; i >= 0; --i)
        kernel->synchConsoleOut->PutChar(_numberBuffer[i] + '0');
}

char SysReadChar() { return kernel->synchConsoleIn->GetChar(); }

void SysPrintChar(char character) {
    kernel->synchConsoleOut->PutChar(character);
}

int SysRandomNum() { return random(); }

char* SysReadString(int length) {
    char* buffer = new char[length + 1];
    for (int i = 0; i < length; i++) {
        buffer[i] = SysReadChar();
    }
    buffer[length] = '\0';
    return buffer;
}

void SysPrintString(char* buffer, int length) {
    for (int i = 0; i < length; i++) {
        if(buffer[i] == ' ')
        {
            kernel->synchConsoleOut->PutChar(' ');
        }
        else
        {
            /* code */
            kernel->synchConsoleOut->PutChar(buffer[i]);
        }
    }
}

bool SysCreateFile(char* fileName) {
    bool success;
    int fileNameLength = strlen(fileName);

    if (fileNameLength == 0) {
        DEBUG(dbgSys, "\nFile name can't be empty");
        success = false;

    } else if (fileName == NULL) {
        DEBUG(dbgSys, "\nNot enough memory in system");
        success = false;

    } else {
        DEBUG(dbgSys, "\nFile's name read successfully");
        if (!kernel->fileSystem->Create(fileName)) {
            DEBUG(dbgSys, "\nError creating file");
            success = false;
        } else {
            success = true;
        }
    }

    return success;
}

int SysOpen(char* fileName, int type) {
    if (type != 0 && type != 1) return -1;

    int id = kernel->fileSystem->Open(fileName, type);
    if (id == -1) return -1;
    DEBUG(dbgSys, "\nOpened file");
    return id;
}

int SysClose(int id) { return kernel->fileSystem->Close(id); }

int SysRead(char* buffer, int charCount, int fileId) {
    if (fileId == 0) {
        return kernel->synchConsoleIn->GetString(buffer, charCount);
    }
    return kernel->fileSystem->Read(buffer, charCount, fileId);
}

int SysWrite(char* buffer, int charCount, int fileId) {
    if (fileId == 1) {
        return kernel->synchConsoleOut->PutString(buffer, charCount);
    }
    return kernel->fileSystem->Write(buffer, charCount, fileId);
}

int SysSeek(int seekPos, int fileId) {
    if (fileId <= 1) {
        DEBUG(dbgSys, "\nCan't seek in console");
        return -1;
    }
    return kernel->fileSystem->Seek(seekPos, fileId);
}

int SysExec(char* name,int prnum) {
    // cerr << "call: `" << name  << "`"<< endl;
    OpenFile* oFile = kernel->fileSystem->Open(name);
    if (oFile == NULL) {
        DEBUG(dbgSys, "\nExec:: Can't open this file.");
        return -1;
    }

    delete oFile;

    // Return child process id
    return kernel->pTab->ExecUpdate(name,prnum);
}

void SysMySleep(int sleep_time){
    kernel->scheduler->addInSleeplist(sleep_time * 10000); 
    IntStatus currentLevel = kernel->interrupt->SetLevel(IntOff);
    kernel->currentThread->Sleep(false);
    kernel->interrupt->SetLevel(currentLevel);
}

void SysWaitUntil(int pid)
{
    kernel->scheduler->AppendWait(pid);  //pvn
    IntStatus currentLevel = kernel->interrupt->SetLevel(IntOff);
    kernel->currentThread->Sleep(false);
    kernel->interrupt->SetLevel(currentLevel);
} 

int SysJoin(int id) { return kernel->pTab->JoinUpdate(id); }

int SysExit(int id) { return kernel->pTab->ExitUpdate(id); }

int SysCreateSemaphore(char* name, int initialValue) {
    int res = kernel->semTab->Create(name, initialValue);

    if (res == -1) {
        DEBUG('a', "\nError creating semaphore");
        delete[] name;
        return -1;
    }

    return 0;
}

int SysWait(char* name) {
    int res = kernel->semTab->Wait(name);

    if (res == -1) {
        DEBUG('a', "\nSemaphore not found");
        delete[] name;
        return -1;
    }

    return 0;
}

int SysSignal(char* name) {
    int res = kernel->semTab->Signal(name);

    if (res == -1) {
        DEBUG('a', "\nSemaphore not found");
        delete[] name;
        return -1;
    }

    return 0;
}

int SysGetPid() { return kernel->currentThread->processID; }

#endif /* ! __USERPROG_KSYSCALL_H__ */
