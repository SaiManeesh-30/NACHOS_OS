#ifndef PTABLE_H
#define PTABLE_H

#include "bitmap.h"
#include "pcb.h"

#define MAX_PROCESS 10

class PTable {
   private:
    Bitmap* bm;

   public:
    Semaphore* bmsem;
    PCB* pcb[MAX_PROCESS];
    int psize;
    PTable(int size);
    ~PTable();
    int ExecUpdate(char* name,int prnum);
    int ExitUpdate(int ec);
    int JoinUpdate(int id);
    int GetFreeSlot();
    bool IsExist(int pid);
    void Remove(int pid);
    char* GetFileName(int id);
};

#endif
