#include "pcb.h"
#include <string.h>

//May want to bring in parent and thread as params to constructor?
PCB::PCB(int id) : pid(id), nextFd(0) {

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
       for (std::map<int, OpenFile*>::iterator it = fileTable.begin(); it != fileTable.end(); ++it) {
    delete it->second;
}
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




// File descriptor management methods

int PCB::AllocateFileDescriptor(OpenFile* file) {
    int fd = nextFd++;
    fileTable[fd] = file;
    return fd;
}

OpenFile* PCB::GetFile(int fileDescriptor) {
    std::map<int, OpenFile*>::iterator it = fileTable.find(fileDescriptor);

    if (it != fileTable.end()) {
        return it->second;
    }
    return NULL;
}

void PCB::ReleaseFileDescriptor(int fileDescriptor) {
   std::map<int, OpenFile*>::iterator it = fileTable.find(fileDescriptor);

    if (it != fileTable.end()) {
        delete it->second; // Close the file
        fileTable.erase(it);
    }

}






// //----------------------------------------------------------------------
// // PCB::AllocateFileDescriptor
// //  Allocates a file descriptor for a given file.
// //
// //  "fileName" is the name of the file for which the descriptor is allocated.
// //----------------------------------------------------------------------
// int PCB::AllocateFileDescriptor(char* fileName) {
//     int fileDescriptorId = bitmap->Find();
//     if(fileDescriptorId != -1) {
//         bool fileAlreadyOpen = false;

//         // Check if the file is already open in the global SysOpenFile table
//         for (int i = 0; i < MAX_FILES; i++) {
//             if (sysOpenFiles[i] != NULL && strcmp(sysOpenFiles[i]->fileName, fileName) == 0) {
//                 fileAlreadyOpen = true;
//                 fileDescriptorId = i;
//                 break;
//             }
//         }

//         if (fileAlreadyOpen) {
//             // Increment userOpens if file is already open
//             sysOpenFiles[fileDescriptorId]->userOpens++;
//         } else {
//             // Open the file and add a new entry in sysOpenFiles
//             OpenFile* openFile = fileSystem->Open(fileName);
//             sysOpenFiles[fileDescriptorId] = new SysOpenFile(openFile, fileDescriptorId, fileName);
//         }
//     }
//     return fileDescriptorId;
// }

// //----------------------------------------------------------------------
// // PCB::ReleaseFileDescriptor
// //  Releases and removes a specified file descriptor.
// //----------------------------------------------------------------------
// void PCB::ReleaseFileDescriptor(int fileDescriptorId)
// {
//     if(fileDescriptorId < 0) return;

//     bitmap->Clear(fileDescriptorId);
//     OFD *openFileDesc = ofds[fileDescriptorId];
//     oft->ReleaseOpenFileDescriptor(openFileDesc);
//     ofds[fileDescriptorId] = NULL;
// }

// //----------------------------------------------------------------------
// // PCB::FetchOpenFileDescriptor
// //  Retrieves the Open File Descriptor using its identifier.
// //----------------------------------------------------------------------
// OFD *PCB::FetchOpenFileDescriptor(int fileDescriptorId)
// {
//     if(fileDescriptorId < 0) return NULL;
//     return ofds[fileDescriptorId];
// }


// //----------------------------------------------------------------------
// // PCB::DeallocateFD
// //  Remove the given file descriptor (file id)
// //----------------------------------------------------------------------
// void PCB::RemoveFileDescriptor(int fid)
// {
//     if(fid < 0) return;

//     bitmap->Clear(fid);
//     OFD *ofd = ofds[fid];
//     oft->RemoveFileDescriptor(ofd);
//     ofds[fid] = NULL;
// }


// //----------------------------------------------------------------------
// // PCB::AllocateFD
// //  Return the file descriptor (file id) allocated
// //
// //  "fileName" is the name of the file
// //----------------------------------------------------------------------
// int PCB::AllocateFileDescriptor(char *fileName)
// {
//     int fid = bitmap->Find();
//     if(fid != -1)
//     {
//         ofds[fid] = oft->AllocateFileDescriptor(fileName);
//     }
//     return fid;
// }


// //----------------------------------------------------------------------
// // PCB::RetrieveFileDescriptor
// //  Retrieves the file descriptor based on the provided file ID.
// //----------------------------------------------------------------------
// OFD *PCB::RetrieveFileDescriptor(int fileId)
// {
//     if(fileId < 0) return NULL;
//     return ofds[fileId];
// }
