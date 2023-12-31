#include "copyright.h"
#include "system.h"
#include "synch.h"
#include "elevator.h"

#if defined(HW1_ELEVATOR) && defined(CHANGED)

int nextPersonID = 1;
Lock *personIDLock = new Lock("PersonIDLock");
int maxOccupancy;
int totalFloors;
int totalPersons;
const char* direction;

ELEVATOR *e;


void ELEVATOR::start() {

    while(1) {
        // A. Wait until hailed
        elevatorLock->Acquire();

        while(totalPersons == 0){
            elevatorMoving->Wait(elevatorLock);
        }

        elevatorLock->Release();

        // B. While there are active persons, loop doing the following
            //active ppl are those either waiting at a floor or inside the elevator
            //if occupancy + peopleWaiting == 0, there is no one to be serviced, 
            // & elevator can be blocked until it is hailed 
        while(totalPersons > 0){
        //      0. Acquire elevatorLock
                elevatorLock->Acquire();
        //      1. Signal persons inside elevator to get off (leaving->broadcast(elevatorLock))
                leaving[currentFloor]->Broadcast(elevatorLock);
        //      2. Signal persons atFloor to get in, one at a time, checking occupancyLimit each time
                    //Elevator won't signal unless there's room in it
                elevatorLock->Release();

                elevatorLock->Acquire();
                for(int i = 0; i < personsWaiting[currentFloor]; i++){
                    if(occupancy <= maxOccupancy){
                        entering[currentFloor]->Signal(elevatorLock);
                    }

                }
        //      2.5 Release elevatorLock
                elevatorLock->Release();
        //      3. Spin for some time
                for(int j =0 ; j< 1000000; j++) {
                    currentThread->Yield();
                }
        //      4. Go to next floor
                //Update current floor once you get to next floor
                elevatorLock->Acquire();
                if (direction == "up") {
                    if (currentFloor == totalFloors - 1) {
                        // Reached the top floor, change direction to "down"
                        direction = "down";
                    } else {
                        currentFloor++;
                    }
                } else if (direction == "down") {
                    if (currentFloor == 0) {
                        // Reached the first floor, change direction to "up"
                        direction = "up";
                    } else {
                        currentFloor--;
                    }
                }
                elevatorLock->Release();

        }

          printf("Elevator arrives on floor %d \n", currentFloor);
    }
}

void ElevatorThread(int numFloors) {

    totalFloors = numFloors;
    printf("Elevator with %d floors was created!\n", numFloors);

    e = new ELEVATOR(numFloors);

    e->start();


}

ELEVATOR::ELEVATOR(int numFloors) {
    currentFloor = 0;
    direction = "up";
    totalPersons = 0;
    occupancy = 0;
    maxOccupancy = 5;

    entering = new Condition*[numFloors];
    // Initialize entering
    for (int i = 0; i < numFloors; i++) {
        entering[i] = new Condition("Entering " + i);
    }
    personsWaiting = new int[numFloors];
    elevatorLock = new Lock("ElevatorLock");
    elevatorMoving = new Condition("ElevatorMovingCondition");

    // Initialize leaving
     leaving = new Condition*[numFloors];
    for (int i = 0; i < numFloors; i++) {
        leaving[i] = new Condition("Leaving " + i);
    }
}


void Elevator(int numFloors) {
    // Create Elevator Thread
    Thread *t = new Thread("Elevator");
    t->Fork(ElevatorThread, numFloors);
}


void ELEVATOR::hailElevator(Person *p) {
       // printf("test\n");

    // 1. Increment waiting persons atFloor
    elevatorLock->Acquire();
    personsWaiting[p->atFloor]++;
    elevatorLock->Release();
    // 2. Hail Elevator
    elevatorLock->Acquire();
    elevatorMoving->Signal(elevatorLock);
    elevatorLock->Release();
    // 2.5 Acquire elevatorLock;
    elevatorLock->Acquire();
    // 3. Wait for elevator to arrive atFloor [entering[p->atFloor]->wait(elevatorLock)]
    while(currentFloor != p->atFloor){

        entering[p->atFloor]->Wait(elevatorLock);

    }
    elevatorLock->Release();
        //must release lock anytime you do a wait, and then reaquire upon returning from wait
    // 5. Get into elevator
    printf("Person %d got into the elevator.\n", p->id);
    // 6. Decrement persons waiting atFloor [personsWaiting[atFloor]++]
    elevatorLock->Acquire();
    personsWaiting[p->atFloor]--;
    // 7. Increment persons inside elevator [occupancy++]
    occupancy++;
    elevatorLock->Release();

    elevatorLock->Acquire();
    // 8. Wait for elevator to reach toFloor [leaving[p->toFloor]->wait(elevatorLock)]
    while(currentFloor != p->toFloor){
        leaving[p->toFloor]->Wait(elevatorLock);
    }
    elevatorLock->Release();

            //must release lock anytime you do a wait, and then reaquire upon returning from wait
    // 9. Get out of the elevator
    printf("Person %d got out of the elevator.\n", p->id);
    // 10. Decrement persons inside elevator
    elevatorLock->Acquire();
    occupancy--;
    totalPersons--;
    // 11. Release elevatorLock;
    elevatorLock->Release();
}

void PersonThread(int person) {

    Person *p = (Person *)person;

    printf("Person %d wants to go from floor %d to %d\n", p->id, p->atFloor, p->toFloor);

    e->hailElevator(p);

}

int getNextPersonID() {
    int personID = nextPersonID;
    personIDLock->Acquire();
    nextPersonID = nextPersonID + 1;
    personIDLock->Release();

    personIDLock->Acquire();
    totalPersons++;
    personIDLock->Release();
    return personID;
}


void ArrivingGoingFromTo(int atFloor, int toFloor) {

    // Create Person struct
    Person *p = new Person;
    p->id = getNextPersonID();
    p->atFloor = atFloor - 1;
    p->toFloor = toFloor - 1;

    // Creates Person Thread
    Thread *t = new Thread("Person " + p->id);
    t->Fork(PersonThread, (int)p);

}

#endif
