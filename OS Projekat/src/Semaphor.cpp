#include "Semaphor.h"
#include "KernSem.h"
#include "List.h"
#include "declare.h"
extern List<KernelSem*> SemaphoreList;

int Semaphore::val() const{ return myImpl->val; }

Semaphore::Semaphore(int init){
    lock
    myImpl = new KernelSem(init);
    unlock
}

int Semaphore::wait (Time maxTimeToWait){
    return myImpl->wait(maxTimeToWait);
}

int Semaphore::signal(int n){
    return myImpl->signal(n);
}

Semaphore::~Semaphore (){
    lock
    SemaphoreList.remove(myImpl);
    delete myImpl;
    unlock
}