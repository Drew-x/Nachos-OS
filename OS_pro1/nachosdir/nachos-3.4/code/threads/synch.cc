// synch.cc
//	Routines for synchronizing threads.  Three kinds of
//	synchronization routines are defined here: semaphores, locks
//   	and condition variables (the implementation of the last two
//	are left to the reader).
//
// Any implementation of a synchronization routine needs some
// primitive atomic operation.  We assume Nachos is running on
// a uniprocessor, and thus atomicity can be provided by
// turning off interrupts.  While interrupts are disabled, no
// context switch can occur, and thus the current thread is guaranteed
// to hold the CPU throughout, until interrupts are reenabled.
//
// Because some of these routines might be called with interrupts
// already disabled (Semaphore::V for one), instead of turning
// on interrupts at the end of the atomic operation, we always simply
// re-set the interrupt state back to its original value (whether
// that be disabled or enabled).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synch.h"
#include "system.h"

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	Initialize a semaphore, so that it can be used for synchronization.
//
//	"debugName" is an arbitrary name, useful for debugging.
//	"initialValue" is the initial value of the semaphore.
//----------------------------------------------------------------------

Semaphore::Semaphore(const char* debugName, int initialValue)
{
    name = debugName;
    value = initialValue;
    queue = new List;
}

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	De-allocate semaphore, when no longer needed.  Assume no one
//	is still waiting on the semaphore!
//----------------------------------------------------------------------

Semaphore::~Semaphore()
{
    delete queue;
}

//----------------------------------------------------------------------
// Semaphore::P
// 	Wait until semaphore value > 0, then decrement.  Checking the
//	value and decrementing must be done atomically, so we
//	need to disable interrupts before checking the value.
//
//	Note that Thread::Sleep assumes that interrupts are disabled
//	when it is called.
//----------------------------------------------------------------------

void
Semaphore::P()
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts

    while (value == 0) { 			// semaphore not available
	queue->Append((void *)currentThread);	// so go to sleep
	currentThread->Sleep();
    }
    value--; 					// semaphore available,
						// consume its value

    (void) interrupt->SetLevel(oldLevel);	// re-enable interrupts
}

//----------------------------------------------------------------------
// Semaphore::V
// 	Increment semaphore value, waking up a waiter if necessary.
//	As with P(), this operation must be atomic, so we need to disable
//	interrupts.  Scheduler::ReadyToRun() assumes that threads
//	are disabled when it is called.
//----------------------------------------------------------------------

void
Semaphore::V()
{
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    thread = (Thread *)queue->Remove();
    if (thread != NULL)	   // make thread ready, consuming the V immediately
	scheduler->ReadyToRun(thread);
    value++;
    (void) interrupt->SetLevel(oldLevel);
}

// Dummy functions -- so we can compile our later assignments
// Note -- without a correct implementation of Condition::Wait(),
// the test case in the network assignment won't work!

#if defined(HW1_LOCKS) || defined(HW1_ELEVATOR) || defined(HW1_CONDITION)

Lock::Lock(const char* debugName) {
    name = debugName;
    lockHolder = NULL;
    free = false;  // Indicates if the lock is currently held
    queue = new List;  // List of threads waiting for the lock
}

// Lock Destructor
Lock::~Lock() {
    delete queue;
}

// Acquire the Lock
void Lock::Acquire() {
    IntStatus oldLevel = interrupt->SetLevel(IntOff);  // Disable interrupts

    while (free) {  // If the lock is held, wait
        queue->Append((void *)currentThread);  // Add current thread to the waiting list
        currentThread->Sleep();  // Put the thread to sleep until the lock is free
    }

    free = true;  // Acquire the lock
    lockHolder = currentThread;  // Set current thread as the lock holder

    (void) interrupt->SetLevel(oldLevel);  // Restore the interrupt level
}

// Release the Lock
void Lock::Release() {
    ASSERT(isHeldByCurrentThread());  // Ensure current thread holds the lock

    IntStatus oldLevel = interrupt->SetLevel(IntOff);  // Disable interrupts

    free = false;  // Release the lock
    lockHolder = NULL;  // Clear the lock holder

    if (!queue->IsEmpty()) {  // If there are waiting threads
        Thread* nextThread = (Thread*)queue->Remove();  // Get next waiting thread
        scheduler->ReadyToRun(nextThread);  // Move the thread to the ready state
    }

    (void) interrupt->SetLevel(oldLevel);  // Restore the interrupt level
}

// Check if the Current Thread Holds the Lock
bool Lock::isHeldByCurrentThread() {
    return currentThread == lockHolder;
}

#else
// Dummy functions -- so we can compile our later assignments 
// Note -- without a correct implementation of Condition::Wait(), 
// the test case in the network assignment won't work!
Lock::Lock(const char* debugName) {}
Lock::~Lock() {}
void Lock::Acquire() {}
void Lock::Release() {}
#endif

#if defined(HW1_CONDITION) || defined(HW1_ELEVATOR) || defined(HW1_CONDITION)
Condition::Condition(const char* debugName) {
    name = debugName; // init
    queue =  new List;
}
Condition::~Condition() {
    delete queue;
}

void Condition::Wait(Lock* conditionLock) {

      // disable interrupts
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    // check if calling thread holds the lock
    ASSERT(conditionLock->isHeldByCurrentThread());

    // Release the lock
    conditionLock->Release();

    // put self in the queue of waiting threads
    queue->Append((void *)currentThread);

    currentThread->Sleep();

    // Re-acquire the lock
    conditionLock->Acquire();

     // enable interrupts
    (void) interrupt->SetLevel(oldLevel);

}
void Condition::Signal(Lock* conditionLock) {

      // disable interrupts
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    // check if calling thread holds the lock
    ASSERT(conditionLock->isHeldByCurrentThread());

    // Dequeue one of the threads in the queue
    Thread* thread = (Thread*)queue->Remove();

    // If thread exists, wake it up.
        if(thread != NULL){

            scheduler->ReadyToRun(thread);
        }



 // enable interrupts
    (void) interrupt->SetLevel(oldLevel);
}
void Condition::Broadcast(Lock* conditionLock) {

      // disable interrupts
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    // check if calling thread holds the lock
    ASSERT(conditionLock->isHeldByCurrentThread());

    // Dequeue all threads in the queue one-by-one
    // Wakeup each thread

     while (!queue->IsEmpty()) {
        Thread* thread = (Thread*)queue->Remove(); 

        if(thread != NULL){

        scheduler->ReadyToRun(thread);   

        }
    }

     // enable interrupts
    (void) interrupt->SetLevel(oldLevel);

 }
#else
Condition::Condition(const char* debugName) { }
Condition::~Condition() { }
void Condition::Wait(Lock* conditionLock) { ASSERT(FALSE); }
void Condition::Signal(Lock* conditionLock) { }
void Condition::Broadcast(Lock* conditionLock) { }
#endif
