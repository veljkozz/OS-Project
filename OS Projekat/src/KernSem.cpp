#include "KernSem.h"
#include "Semaphor.h"
#include "List.h"
#include "PCB.h"
#include "Schedule.h"
#include "declare.h"
volatile List<KernelSem*> SemaphoreList;

KernelSem::KernelSem(int init){
    softLock
    val = init;
    SemaphoreList.add(this);
    softUnlock
}

// n==0 radi kao sa prezentacija signal
// n>0 odblokiraj do n procesa koje je blokirano, kontam val+=n;
// OVDE STAO
int KernelSem::signal(int n){
    softLock
    int retval=0;
    if(n==0){
        val++;
        if(val<=0){
            PCB* proc = BlockedList.take();
            proc->status = READY;
            Scheduler::put(proc);
        }
    }else if (n>0) 
    {
        val+=n;
        PCB* t = BlockedList.take();
        while(t!=0 && retval < n){
            retval++;
            t->status = READY;
            Scheduler::put(t);
        }
        
    }else
    {
        retval = n;
    }

    softUnlock

    return retval;
}

// val-- ako je val  < 0 blokiraj jednu
// sa max vremenom cekanja maxTimeToWait, osim ako ==0
// PCB->timeBlocked = -1 kada nije deblokiran zbog isteka vremena
int KernelSem::wait(Time maxTimeToWait){
    softLock
    val--;
    if(val < 0){
        PCB::running->status = BLOCKED;
        if(maxTimeToWait==0) PCB::running->timeBlocked = -1;
        else PCB::running->timeBlocked = maxTimeToWait;
        BlockedList.add((PCB*)PCB::running);
        dispatch();
    } else {
        softUnlock
        return 1;
    }
    softUnlock
    return (PCB::running->timeBlocked == 0) ? 0 : 1; 
}

