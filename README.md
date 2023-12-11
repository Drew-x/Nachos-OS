# Nachos-OS


I have reviewed the contents of the text files for each project. Now, I will format these into separate sections for Project 1, Project 2, and Project 3 to create a well-structured README file for your GitHub repository.

#### Project 1: 

# 1. SimpleThread Modified Test Report

The original SimpleThread function provided in the startup code without the addition of semaphores or locks for synchronization was called 
by threadtest in order to create 2 threads. In this case, there was no critical section or updating of shared variables. Instead, threads 0
and 1 simply looped 5 times, in random order. After this, the threadTest function was modified to take n values as input, which would create
n threads and loop them 5 times. We tested this with values 0 through 4, and saw that the threads executed and looped in random order.   

Once the new modified version of SimpleThread was added to the threadtest class, we were able to test with and without random interrupts, using
input values between 0 and 4. Without the random interrupts, threads appear to be synchronized and obtain the same final value. However, once
random interrupts are enabled, the sharedVariable value begins to differ between the various thread executions. This is simply because there are
no checks or synchronization primitives being used to organize the modification of the sharedVariable.

# 2. Experience and Completion of Exercises

I was able to complete all of the 4 exercises in Project 1, ensuring to test each portion of code using the test files provided in Canvas. 
	
	a. Semaphore Test
	
	The Semaphore test was the simplest to complete due to the fact that the Semaphore functions P() and V() were already implemmented
	in the starter code. The most difficult part was identifying the entry and exit sections surrounding the critical section within
	the SimpleThread() function and setting up the barrier that keeps track of the number of threads active. Once the barrier was set
	and the sections were defined, all that was left was to create a Semaphore object within the threadtest class and call the P()
	and V() functions in the entry and exit sections, respectively. After running with both the threadtest(int testnum) function and
	the Semaphore_Ping() test function provided, it seemed that the threads were synchronized as expected.

	b. Lock Test
	
	The Lock test was more difficult compared to the Semaphore Test because the relevant functions Acquire() and Release() were not 
	implemmented beforehand. However, following the implemmentation of the Semaphore functions, it was not too difficult to understand 
	and replicate the pattern for the Locks. The most complicated part was figuring out the logic for the isHeldByCurrentThread() function
	and how to use it within the Release() function. However, once these functions were defined, we were able to simply create a Lock instance
	in threadtest and invoke lock->Acquire() and lock->Release() in the entry and exit sections, respectively. We encountered some issues
	when attmepting to run the Lock synchronization code using the threadtest(int testnum) call in main, but it was working as expected when 
	calling the LockTest() function.

	c. Condition Test
	
	The condition test was similar in difficulty to the lock test, as it also required us to implement the relevant functions. In this case,
	we had to implemment Broadcast(), Wait(), and Signal(). The most challenging part was the Broadcast() function, as it was difficult to 
	find a way to loop through and dequeue all of the threads one by one. Once that was done, we had trouble understanding how the condition
	variables should be used in threadtest to synchronize the update of the sharedVariable. Once we realized that we had to use the Ping() test
	to run the Condition variable implemmentation, it was wasier to debug and finish this part of the project.

	d. Elevator Test
 
	The elevator test was the most difficult part of the project. After watching the professor's videos, we had a good understanding of the 
	background and requirements of the problem and began implemmenting the start() and hailElevator() functions. At first, we did not understand
	how to hail the elevator from within hailElevator, but we eventually realized that this would mean signaling the Condition variable tied to the
	elevator thread, as it had been set to wait to be hailed in the beginning of the start function. Additionally, we were confused on how to 
	move the elevator up and down floors, as we assumed we would need to go to specific floors based on the toFloor attribute defined in the Person
	struct. After meeting with the professor, he informed us that the elevator should only go up and down one floor at a time, regardles of the 
	toFloor attribute of the passengers. He also helped us debug the code, as we were obtaining segmentation faults due to the fact that the elevator
	was initialized using currentFloor = 1 instead of currentFloor = 0. After this, we were able to properly compile and run the ElevatorTest() function
	from main. 


#### Project 2: _(Title from Project 2 File)_

# 1. Code Design

