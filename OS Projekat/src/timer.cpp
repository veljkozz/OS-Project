#include <DOS.H>
#include <STDIO.H>
#include <STDARG.H>
#include <iostream.h>
#include <stdlib.h>
#include "schedule.h"
#include "pcb.h"
#include "thread.h"
#include "List.h"
#include "Semaphor.h"
#include "KernSem.h"
#include "Event.h"
#include "IVTENTRY.h"
#include "KernelEv.h"
#include "declare.h"


unsigned tsp;
unsigned tss;
unsigned tbp;
volatile int brojac = 2;
volatile int zahtevana_promena_konteksta = 0;
volatile int dispatchRequest = 0;
List<KernelSem*>::Node* sempok = 0;
List<PCB*>::Node* pcbpok = 0;
List<PCB*>::Node* prevpcb = 0;
List<PCB*>::Node* old = 0;



typedef void interrupt(*pInterrupt)(...);

void tick();


void dispatch();
void signalHandling();
extern List<KernelSem*> SemaphoreList;
extern List<PCB*> globalPCBList;
volatile int lockFlag = 1;
volatile PCB* PCB::running = 0; 


void Idle(){
	while(1);
}

static PCB* idlePCB = new PCB(Idle);

int globalcnt = 0;
void interrupt timer(){	// prekidna rutina
	
	//prolazenje kroz listu semafora
	//pa kroz njihove liste blokiranih procesa
	if(!zahtevana_promena_konteksta){
		// poziv stare prekidne rutine koja se 
     		// nalazila na 08h, a sad je na 60h
		asm int 60h;
		tick();
		sempok = SemaphoreList.head;
		while(sempok){
			prevpcb = 0;
			pcbpok = sempok->data->BlockedList.head;
			while(pcbpok){
				if(pcbpok->data->timeBlocked > 0) {
					pcbpok->data->timeBlocked--;
					if(pcbpok->data->timeBlocked == 0) {
						PCB* proc = pcbpok->data;
						if(prevpcb==0){
							old = pcbpok;
							pcbpok=pcbpok->next;
							sempok->data->BlockedList.head = pcbpok;
							delete old;
						}else{
							prevpcb->next = pcbpok->next;
							old = pcbpok;
							pcbpok=pcbpok->next;
							delete old;
							
						}
						if(sempok->data->BlockedList.head == 0) sempok->data->BlockedList.tail = 0;
						else if(pcbpok==0) sempok->data->BlockedList.tail = prevpcb;
						sempok->data->val++;
						proc->status = READY;
						Scheduler::put(proc);
					}else{
						prevpcb = pcbpok;
						pcbpok=pcbpok->next;
					}
				}else{
					prevpcb = pcbpok;
					pcbpok=pcbpok->next;
				}
			}
			sempok=sempok->next;
		}
	}
	if (!zahtevana_promena_konteksta && brojac > 0) brojac--; 

	if (brojac == 0 || zahtevana_promena_konteksta) {
		if(lockFlag>0 || dispatchRequest > 0){
			zahtevana_promena_konteksta = 0;
			dispatchRequest = 0;
			if(PCB::running->toBeKilled == 1){
				delete[] PCB::running->stack;
 				PCB::running->stack=0;
 				PCB::running->toBeKilled = 0;
			}

			asm {
				// cuva sp
				mov tsp, sp
				mov tss, ss
				mov tbp, bp
			}

			PCB::running->sp = tsp;
			PCB::running->ss = tss;
			PCB::running->bp = tbp;

			if(PCB::running->status == READY) Scheduler::put((PCB*)PCB::running);
			PCB::running = Scheduler::get();
			if(PCB::running == 0)
				PCB::running = idlePCB;
			
			tsp = PCB::running->sp;
			tss = PCB::running->ss; 
			tbp = PCB::running->bp;
			brojac = PCB::running->timeSlice;
			//lockFlag = PCB::running->myLock;

			asm {
				mov sp, tsp 
				mov ss, tss
				mov bp, tbp
			} 

			signalHandling();    
		}else{
			//zahtevana_promena_konteksta = 1;
		}
	} 	                                              
}

void dispatch(){ // sinhrona promena konteksta 
	lock
	zahtevana_promena_konteksta = 1;
	dispatchRequest = 1;
	//timer();
	asm int 8h;
	unlock
}

SignalId signal;
void signalHandling(){
	
	softLock
	while(!PCB::running->signalRequests.isEmpty()){
		signal = PCB::running->signalRequests.pop_front();
		if(!PCB::globalSignalBlock[signal] && !PCB::running->SignalBlock[signal] ){
			if(signal==0) {
				//nasilno prekida nit
				lock                
    				pcbpok = PCB::running->waitingList.head;
				while(pcbpok){
					pcbpok->data->status = READY;
					Scheduler::put(pcbpok->data);
					pcbpok=pcbpok->next;
				}
				PCB::running->status = FINISHED;
				if(PCB::running->myCreator != 0) PCB::running->myCreator->signal(1); 
    			for(int i=0;i<16;++i) PCB::running->HandlerLists[i].empty();
    			PCB::running->signalRequests.empty();
    			if(PCB::running->stack != 0 ) PCB::running->toBeKilled = 1;
				unlock
				softUnlock
				dispatch();
			}else{
				asm { pushf; sti; }
					for(int k = 0; k < PCB::running->HandlerLists[signal].size(); ++k)
						PCB::running->HandlerLists[signal].get(k)();
				asm popf;
			}
		}
	}

	softUnlock
	
}  

