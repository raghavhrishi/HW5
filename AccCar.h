#ifndef _ACC_CAR_H_
#define _ACC_CAR_H_

#include "mbed.h"
#include "Road.h"

class Road;
class Car;
//enum OPCODE {ACC=0,APPROACH=1,INTERSECT=2,CHANGETGT=3,STOP=4};

class AccCar{
public:
    int status;
    int position;
    int speed;
    int flag;
    
    
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