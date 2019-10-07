/* mbed Microcontroller Library
 * Copyright (c) 2018 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include "vector"
#include <cctype>
#include "Car.h"
#include "AccCar.h"
#include "TextLCD.h"
#include "Road.h"
#include <queue>
using namespace std;


Serial pc(USBTX, USBRX);
TextLCD lcd(p15, p16, p17, p18, p19, p20);

#define ROADLENGTH 100
queue <AccCar *> waiting_cars; 
Mutex queue_mutex;
EventFlags allow_entry;

// Read the max number of services to perform from pc input
int read_int(char* prompt) {
    int maxService = 0;
    
    pc.printf(prompt);
    char input;
    while(1) {
        input = pc.getc();
        pc.putc(input);
        
        if( std::isdigit(input) ) {
            maxService = (maxService * 10) + (input-'0');   
        } else {
            pc.putc(input);
            break;   
        } 
    }
    
    return maxService;
}

// main() runs in its own thread in the OS
int main()
{   
    // ------------------------------------------------------------------------------
    // The following three variables are used for timing statistics, do not modify them
    Timer stopwatch;    // A timer to keep track of how long the updates take, for statistics purposes
    int numberCycles = 0;
    int totalUpdateTime = 0;
    // ------------------------------------------------------------------------------
    
    //Function specific variables and objects
    int carCount;
    Road road; 
    stopwatch.reset();

    //initialize all 5 cars
    for(int i = 0; i <5 ; i ++){
        AccCar newCar(i+1, &road, 2^(carCount));        
    }

    do {    
            if(waiting_cars.size() >0){ 
                AccCar * newCar = waiting_cars.front();
                waiting_cars.pop();
                allow_entry.set(newCar->flag);
            }
            
            //added allCars vector for the road to reference each car on it (See Road.cpp and Road.h)
                //if(carCount > 0){ 
                    //newCar.set_forward_car(road.allCars[carCount-1]);
                    //newCar.reset(rand() % 11 + 5);
                //}
                
            //}
            //send the update signal
            road.let_cars_update();
            //wait for the cars to update
            road.wait_for_car_update();

        // ------------------------------------------------------------------
        // Timing statistics logic, do not modify
        totalUpdateTime += stopwatch.read_ms();
        numberCycles++;
        stopwatch.reset();
        // ------------------------------------------------------------------
        
        lcd.cls();
       // lcd.printf("1 %d -> %d\n2 %d -> %d", car1.position, car1.speed, car2.position, car2.speed);
        
    } while (road.allCars[5]->position <= ROADLENGTH); 
    
            // ----------------------------------------------------------------------
    // Timing statistics printout, do not modify
    pc.printf("Average update cycle took: %fms \r\n", (totalUpdateTime*1.0)/(numberCycles*1.0));
    totalUpdateTime = 0;
    numberCycles = 0;
    // ----------------------------------------------------------------------
}
