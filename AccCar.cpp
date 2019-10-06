#include "AccCar.h"
#include <stdlib.h>
#include <algorithm>

#define TICK 1000
#define MONITOR_DIST 17
#define SAFETY_GAP 2

AccCar::AccCar(int id, Road* road, int flag) {
    this->id = id;
    this->road = road;
    this->flag = flag;
        
    this->thread = NULL;
}

void AccCar::set_forward_car(Car* car) {
    this->forward_car = car;   
}

void AccCar::update() {
    while (true) {
        ThisThread::sleep_for(TICK);
        road->go_flags.wait_all(flag);
                
        position = position + speed;
        
        if (forward_car->position - position < MONITOR_DIST) {
            road->done_flags.wait_all(forward_car->flag, osWaitForever, false);
            
            int diff = forward_car->position - position;
            int maxSafeSpeed = diff - SAFETY_GAP;
            
            speed = std::min(maxSafeSpeed, target_speed);
        } else {
            speed = target_speed;   
        }
        
        road->done_flags.set(flag);   
    }
}

void AccCar::reset(int speed) {
    road->done_flags.clear(flag);
    
    if (thread != NULL) {
        thread->terminate();   
    }
    
    thread = new Thread();
    thread->start( callback(this, &AccCar::update) );
    
    this->position = 0;
    this->speed = speed;
    this->target_speed = speed;
}

void AccCar::stop() {
    if (thread != NULL) {
        thread->terminate();   
    }   
}