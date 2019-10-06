#ifndef _ROAD_H_
#define _ROAD_H_

#include "mbed.h"
#include "Car.h"
#include "AccCar.h"

class Car;
class AccCar;

class Road {
public:
    EventFlags go_flags;
    EventFlags done_flags;

    Road();
    void add_car(Car* car);
    void add_acc_car(AccCar* car);
    void let_cars_update();
    void wait_for_car_update();
        
    
private:
    Car* car1;
    AccCar* car2;
    
    int active_cars;
};
#endif