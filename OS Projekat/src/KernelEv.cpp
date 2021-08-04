#include "KernelEv.h"
#include "Event.h"
#include "IVTEntry.h"
#include "Schedule.h"
#include "PCB.h"
#include "declare.h"

class IVTEntry;


KernelEv::KernelEv(IVTNo ivtNo){
    lock
    myPCB = (PCB*)PCB::running;
    val = 1;
    myIVT = IVTEntry::IVTEntries[ivtNo];
    myIVT->myEv = this;
    unlock
}
void KernelEv::wait (){
    lock
    if((PCB*)PCB::running == myPCB){
        if(val){
            val = 0;
            PCB::running->status = BLOCKED;
            dispatch();
        }
    }
    unlock
}

void KernelEv::signal(){
    lock
    if(val==0){
        myPCB->status = READY;
        Scheduler::put(myPCB);
        val = 1;
    }
    if(lockFlag) dispatch();
    unlock
}
