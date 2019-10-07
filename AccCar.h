#ifndef _ACC_CAR_H_
#define _ACC_CAR_H_

#include "mbed.h"
#include "Car.h"
#include "Road.h"

class Road;
class Car;

class AccCar{
public:
    int position;
    int speed;
    int flag;
    int counter;

    AccCar(int id, Road* road, int flag);
    void set_forward_car(AccCar* car);
    void update();
    void reset(int speed);
    void stop();
    
protected:
    int id;
    int target_speed;
    AccCar* forward_car;
    
    Road* road;
    Thread* thread; 
};
#endif