#ifndef _CAR_H_
#define _CAR_H_

#include "Road.h"
#include "mbed.h"

class Road;

class Car {
public:
    int position;
    int speed;
    int flag;
    
    Car(int id, Road* road, int flag);
    void update();
    void reset(int position, int speed);
    void stop();
    
protected:
    int id;
    int cycle;
    Road* road;
    Thread* thread; 
};
#endif