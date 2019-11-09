#include "AccCar.h"
#include <stdlib.h>
#include <algorithm>
#include <vector>

#define TICK 1000
#define MONITOR_DIST 17
#define SAFETY_GAP 2

EXTERN vector <AccCar *> car_queue;
EXTERN Mutex queue_lock;

AccCar::AccCar(int id, Road* road, int flag) {
    this->id = id;
    this->road = road;
    this->flag = flag;
    
    queue_request = false;
    waited = false;
        
    this->thread = NULL;
}

void AccCar::set_forward_car(AccCar* car) {
    this->forward_car = car;   
}

void AccCar::set_target_speed(int speed) {
    this->target_speed = speed;
    this->speed = speed;   
}

void AccCar::update() {
    do {
        ThisThread::sleep_for(TICK);
        road->go_flags.wait_all(flag);
        cycle++;   
        position = position + speed;
        
        if (cycle % 5 == 0) {
            target_speed = rand() % 11 + 5;
        }
        
        if( position < 54 ) {       
            if (forward_car != NULL && forward_car->position > -1) {
                if (forward_car->position - position < MONITOR_DIST && forward_car->position < 55) {
                    road->done_flags.wait_all(forward_car->flag, osWaitForever, false);
                    
                    int diff = forward_car->position - position;
                    int maxSafeSpeed = diff - SAFETY_GAP;
                    
                    speed = std::min(maxSafeSpeed, target_speed);
                } else {
                    speed = target_speed;   
                }
            } else {
                speed = target_speed;
            }
            
            int distance_to_intersection = 54 - position;
            speed = std::min(distance_to_intersection, speed);
                  
        } else if( position == 54 ) {
            queue_request++; 
            if( waited) {
                queue_lock.lock();
                if((car_queue.at(0)->id == this->id) && (car_queue.at(0)->road->id == this->road->id)){
                    road->intersection_car = id;
                    speed = 1;
                }
                queue_lock.unlock(); 
            } else {
                speed = 0;   
                waited = true;
                
            }

        } else if( position < 56 ) {
            speed = 1;
        } else {
            if( position == 56 ) {
                queue_lock.lock();
                car_queue.erase(car_queue.begin());
                queue_lock.unlock(); 
                road->intersection_car = -1;
                
            }
            
            if (forward_car != NULL && forward_car->position > -1) {
                if (forward_car->position - position < MONITOR_DIST ) {
                    road->done_flags.wait_all(forward_car->flag, osWaitForever, false);
                    int diff = forward_car->position - position;
                    int maxSafeSpeed = diff - SAFETY_GAP;
                    speed = std::min(maxSafeSpeed, target_speed);
                } else {
                    speed = target_speed;   
                }

            } else {
                speed = target_speed;
            }
        } 
        
        road->done_flags.set(flag);   
    } while( position < 100 );
}

void AccCar::reset() {
    road->done_flags.clear(flag);
    
    if (thread != NULL) {
        thread->terminate();   
    }
    
    thread = new Thread();
    thread->start( callback(this, &AccCar::update) );
    
    this->position = 0;
    this->cycle = 0;
}