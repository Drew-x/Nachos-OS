#ifndef PCB_H
#define PCB_H

#include "list.h"
#include "pcbmanager.h"

class Thread;
class PCBManager;
class Condition;
class Lock;
extern PCBManager* pcbManager;

class PCB {

    public:
        PCB(int id);
        ~PCB();
        int pid;
        PCB* parent;
        Thread* thread;
        int exitStatus;

        void AddChild(PCB* pcb);
        int RemoveChild(PCB* pcb);
        bool HasExited();
        void DeleteExitedChildrenSetParentNull();
        List* GetChildren();

    private:
        List* children;

};

#endif // PCB_H