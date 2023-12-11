#include "pcb.h"


//May want to bring in parent and thread as params to constructor?
PCB::PCB(int id) {

    //assign pid to process
    //must ensure these id's are unique
    pid = id;

    //Initialize parent & children
    parent = NULL;
    children = new List();
    
    //Initialize thread
    thread = NULL;

    exitStatus = -9999;

}



PCB::~PCB() {

    delete children;

}



void PCB::AddChild(PCB* pcb) {

    children->Append(pcb);


}

//returns int because RemoveItem gives back 0 for success & -1 for failure
int PCB::RemoveChild(PCB* pcb){

    return children->RemoveItem(pcb);

}


bool PCB::HasExited() {
    return exitStatus == -9999 ? false : true;
}


void decspn(int arg) {
    PCB* pcb = (PCB*)arg;
    if (pcb->HasExited()) pcbManager->DeallocatePCB(pcb);
    else pcb->parent = NULL;
}


void PCB::DeleteExitedChildrenSetParentNull() {
    children->Mapcar(decspn);
}

List* PCB::GetChildren() {
    return children;
}