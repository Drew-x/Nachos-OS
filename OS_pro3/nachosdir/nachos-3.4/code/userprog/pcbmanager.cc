#include "pcbmanager.h"

//This class is meant to manage the pcb class
//keeps track of state of all objects in this class
//creates pcb's, we use this class rather than pcb class directly

//maxprocesses is the max number of pcbs that can be made
PCBManager::PCBManager(int maxProcesses) {

    bitmap = new BitMap(maxProcesses);
    pcbs = new PCB*[maxProcesses];
    pcbManagerLock = new Lock("pcbManagerLock");

    //initialize all pcbs to null
    for(int i = 0; i < maxProcesses; i++) {
        pcbs[i] = NULL;
    }

}


PCBManager::~PCBManager() {

    delete bitmap;

    delete pcbs;

}

//Returns pointer to PCB
//
PCB* PCBManager::AllocatePCB() {

    // Aquire pcbManagerLock
    pcbManagerLock->Acquire();

    int pid = bitmap->Find();
   // printf("PID FOUND: %d\n", pid);
    // Release pcbManagerLock
    pcbManagerLock->Release();


  if(pid == -1){
        return NULL;
    }

    pcbs[pid] = new PCB(pid);

    return pcbs[pid];

}

//returns integer bc this can fail if pcb to deallocate doesnt exist
int PCBManager::DeallocatePCB(PCB* pcb) {

    // Check if pcb is valid -- check pcbs for pcb->pid
     if (pcbs[pcb->pid] == NULL){
     
     return -1;
     }

     // Aquire pcbManagerLock
    pcbManagerLock->Acquire();


    bitmap->Clear(pcb->pid);

    // Release pcbManagerLock
    pcbManagerLock->Release();

    int pid = pcb->pid;
   // printf("PID DELETED: %d\n", pid);

    delete pcbs[pid];
    pcbs[pid] = NULL;

    return 0;

}

PCB* PCBManager::GetPCB(int pid) {
    return pcbs[pid];
    
}