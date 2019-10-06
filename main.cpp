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
    //Create roads and push into a vector
    vector <Road> allRoads;
    for (int i=0; i <2; i++){
        Road newRoad; 
        allRoads.push_back(newRoad);
    }
    //Car car1(1, &road, 0x01);
    AccCar car1(1, &road, 0x02);
    AccCar car2(2, &road, 0x04);

    
    //road.add_car(&car1);
    road.add_acc_car(&car1);
    //road.add_acc_car(&car2);
    
    int car_1_start_position = -1;
    do {
        car_1_start_position = read_int("Enter a position between 20 and 40 for Car 1: " );
        pc.printf("\r\n");
    } while( car_1_start_position < 20 || car_1_start_position > 40);
    
    int car_1_start_speed = -1;
    do {
        car_1_start_speed = read_int("Enter a speed between 5 and 15 for Car 1: " );
        pc.printf("\r\n");
    } while( car_1_start_speed < 5 || car_1_start_speed > 15);
    
    int car_2_target_speed = -1;
    do {
        car_2_target_speed = read_int("Enter a speed between 5 and 15 for Car 2: ");
        pc.printf("\r\n");
    } while( car_2_target_speed < 5 || car_2_target_speed > 15);
    
    stopwatch.start();
    
    car2.set_forward_car(&car1);
    
    //car1.reset(car_1_start_position, car_1_start_speed);
    car2.reset(car_2_target_speed);
    stopwatch.reset();
    
    do {
        
        
        //evaluate if another car can be added to the road
        if(carCount < 5 &&  &&  ){ 
            AccCar newCar(carCount + 1, &road, 2^(carCount));
            allCars.push_back(&newCar);
            if(carCount > 0){ 
                newCar.set_forward_car(allCars[carCount-1]);
                newCar.reset(rand() % 11 + 5);
            }
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
        lcd.printf("1 %d -> %d\n2 %d -> %d", car1.position, car1.speed, car2.position, car2.speed);
        
    } while (car2.position <= ROADLENGTH); 
    car1.stop();
    car2.stop();
    
            // ----------------------------------------------------------------------
    // Timing statistics printout, do not modify
    pc.printf("Average update cycle took: %fms \r\n", (totalUpdateTime*1.0)/(numberCycles*1.0));
    totalUpdateTime = 0;
    numberCycles = 0;
    // ----------------------------------------------------------------------
}
