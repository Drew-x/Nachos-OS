
#ifndef ELEVATOR_H
#define ELEVATOR_H

#include "copyright.h"

void Elevator(int numFloors);
void ArrivingGoingFromTo(int atFloor, int toFloor);

typedef struct Person {
    int id;
    int atFloor;
    int toFloor;
} Person;


class ELEVATOR {

public:
    ELEVATOR(int numFloors);
    ~ELEVATOR();
    void hailElevator(Person *p);
    void start();

private:
    int currentFloor;
    Condition **entering;
    Condition **leaving;
    Condition *elevatorMoving;
    int *personsWaiting;
    int occupancy;
    int maxOccupancy;
    //Use anytime you want to signal, wait on, or broadcast a condition var
    Lock *elevatorLock;

};

#endif