// threadtest.cc 
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "synch.h"


// testnum is set in main.cc
int testnum = 1;


//must initiate semaphore objects here?
int numThreadsActive; // used to implement barrier upon completion

#if defined(HW1_SEMAPHORES)
Semaphore* mutex = new Semaphore("Semaphore", 1);
#endif

#if defined(HW1_LOCKS)
Lock* lock = new Lock("Lock");
#endif


//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
//----------------------------------------------------------------------


//----------------------------------------------------------------------
// ThreadTest1
// 	Set up a ping-pong between two threads, by forking a thread 
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// ThreadTest
// 	Invoke a test routine.
//----------------------------------------------------------------------

// Modified version of ThreadTest that takes an integer n
// and creates n new threads, each calling SimpleThread and
// passing on their ID as argument.

#if defined(CHANGED)

int SharedVariable;
void SimpleThread(int which) {

int num, val;
for(num = 0; num < 5; num++) {

    //Entry Section -- Invoke P
    

#if defined(HW1_SEMAPHORES)
    mutex->P();   
#endif

#if defined(HW1_LOCKS)
    lock->Acquire();
#endif
 
val = SharedVariable;
printf("*** thread %d sees value %d\n", which, val); 
currentThread->Yield();
SharedVariable = val+1;
currentThread->Yield();

    //Exit Section -- Invoke V

#if defined(HW1_SEMAPHORES)
   mutex->V();
#endif

#if defined(HW1_LOCKS)
   lock->Release();
#endif


}

//decrement numThreadsActive

   numThreadsActive--;

//check if numThreadsActive is 0 or not, and if it is not, Yield to a diff thread
//while numThreadsActive > 0, keep Yielding, when numThreadsActive = 0, stop yielding
//all threads should see same final value

while(numThreadsActive > 0){
    currentThread->Yield();
}


val = SharedVariable;
printf("Thread %d sees final value %d\n", which, val);
}

void
ThreadTest(int n) {


    DEBUG('t', "Entering SimpleTest");
    Thread *t;
    numThreadsActive = n;
    printf("NumthreadsActive = %d\n", numThreadsActive);

    for(int i=1; i<n; i++)
    {
        t = new Thread("forked thread");
        t->Fork(SimpleThread,i);
    }
    SimpleThread(0);
}

#else


void
SimpleThread(int which)
{
    int num;
    
    for (num = 0; num < 5; num++) {
	printf("*** thread %d looped %d times\n", which, num);
        currentThread->Yield();
    }
}

#endif

void
ThreadTest1()
{
    DEBUG('t', "Entering ThreadTest1");

    Thread *t = new Thread("forked thread");

    t->Fork(SimpleThread, 1);
    SimpleThread(0);
}

void
ThreadTest()
{
    switch (testnum) {
    case 1:
	ThreadTest1();
	break;
    default:
	printf("No test specified.\n");
	break;
    }
}
