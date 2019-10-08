/* mbed Microcontroller Library
 * Copyright (c) 2018 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include "vector"
#include <cctype>
#include "AccCar.h"
#include "TextLCD.h"
#include "Road.h"

Serial pc(USBTX, USBRX);
TextLCD lcd(p15, p16, p17, p18, p19, p20);

#define ROADLENGTH 100

// main() runs in its own thread in the OS
int main()
{   
    // ------------------------------------------------------------------------------
    // The following three variables are used for timing statistics, do not modify them
    Timer stopwatch;    // A timer to keep track of how long the updates take, for statistics purposes
    int numberCycles = 0;
    int totalUpdateTime = 0;
    // ------------------------------------------------------------------------------

    //Create road
    Road road; 
    int cooldown=0;
    int carCount=0;

    //Instantiate all the cars
    vector<AccCar *> allCars;

    AccCar Car1(1,&road,1);
    Car1.set_forward_car(NULL);
    allCars.push_back(&Car1);

    AccCar Car2(2,&road,2);
    Car2.set_forward_car(&Car1);
    allCars.push_back(&Car2);

    AccCar Car3(3,&road,4);
    Car3.set_forward_car(&Car2);
    allCars.push_back(&Car3);

    AccCar Car4(4,&road,8);
    Car4.set_forward_car(&Car3);
    allCars.push_back(&Car4);

    AccCar Car5(5,&road,16);
    Car5.set_forward_car(&Car4);
    allCars.push_back(&Car5);

    stopwatch.reset();
    do {
        //check the cooldown 
        if(cooldown == 0 && carCount<5) { 
            allCars[carCount]->reset(rand() % 11+5);
            road.add_acc_car(allCars[carCount]);
            carCount ++;
            cooldown = rand() %3;
        }
        else { 
            cooldown--;
        }
        pc.printf("Letting Cars Update \r\n");
        //send the update signal
        road.let_cars_update();
        pc.printf("Waiting for Response  \r\n");
        //wait for the cars to update
        road.wait_for_car_update();

        //lcd.cls();
        for (int i=0; i<5; i++){ 
           pc.printf("Car %d Position %d : Speed %d \r\n",i+1,allCars[i]->position,allCars[i]->speed);
        }

        // ------------------------------------------------------------------
        // Timing statistics logic, do not modify
        totalUpdateTime += stopwatch.read_ms();
        numberCycles++;
        stopwatch.reset();
        // ------------------------------------------------------------------
        
    } while (allCars[4]->position <= ROADLENGTH); 
    
    pc.printf("Stopping Simulation \r\n");
    for (int i=0; i<5; i++){ 
        allCars[i]->stop();
    }
            // ----------------------------------------------------------------------
    // Timing statistics printout, do not modify
    pc.printf("Average update cycle took: %fms \r\n", (totalUpdateTime*1.0)/(numberCycles*1.0));
    totalUpdateTime = 0;
    numberCycles = 0;
    // ----------------------------------------------------------------------
}
