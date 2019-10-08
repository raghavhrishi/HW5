#include "AccCar.h"
#include <cstdlib>
#include <algorithm>
#include <rtos.h>

#define TICK 1000
#define MONITOR_DIST 17
#define SAFETY_GAP 2



AccCar::AccCar(int id, Road* road, int flag) {
    this->id = id;
    this->road = road;
    this->flag = flag;
    this->thread = NULL;
    //Set the position and speed to 0
    this->speed = 0;
    this->position = -1;
   
}

void AccCar::set_forward_car(AccCar* car) {
    this->forward_car = car;   
}

void AccCar::update() {
    //----------------Initialization--------------------
    int counter;
    int diff;
    int maxSafeSpeed;
    status =0;

    while (true) {
        //sleep for 1 second and update control variables
        ThisThread::sleep_for(TICK);
        road->go_flags.wait_all(flag);
        position = position + speed;
        //----------------Apply State Logic--------------------
        switch(status){
            case 0:
                //Evaluate distance to nearest vehicle
                    if(forward_car->position - position < MONITOR_DIST) { 
                        road->done_flags.wait_all(forward_car->flag, osWaitForever, false);
                        diff = forward_car->position - position;
                        maxSafeSpeed = diff - SAFETY_GAP;
                        speed = std::min(maxSafeSpeed, target_speed);
                } else { 
                    speed = target_speed;
                }
                //Evaluate distance to intersection and override
                if(position+speed >=54 && position <55){ 
                    speed = 54-position;
                    status = 1;
                }
               break;
            case 1:
                speed = 0;
                //Switch to the Intersect State
                status = 2;
                break;
            case 2: 
                speed =1;
                //Switch back to ACC State
                if(position >55)
                    status=0;
                break;
        }

        counter ++ ;
        if(counter == 5){ 
            target_speed = rand()%11+5;
            counter = 0;
        } else { counter ++;}

        //Done Updating - Continue Break Point
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