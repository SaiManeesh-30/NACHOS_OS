#ifndef PCB_H
#define PCB_H

class PCB {
   private:
    Semaphore *joinsem;
    Semaphore *exitsem;
    Semaphore *multex;
    int exitcode;
    int numwait;

    char filename[128];

   public:
    Thread *thread;
    int status;
    int parentID;
    int processID;
    PCB();
    PCB(int id);
    ~PCB();

    int Exec(char *filename, int pid, int prnum);
    int GetID();
    int GetNumWait();
    int getStatus();

    void JoinWait();
    void ExitWait();
    void JoinRelease();
    void ExitRelease();

    void IncNumWait();
    void DecNumWait();

    void SetExitCode(int ec);
    int GetExitCode();

    void SetFileName(char *fn);
    char *GetFileName();
};

#endif
