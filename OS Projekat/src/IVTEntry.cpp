#include "IVTEntry.h"
#include <DOS.H>
#include "KernelEv.h"
#include "declare.h"

IVTEntry* IVTEntry::IVTEntries[256] = {0};

IVTEntry::IVTEntry(IVTNo numEntry, pInterrupt newRoutine){
    lock
    num = numEntry;
    oldRoutine = getvect(numEntry);
    this->newRoutine = newRoutine;
    setvect(numEntry, newRoutine);
    IVTEntry::IVTEntries[numEntry] = this;
    unlock
}

void IVTEntry::signal(){
    lock
    //cout << "Signal iz IVTEntry\n";
    myEv->signal();
    unlock
}

void IVTEntry::Old(){
    lock
    if(oldRoutine != 0) oldRoutine();
    unlock
}

IVTEntry::~IVTEntry(){
    lock
    setvect(num, oldRoutine);
    if(oldRoutine != 0) oldRoutine();
    IVTEntry::IVTEntries[num] = 0;
    unlock
}

IVTEntry* IVTEntry::getThisEntry(IVTNo n){
    return IVTEntry::IVTEntries[n];
}