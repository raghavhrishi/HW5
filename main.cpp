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
#include <stdlib.h>
#include <string>

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
    stopwatch.start();

    //Create road
    Road road; 
    int cooldown=0;
    int carCount=0;
    int inroad;
    int intersector;
    string out_string;

    //Instantiate all the cars
    vector<AccCar *> allCars;
    vector<int> speed;
    
    AccCar Car1(1,&road,1);
    Car1.set_forward_car(NULL);
    allCars.push_back(&Car1);
    speed.push_back(rand() % 11+5);

    AccCar Car2(2,&road,2);
    Car2.set_forward_car(&Car1);
    allCars.push_back(&Car2);
    speed.push_back(rand() % 11+5);

    AccCar Car3(3,&road,4);
    Car3.set_forward_car(&Car2);
    allCars.push_back(&Car3);
    speed.push_back(rand() % 11+5);

    AccCar Car4(4,&road,8);
    Car4.set_forward_car(&Car3);
    allCars.push_back(&Car4);
    speed.push_back(rand() % 11+5);

    AccCar Car5(5,&road,16);
    Car5.set_forward_car(&Car4);
    allCars.push_back(&Car5);
    speed.push_back(rand() % 11+5);

    stopwatch.reset();
   
    do {
        pc.printf("---------------------\r\n",cooldown);
        //check the cooldown and car count
        out_string = "X,";
        //check cooldown period (random [0,3]) , Less than 5 cars on the road (carCount), and the speed of the car is less than the forward_car
        if(cooldown <= 0 && carCount<5 && speed[carCount] <= (allCars[carCount]->forward_car->position-2)) { 
            //Add the Car to the Road
            allCars[carCount]->reset(speed[carCount]);
            road.add_acc_car(allCars[carCount]);
            carCount ++;
            out_string = std::to_string(carCount) + ",";
            cooldown = rand() %4;   
            pc.printf("Cooldown Timer:  %d \r\n",cooldown);

        }
        else { 
            //decrement the cooldown (timer)
            cooldown--;
        }
        
        //send the update signal
        road.let_cars_update();

        //wait for the cars to update
        road.wait_for_car_update();

        //Cleat the 
        lcd.cls();
        intersector=0;
        for (int i=0; i<5; i++){ 
            pc.printf("Car %d: position %d, speed %d \r\n",i+1,allCars[i]->position,allCars[i]->speed);
            if(allCars[i]->position == 55 ){ 
                intersector = allCars[i]->id;
            } 
        }
        // ------------------------------------------------------------------
        // Timing statistics logic, do not modify
        totalUpdateTime += stopwatch.read_ms();
        numberCycles++;
        stopwatch.reset();
        // ------------------------------------------------------------------
        if(intersector == 0) { out_string += "X";} else {out_string+=to_string(intersector);}
        lcd.printf("%s",out_string.c_str());

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