The code is designed to process and execute all of the necessary system calls within the exception.cc
file. Each process, upon creation, is assigned a new pcb and a new address space. If the process is the
first to execute from main or is performing an exec, the address space constructor that takes an executable
file as input is utilized. Otherwise, if the process is being created via a fork system call, the address
space constructor that takes in the parent address space and returns a copy of it is used. Each address space 
constructor is responsible for loading in the appropriate sections (code and initialized data) from either
the executable or the existing parent address space. It must also account for the stack and uninitialized
data (heap) segments. A key component of the address space creation is the use of the memory manager class,
which uses a bitmap to allocate and deallocate pages of physical memory to be used during execution. This 
allocation allows the address space constructors to also perform translation from virtual pages to physical 
page frames. 

Each process is represented by its pcb. More specifically, by the pid of its pcb.
Each pcb keeps track of the thread that owns it, the parent pcb, its pid, its exit status, and a list of
child threads, if applicable. The pcb's are managed by the pcbManager class. The pcbManager's primary job
is to allocate and deallocate pcb's as needed throughout the program. Allocating a pcb involves finding the
first available bit in the bitmap and assigning the pcb's pid to this value as well as creating a new pcb and
storing it in the pcbs array. A design choice we made was to begin the pid assignments at 1 rather than 0 to better
match the expected outputs. Conversely, deallocating the pcb involves clearing the bit in the bitmap associated
with the pid of the process whose pcb is being deleted as well as deleting that pcb from the pcbs array.


# 2. How the Code Works

The program begins in main.cc within the threads directory. From main, the StartProcess function within progtest.cc
is called. StartProcess is responsible for loading in the test file or executable and correctly setting up the environment.
It creates a new pcb and address space for the starting process using the test executable. It also sets up the initial 
register values, loads the page table registers, and begins running the stream of instructions pointed to by the 
program counter, which is achieved by 'machine->Run()'. Then, it begins executing the various test files, which invoke the
system calls. Each system call works as follows:

	a. Exit

	When a process invokes exit, it first makes a call to DeleteExitedChildrenSetParentNull. This function deallocates the pcb's
	of any children threads that have already exited and sets the parent pcb to null for any children process that has not
	exited yet. After this, there is a check to see if pcb->paret is null, indicating that the process exiting is a child
	whose parent had already exited. In this case, the process deallocates its own pcb using the pcbManager. Once the pcb's are deallocated, 
	the address space of the current process is deleted and currentThread calls Finish().

	b. Fork

	When a process invokes fork, the first call that is made is to the pcbManager to allocate a new pcb for the incoming child
	process. The pcb must be allocated before anything else is done so that the child's pid can be used throught the fork function.
	Once it is allocated, the user state of the current thread is saved and an address space is created for the child as an
	exact copy of the parent address space. Then, a thread is created for the child and the new address space is assigned to it.
	Additionally, the parent, child thread, and address space are all assigned to the child pcb and the new pcb is added to the 
	current thread's list of children. At this point, the machind registers PCReg, PrevPCReg, and NextPCReg must be updated to point
	to the new function address that will be executed by the child thread. Then, the user state of the child thread is saved and fork
	is called, passing in the address of the childFunction as a parameter. The pid of the new child process is returned.

	c. Exec

	When exec is invoked, the first step is to open the executable file. Once this is accomplished, the existing address space of the
	current thread must be deleted and a new one must be created using the new executable. Then, the new address space's pcb must be set
	to the current thread's pcb and the current thread must be assigned its new address space. Then, the initial register values must be
	set and the page table must be loaded. Finally, machine->Run() is called to jump into the user program.

	d. Join

	When a process invokes join, it must first obtain the pcb of the process it would like to join (joinPCB). Then, the current thread must 
	yield until the joinPCB has exited. Once it has exited, the exit status of the pcb is stored and returned by the join function.

	e. Kill

	When kill is invoked, the first step is to get the PCB of the process to be killed. Then, one must check if the pcb of the process to be 
	killed is the same as the one that called kill. If it is, then the current process calls doExit. If the two processes are distinct, it is 
	a valid kill. In this case, the exit status is set to 0, indicating it has exited. Then, DeleteExitedChildrenSetParentNull() is called to
	deallocate any children that have exited and set the parent to null for those that have not. After this, there is a check to see if pcb->parent
	is null and, if it is, the child called the pcbManager to deallocate its own pcb. Then, the thread to be killed is removed from the ready list
	and it is deleted. 

	f. Yield

	This system call was already implemented within the threads class. It gives up the CPU if any other thread is ready to run. If there is another
	thread ready to run, it places the current thread at the end of the ready to run list and runs the next thread that has been scheduled. 

