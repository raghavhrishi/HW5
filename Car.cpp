#include "Car.h"
#include <stdlib.h>

#define TICK 1000

Car::Car(int id, Road* road, int flag) {
    this->id = id;   
    this->road = road;
    this->flag = flag;
    
    cycle = 0;
    
    this->thread = NULL;
}

void Car::update() {
    while (true) {
        ThisThread::sleep_for(TICK);
        road->go_flags.wait_all(flag);
        cycle++;
        
        position = position + speed;
        
        if (cycle % 5 == 0) {
            speed = rand() % 11 + 5;
        }
 
        road->done_flags.set(flag);   
    }
}

void Car::reset(int position, int speed) {
    road->done_flags.clear(flag);
    
    if (thread != NULL) {
        thread->terminate();   
    }
    
    thread = new Thread();
    thread->start( callback(this, &Car::update) );
    
    cycle = 0;
    this->position = position;
    this->speed = speed;
}

void Car::stop() {
    if (thread != NULL) {
        thread->terminate();   
    } 
}