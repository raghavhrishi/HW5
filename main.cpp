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

Serial pc(USBTX, USBRX);
TextLCD lcd(p15, p16, p17, p18, p19, p20);

#define ROADLENGTH 100

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
    
    int carCount;
    //Create a vector to store the cars
    vector < AccCar *> allCars;
    //Create roads
    Road road; 
    int cooldown;
    stopwatch.reset();
    
    do {
        //check the cooldown for adding cars to the road
        if (cooldown == 0) {
            //Check the number of cars on the road
            if(carCount < 5) { 
                //Create a new car
                carCount ++;
                AccCar newCar(carCount,&road,2^(carCount-1));
                //First Car Check
                if(carCount > 1){ 
                    //check the distance to the next car on the road
                    
                }         
            }
            //Set the cooldown for the next car
            cooldown = rand() % 3;
            } 
            else { 
                cooldown --; 
        }
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
        
    } while (allCars[5]->position <= ROADLENGTH); 
    
            // ----------------------------------------------------------------------
    // Timing statistics printout, do not modify
    pc.printf("Average update cycle took: %fms \r\n", (totalUpdateTime*1.0)/(numberCycles*1.0));
    totalUpdateTime = 0;
    numberCycles = 0;
    // ----------------------------------------------------------------------
}
