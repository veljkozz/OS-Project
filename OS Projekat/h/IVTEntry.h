#ifndef _IVTENTRY_
#define _IVTENTRY_
#include "KernelEv.h"

typedef void interrupt(*pInterrupt)(...);

class KernelEv; 
class IVTEntry;
#define PREPAREENTRY(numEntry, callOld)\
void interrupt inter##numEntry(...); \
IVTEntry newEntry##numEntry(numEntry, inter##numEntry); \
void interrupt inter##numEntry(...){\
newEntry##numEntry.signal();\
if (callOld == 1) newEntry##numEntry.Old();\
}
//moze gore dispatch
class IVTEntry{
public:
    IVTNo num;
    pInterrupt oldRoutine;
    pInterrupt newRoutine;
    IVTEntry(){}
    IVTEntry(IVTNo numEntry, pInterrupt);
    ~IVTEntry();
    
    KernelEv* myEv;

    void signal();
    void Old();
    
    static IVTEntry* IVTEntries[256];
    static IVTEntry* getThisEntry(IVTNo n);
};






#endif