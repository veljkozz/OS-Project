#include "PCB.h"
#include <dos.h>
#include "SCHEDULE.H"
#include "declare.h"
#include "stdio.h"

int PCB::idCnt = 1;
void dispatch();
volatile List<PCB*> globalPCBList;
extern unsigned tsp, tss, tbp;

int PCB::globalSignalBlock[16] = {0};

PCB::PCB(void (*body)(), Time timeSlice){
    lock
    unsigned* st1 = new unsigned[1024];

	st1[1023] =0x200;//setovan I fleg u
                      // pocetnom PSW-u za nit
	st1[1022] = FP_SEG(body);
	st1[1021] = FP_OFF(body);

	sp = FP_OFF(st1+1012); 
	ss = FP_SEG(st1+1012);
	bp = sp;
	//status = READY;
    id = idCnt++;
    this->timeSlice = timeSlice; 
    for(int i=0;i<16;++i) this->SignalBlock[i] = PCB::running->SignalBlock[i];
    toBeKilled=0;
    myLock = 1;
    unlock
}
PCB::PCB(){
    lock
    /*asm {
				// cuva sp
				mov tsp, sp
				mov tss, ss
				mov tbp, bp
		}
    this->sp = tsp;
    this->ss = tss;
    this->bp = tbp;*/
    for(int i=0;i<16;++i) this->SignalBlock[i] = 0;
    toBeKilled=0;
    myLock = 1;
    unlock
 }

PCB::PCB(Thread* myThread, StackSize stackSize, Time timeSlice){
    lock
    
    stackSize/=2;
    if(stackSize > STACK_MAX_SIZE) stackSize = (unsigned long)STACK_MAX_SIZE;
	stack = new unsigned[stackSize];

    stack[stackSize-1] = FP_SEG(myThread);
    stack[stackSize - 2] = FP_OFF(myThread);
	stack[stackSize-5] =0x200; //setovan I fleg            
	stack[stackSize-6] = FP_SEG(PCB::wrapper);
	stack[stackSize-7] = FP_OFF(PCB::wrapper);

    sp = FP_OFF(stack+stackSize-16);
	ss = FP_SEG(stack+stackSize-16);
    bp = sp;

	id = idCnt++;

    if(timeSlice > 0) {
        this->timeSlice = timeSlice;
    }
    else this->timeSlice = -1;  //ako je timeSlice == 0 onda treba da bude vremenski neograniceno pa stavio -1 jer manje od nule (pogledati timer)
    this->myThread = myThread;
    
    //dodatak za signale, kreirana nit nasledjuje osobine/podesavanja od niti koja je kreira
    //this->SignalBlock = new int[16];
    for(int i=0;i<16;++i) this->SignalBlock[i] = PCB::running->SignalBlock[i];
    //for(i=0;i<16;++i) cout << SignalBlock[i] << " ";
    myCreator = (PCB*)PCB::running;
    toBeKilled=0;
    myLock = 1;
    unlock
}
List<PCB*>::Node* t = 0;
void PCB::wrapper(Thread* myThread){
    PCB::running->myThread->run();
    softLock                        //ovde npr sam ubedjen treba hardlock
    
    if(PCB::running->myCreator != 0) PCB::running->myCreator->signal(1);     //signaliziraj kreatoru da si zavrsio
    PCB::running->signal(2);    //signaliziraj samom sebi da si zavrsio i guess
    t = PCB::running->waitingList.head;
    while(t){
        t->data->status = READY;
        Scheduler::put(t->data);
        t=t->next;
    }
    PCB::running->status = FINISHED;
    softUnlock
    dispatch();
}

PCB::~PCB(){
    lock
    //signal(2);
    status = FINISHED;
    //for(int i=0;i<16;++i) HandlerLists[i].empty();
    //signalRequests.empty();
    if(stack!=0) delete []stack;
   // if(PCB::running == this && stack != 0) toBeKilled = 1;
    unlock
}

void signalHandling();
void PCB::signal(SignalId signal) volatile {
    softLock
    signalRequests.add(signal);
    if(PCB::running == this) signalHandling();
    softUnlock
}

void PCB::registerHandler(SignalId signal, SignalHandler handler){
    //lock
    HandlerLists[signal].add(handler);
    //unlock
}

void PCB::swap(SignalId id, SignalHandler hand1, SignalHandler hand2){
    //lock
    HandlerLists[id].swap(hand1, hand2);
    //unlock
}

void PCB::blockSignal(SignalId signal){
    lock
    SignalBlock[signal] = 1;
    unlock
}

void PCB::blockSignalGlobally(SignalId signal){
    lock
    PCB::globalSignalBlock[signal] = 1;
    unlock
}

void PCB::unblockSignal(SignalId signal){
    lock
    SignalBlock[signal] = 0;
    unlock
}

void PCB::unblockSignalGlobally(SignalId signal){
    lock
    PCB::globalSignalBlock[signal] = 0;
    unlock
}

void PCB::unregisterAllHandlers(SignalId signal){
    //lock
    HandlerLists[signal].empty();
    //unlock
}