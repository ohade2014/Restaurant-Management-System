//
// Created by Yuval Eisner & Ohad Eliyahou on 11/6/18.
//
#include "../include/Dish.h"
#include <iostream>

//Constructor
Dish:: Dish(int d_id, std::string d_name, int d_price, DishType d_type): id(d_id), name(d_name) , price(d_price), type(d_type) {
};

//Return id of dish
int Dish:: getId() const{
    return id;
};

//Return name
std::string Dish:: getName() const{
    return name;
};

//Return price
int Dish:: getPrice() const{
    return price;
};

//Return type of dish
DishType Dish:: getType() const{
    return type;
};