NOTE: Appropriate checks were included throughout the system calls to ensure that the pid is never less than 0, and that certain values are not null or
invalid upon creation or use. 


#### 3. How to run tests

	a. Add the provided test files (fork.c, kill.c, etc) to the 'test' directory
	b. Update the makefile in the test directory so that the executable for the test file is created upon typing 'make'
		b-1. Add the name of the test file (without the .c) to the list of 'all: ' in the makefile
		b-2. Add the commands to properly compile the test file:

			EX: fork.o: fork.c
					$(CC) $(CFLAGS) -c fork.c
				fork: fork.o start.o
					$(LD) $(LDFLAGS) start.o fork.o -o fork.coff
					../bin/coff2noff fork.coff fork

			Here, one would replace each word 'fork' with the name of the test file you want to compile.

		b-3. Navigate to the top level code directory and type 'make'. You should see the executable file be created within the test directory.
			 If you do not see the executable, type 'make [x]', where '[x]' is the name of the file you want to compile.

	c. Navigate to the userprog directory and type './nachos -x ../test/[x]', where '[x]' is the name of the file you want to run.


### 4. Issues/Discrepancies

Though our tests run to completion, we had some initial confusion due to the expected output that was provided for the project. In the 
expected output, the pid's of the processes typically would only oscillate between 1 and 2, signifying that all children pcb's are being 
deallocated as soon as they have exited. However, in our output, the child pid's continue to increase until the program runs to completion.
After speaking to the professor, we understood that out output should be different from the expected output that was provided because we are
not fully deallocating the children pcb's to account for a potential join from the parent after the child has exited. This explains why our
output may not match up exactly to the expected one provided. Additionally, other error handling messages were used wherever a specific message was not
provided. 




#### Project 3: _(Title from Project 3 File)_



# 1. Code Design

The code is set up in a similar fashion to project2. All of the I/O system calls
are implemented within the exception.cc class. The PCB class was updated to include 
3 new functions: AllocateFileDescriptor, GetFile, and ReleaseFileDescriptor. These
functions allow users to manage file descripers using a map, similar to how the PCB's 
themselves are allocated. When a process attempts to open a file, it must use 
AllocateFileDescriptor to obtain the fileID that corresponds to the file. This id is
also needed to preform read and write operations on the specified files. Upon closing,
the process calls ReleaseFileDescriptor to free up the space and allow for more files 
to be allocated. 

# 2. How the code works

Following the test file provided, the program opens the source file named "in.dat". 
If the file cannot be opened (i.e., src is less than 0), the program exits with an error 
code of 100. The program then prompts the user to input the name of the destination file
and creates a file with that name. Then, the data is read from the source file, stored
in the buf array, and written into the destination file. 

# 3. How to run tests

a. Add the provided test files (fork.c, kill.c, etc) to the 'test' directory
	b. Update the makefile in the test directory so that the executable for the test file is created upon typing 'make'
		b-1. Add the name of the test file (without the .c) to the list of 'all: ' in the makefile
		b-2. Add the commands to properly compile the test file:

			EX: fork.o: fork.c
					$(CC) $(CFLAGS) -c fork.c
				fork: fork.o start.o
					$(LD) $(LDFLAGS) start.o fork.o -o fork.coff
					../bin/coff2noff fork.coff fork

			Here, one would replace each word 'fork' with the name of the test file you want to compile.

		b-3. Navigate to the top level code directory and type 'make'. You should see the executable file be created within the test directory.
			 If you do not see the executable, type 'make [x]', where '[x]' is the name of the file you want to compile.

	c. Navigate to the userprog directory and type './nachos -x ../test/[x]', where '[x]' is the name of the file you want to run.

# 4. Issues/Discrepancies:
We implemented the Open, Read, Write, Close, and Create functions and made modifications to the PCB class to manage file descriptors. However, when executing the test files (cp.c and concurrent.c), we encountered issues. Specifically, the program incorrectly reported that "in.dat" did not exist, despite creating files based on the content inside "in.dat." For instance, it generated a file named "HELLO WORLD!" and then prompted the user to input a destination file name. However, it failed to accept this input. Essentially, it appeared to misinterpret the content of "in.dat" as the destination file location, resulting in the creation of "HELLO WORLD!" and an inability to accept subsequent user input due to this misinterpretation.



