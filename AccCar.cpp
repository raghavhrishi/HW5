#include "AccCar.h"
#include <stdlib.h>
#include <algorithm>
#include <queue>

#define TICK 1000
#define MONITOR_DIST 17
#define SAFETY_GAP 2
#define UCOUNT 5

extern queue<AccCar *>  waiting_cars;
extern Mutex queue_mutex;
extern EventFlags allow_entry;

AccCar::AccCar(int id, Road* road, int flag) {
    this->id = id;
    this->road = road;
    this->flag = flag;
    this->thread = NULL;
    //set the update counts 
    counter = 0;
}

void AccCar::set_forward_car(AccCar* car) {
    this->forward_car = car;   
}

void AccCar::update() {
    //wait to enter the road
    int randomwait = rand() % 3;
    ThisThread::sleep_for(randomwait*1000);
    //check other necessary road conditions
    if(road->allCars.size()>0){ 
        set_forward_car(road->allCars.size()-1);
        while(forward_car->position < 2); 
    }
    //push into the main queue
    queue_mutex.lock(); 
        waiting_cars.push(this);
    queue_mutex.unlock();
    //wait for the okay from main  
    allow_entry.wait_all(flag);

    while (true) {
        //sleep for 1 second
        ThisThread::sleep_for(TICK);
        road->go_flags.wait_all(flag);
        //Update position and speec control counter
        counter++;

        position = position + speed;
        //wait at STOP sign
        if(position == 54){ 
            speed = 0;
            continue;
        }
        // In Intersection speed will not be changed
        else if(position >54 && position <56) { 
            speed =1;
            continue;
        }
        else if(position == 56) { 
            counter = 0;
        }

        //Update Speed Logic @ 5 seconds, or if it is approaching intersection
        if(counter == 5 || 54- (position+speed)< 0){
        //forward car logic
            if (forward_car->position - position < MONITOR_DIST) {
                road->done_flags.wait_all(forward_car->flag, osWaitForever, false);
                int diff = forward_car->position - position;
                int maxSafeSpeed = diff - SAFETY_GAP;
                speed = std::min(maxSafeSpeed, target_speed);
            } else {
                speed = target_speed;   
            }
            //reset the speed change counts
            counter=0;
        } 
        //speed control to not overshoot
        else if(54- (position+speed)< 0) { 
            speed = 54-position;
        }

        //Done Updating - Continue Break Point
        contin:;
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