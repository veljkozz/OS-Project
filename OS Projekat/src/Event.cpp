#include "Event.h"
#include "KernelEv.h"
#include "declare.h"

//DODATI MAKRO PREPAREENTRY
Event::Event (IVTNo ivtNo){
    lock
    myImpl = new KernelEv(ivtNo);
    unlock
}

 void Event::wait (){
     lock
     myImpl->wait();
     unlock
 }

 void Event::signal(){
     lock
     myImpl->signal();
     unlock
 }

 Event::~Event (){
     lock
     signal();
     delete myImpl;
     unlock
 }