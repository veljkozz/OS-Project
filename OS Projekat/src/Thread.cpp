#include "thread.h"
#include "PCB.h"
#include "List.h"
#include "SCHEDULE.H"
#include <stdio.h>
#include "declare.h"

class PCB;
extern List<PCB*> globalPCBList;

Thread::Thread (StackSize stackSize, Time timeSlice){
    lock
    myPCB = new PCB(this, stackSize, timeSlice);
    globalPCBList.add(myPCB);
    unlock
}
void Thread::start(){
    softLock
    myPCB->status = READY;
    Scheduler::put(this->myPCB);
    softUnlock
}


void Thread::waitToComplete(){
    lock
    if(myPCB->status == FINISHED || myPCB == PCB::running) {
        unlock
        return;
    }
    myPCB->waitingList.add((PCB*)PCB::running);
    PCB::running->status = BLOCKED;
    dispatch();
    unlock
}

Thread::~Thread(){
    lock
    //cout << "Destruktor od " << getId() << endl;
    globalPCBList.remove(this->myPCB);
    delete this->myPCB;
    unlock
}

ID Thread::getId(){
    return myPCB->id;
}

ID Thread::getRunningId(){
    return PCB::running->id;
}

Thread * Thread::getThreadById(ID id){
    softLock
    List<PCB*>::Node* t = globalPCBList.head;
    while(t){
        if(t->data->id == id) {
            softUnlock
            return t->data->myThread;
        }
        t=t->next;
    }
    softUnlock
    return 0;
}

//stvari za signale
void Thread::signal(SignalId signal){
    myPCB->signal(signal);
}

void Thread::registerHandler(SignalId signal, SignalHandler handler){
    myPCB->registerHandler(signal, handler);
}

void Thread::unregisterAllHandlers(SignalId signal){
    myPCB->unregisterAllHandlers(signal);
}


void Thread::swap(SignalId id, SignalHandler hand1, SignalHandler hand2){
    myPCB->swap(id, hand1, hand2);
}

void Thread::blockSignal(SignalId signal){
    myPCB->blockSignal(signal);
}
void Thread::blockSignalGlobally(SignalId signal){
    PCB::blockSignalGlobally(signal);
}
void Thread::unblockSignal(SignalId signal){
    myPCB->unblockSignal(signal);
}
void unblockSignalGlobally(SignalId signal){
    PCB::unblockSignalGlobally(signal);
}
