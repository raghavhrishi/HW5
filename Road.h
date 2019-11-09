#ifndef _ROAD_H_
#define _ROAD_H_

#include "mbed.h"
#include "AccCar.h"

#define MAX_CARS 5

class AccCar;

class Road {
public:
    int id;
    EventFlags go_flags;
    EventFlags done_flags;
    int active_cars;
    int intersection_car;
    AccCar * car_waiting;
    
    Road(int id);
    int try_enter_car(int time);
    void let_cars_update();
    void wait_for_car_update();
    int check_exit_cars(int cars[]);
    
    void print_status();
    
    
private:
    AccCar* last_car;
    AccCar* cars[MAX_CARS];
    
    AccCar* pending_car;
    int next_release;
    int num_cars;
};
#endif