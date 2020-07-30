#ifndef _PCB_
#define _PCB_
#include "thread.h"
#include "List.h"
#define STACK_MAX_SIZE 32768
#define MIN_STACK_SIZE 512
enum Status {BLOCKED= -1 , SLEEPING = 0 , READY = 1, FINISHED = 2};
class PCB{
public:
    unsigned* stack;
	unsigned sp;
	unsigned ss;
	unsigned bp;
	Status status;
    int id;
	int timeSlice;
    unsigned int timeBlocked;
    int toBeKilled;
    int myLock;
    Thread* myThread;
    List<PCB*> waitingList;

    static int idCnt;
    volatile static PCB* running;
    PCB* myCreator;
    
    ~PCB();

    PCB::PCB(void (*body)(), Time TimeSlice = defaultTimeSlice);
    PCB();
    PCB(Thread* myThread, unsigned long stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice);

public:
    // ONO STO SAM JA DODAO ZA SIGNALE
    List<SignalHandler> HandlerLists[16];
    List<SignalId> signalRequests;

    static int globalSignalBlock[16];
    int SignalBlock[16];

    //SIGNALI STVARI
    void signal(SignalId signal) volatile;

    void registerHandler(SignalId signal, SignalHandler handler);
    void unregisterAllHandlers(SignalId signal);
    void swap(SignalId id, SignalHandler hand1, SignalHandler hand2);

    void blockSignal(SignalId signal);
    static void blockSignalGlobally(SignalId signal);
    void unblockSignal(SignalId signal);
    static void unblockSignalGlobally(SignalId signal);
    
protected:
    friend class Thread;
    static void wrapper(Thread*);
};










#endif