#ifndef _KERNELSEM_
#define _KERNELSEM_
#include "Semaphor.h"
#include "List.h"
#include "PCB.h"
class PCB;

class KernelSem{
public:
    int val;
    
    List<PCB*> BlockedList;
    
    virtual int wait (Time maxTimeToWait);
    virtual int signal(int n);
    
    KernelSem(int);
};


#endif