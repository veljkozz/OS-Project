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
            //printf("Ja ga blokiro\n");
            PCB::running->status = BLOCKED;
            dispatch();
            //printf("Ja ga odblokiro\n");
        }
    }
    unlock
}

void KernelEv::signal(){
    lock
    //printf("Evo me u signal od KernelEv\n");
    if(val==0){
        //while(1);
        myPCB->status = READY;
        Scheduler::put(myPCB);
        val = 1;
        //dispatch();
        //PCB::running = Scheduler::get();
    }
    if(lockFlag) dispatch();
    unlock
}
