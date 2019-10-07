#include "Road.h"

Road::Road() {
    active_cars = 0x00;

}

void Road::add_car(Car* car) {
    this->car1 = car;
    active_cars = active_cars | car->flag;
}
 
void Road::add_acc_car(AccCar* car) {
    this->car2 = car;
    active_cars = active_cars | car->flag;
    allCars.push_back(car);
}
 
void Road::let_cars_update() {
    go_flags.set(active_cars);
}
 
void Road::wait_for_car_update() {
    done_flags.wait_all(active_cars);
}

int Road::getCars() { 
    return active_cars;
}