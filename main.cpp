/* mbed Microcontroller Library
 * Copyright (c) 2018 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include "Road.h"
#include "AccCar.h"
#include "TextLCD.h"
#include "Mutex.h"
#include "Queue.h"
#include <vector>

vector<AccCar*> car_queue;
Mutex queue_lock;
Mutex print_mutex;


TextLCD lcd(p15, p16, p17, p18, p19, p20);

// main() runs in its own thread in the OS
int main() {
    // ------------------------------------------------------------------------------
    // The following three variables are used for timing statistics, do not modify them
    Timer stopwatch;    // A timer to keep track of how long the updates take, for statistics purposes
    int numberCycles = 0;
    int totalUpdateTime = 0;
    // ------------------------------------------------------------------------------

    int time = 0;

    Road road1(1);
    Road road2(2);

    stopwatch.start();
    stopwatch.reset();
    
    int return_cars[MAX_CARS];
    
    do {
        
        int new_car_1 = road1.try_enter_car(time);
        int new_car_2 = road2.try_enter_car(time);
        
        road1.let_cars_update();
        road2.let_cars_update();

        road1.wait_for_car_update();
        road2.wait_for_car_update();

        //Poll the roads for waiting cars
        if(road1.car_waiting){
            car_queue.push_back(road1.car_waiting);       
        }
        if(road2.car_waiting) { 
            car_queue.push_back(road2.car_waiting);
        }
       
       printf("Queue Size %d \r\n",car_queue.size());
       if(car_queue.size() > 0)
        printf("First Queue Item: Car %d \r\n",car_queue.at(0)->id);

        printf("\r\nRoad  Update %d\r\n", time);
        road1.print_status();
        road2.print_status();
        
        //print for road 1
        if(new_car_1 == -1 && road1.intersection_car == -1){
                lcd.printf("x, x");
        }
        else if(new_car_1 != -1 && road1.intersection_car == -1){
            lcd.printf("%d, x", new_car_1);
        }
        else if(new_car_1== -1 && road1.intersection_car != -1){
            lcd.printf("x, %d", road1.intersection_car);
        }
        else{
            lcd.printf("%d, %d", new_car_1, road1.intersection_car);
        }
         lcd.printf("\r\n");

        //print for road 2
        if(new_car_2 == -1 && road2.intersection_car == -1){
                lcd.printf("x, x");
        }
        else if(new_car_2 != -1 && road2.intersection_car == -1){
            lcd.printf("%d, x", new_car_2);
        }
        else if(new_car_2== -1 && road2.intersection_car != -1){
            lcd.printf("x, %d", road2.intersection_car);
        }
        else{
            lcd.printf("%d, %d", new_car_2, road2.intersection_car);
        }

        printf("\r\n");
        road1.check_exit_cars(return_cars);
        road2.check_exit_cars(return_cars);
        
        time++;
        
        // ------------------------------------------------------------------
        // Timing statistics logic, do not modify
        totalUpdateTime += stopwatch.read_ms();
        numberCycles++;
        stopwatch.reset();
        // ------------------------------------------------------------------
        
        lcd.cls();
        //lcd.printf("1 %d -> %d\n2 %d -> %d", 0,0,car2.position, car2.speed);
        
    } while( road1.active_cars > 0x00 || road2.active_cars > 0x00);
    
            // ----------------------------------------------------------------------
    // Timing statistics printout, do not modify
    printf("Average update cycle took: %fms \r\n", (totalUpdateTime*1.0)/(numberCycles*1.0));
    totalUpdateTime = 0;
    numberCycles = 0;
    // ----------------------------------------------------------------------
}
