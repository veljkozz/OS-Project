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

void interrupt timer();
static PCB* initial = 0;


unsigned oldTimerOFF, oldTimerSEG; // stara prekidna rutina

// postavlja novu prekidnu rutinu
void inic(){
	asm{
		cli
		push es
		push ax

		mov ax,0   //  ; inicijalizuje rutinu za tajmer
		mov es,ax

		mov ax, word ptr es:0022h //; pamti staru rutinu
		mov word ptr oldTimerSEG, ax	
		mov ax, word ptr es:0020h	
		mov word ptr oldTimerOFF, ax	

		mov word ptr es:0022h, seg timer	 //postavlja 
		mov word ptr es:0020h, offset timer //novu rutinu

		mov ax, oldTimerSEG	 //	postavlja staru rutinu	
		mov word ptr es:0182h, ax //; na int 60h
		mov ax, oldTimerOFF
		mov word ptr es:0180h, ax

		pop ax
		pop es
		sti
	}
	
	initial = new PCB();
	initial->timeSlice = 2;
	initial->id = 0;
	initial->status = READY;
	initial->toBeKilled = 0;
	brojac = initial->timeSlice;
	PCB::running = initial;
}

// vraca staru prekidnu rutinu
void restore(){
	asm {
		cli
		push es
		push ax

		mov ax,0
		mov es,ax


		mov ax, word ptr oldTimerSEG
		mov word ptr es:0022h, ax
		mov ax, word ptr oldTimerOFF
		mov word ptr es:0020h, ax

		pop ax
		pop es
		sti
	}
}

int syncPrintf(const char *format, ...)
{
	int res;
	va_list args;
	lock
		va_start(args, format);
	res = vprintf(format, args);
	va_end(args);
	unlock
		return res;
}

int userMain (int argc, char* argv[]);

class userThread : public Thread {
public:
	userThread(int argc, char** argv) : argc(argc), argv(argv){}
	~userThread()
	{
		waitToComplete();
	}
	int argc;
	char** argv;
protected:
	void run(){
		userMain(argc,argv);
	}
};

int main(int argc, char* argv[]){

	lock
	inic();
	unlock
	
	userThread ut(argc,argv);
	ut.start();
	ut.waitToComplete();
	
	lock
	restore();
	unlock
	return 0;
}

