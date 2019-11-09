#include <math.h>
#include "Road.h"

EXTERN Mutex print_mutex;

Road::Road(int id) {
    active_cars = 0x00;
    last_car = NULL;
        
    intersection_car = -1;
    next_release = 0;
    num_cars = 0;
    this->id = id;
    
    
    pending_car = new AccCar(num_cars, this, pow(2, num_cars));
    pending_car->set_target_speed(rand() % 11 + 5);
}

int Road::try_enter_car(int time) {
    if( next_release <= time && pending_car != NULL ) {
        if( last_car == NULL || last_car->position >= pending_car->speed + 2 ) {           
            pending_car->set_forward_car(last_car);
            pending_car->reset();
            active_cars = active_cars | pending_car->flag;
            
            last_car = pending_car;
            cars[num_cars] = pending_car;
            
            num_cars++;
            
            if(num_cars < MAX_CARS) {
                pending_car = new AccCar(num_cars, this, pow(2, num_cars));
                pending_car->set_target_speed(rand() % 11 + 5);
                
                next_release = time + rand() % 3;
            } else {
                pending_car = NULL;   
            }
            
            return last_car->id;
        }
    }
    
    return -1;
}
 
 void Road::let_cars_update() {
     if( active_cars > 0x00 ) {
        go_flags.set(active_cars);
     }
 }
 
 void Road::wait_for_car_update() {
     if( active_cars > 0x00 ) {
        done_flags.wait_all(active_cars);

        //poll the cars to see if anyone has reached the intersection
        car_waiting = NULL;
        for(int i = 0; i<num_cars; i++){ 
            if(cars[i]->queue_request  == 1) { 
                car_waiting = cars[i]; 
            }
        }
     }
 }
 
 

 int Road::check_exit_cars(int cars[]) {
     int arr_index = 0;
     
     for( int i=0; i < num_cars; i++ ) {
         if( this->cars[i]->position >= 100 ) {
            cars[arr_index] = this->cars[i]->id;
            arr_index++;
            
            active_cars = active_cars ^ this->cars[i]->flag;
            
            this->cars[i]->position = -1;
            this->cars[i]->speed = -1;
         }
     }
     
     return arr_index;   
 }
 
 void Road::print_status() {
     print_mutex.lock();
     for( int i=0; i < num_cars; i++ ) {
         printf("Car %d: %d -> %d\r\n", cars[i]->id, cars[i]->position, cars[i]->speed); 
     }   
     print_mutex.unlock();
 }
 