#ifndef _KERNELEV_
#define _KERNELEV_
#include "Event.h"
#include "IVTEntry.h"
#include "PCB.h"
class Event;
//typedef unsigned char IVTNo;

class IVTEntry;

class KernelEv{
public:
    int val;
    KernelEv(IVTNo ivtNo);
    PCB* myPCB;
    IVTEntry* myIVT;
    void wait ();
    void signal();

   //Event* event;
};



#endif