#ifndef _ACC_CAR_H_
#define _ACC_CAR_H_

#include "mbed.h"
#include "Road.h"

class Road;

class AccCar{
public:
    int id;
    int position;
    int speed;
    int flag;
    int queue_request;
    
    
    AccCar(int id, Road* road, int flag);
    
    void set_forward_car(AccCar* car);
    void set_target_speed(int speed);
    void update();
    void reset();
    Road* road;
    
protected:
    int target_speed;
    bool waited;
    int cycle;
    
    AccCar* forward_car;
    
   
    Thread* thread; 
};
#endif