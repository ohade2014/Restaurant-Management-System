//
// Created by Yuval Eisner & Ohad Eliyahou on 11/9/18.
//
#include "../include/Customer.h"
#include <iostream>

//Constructor
Customer::Customer(std::string c_name, int c_id): name(c_name) , id(c_id), orders(){};

//Distructor
Customer::~Customer() {};

//Return customer name
std::string Customer::getName() const{
    return name;
};

//Return customer id
int Customer::getId() const{
    return id;
};

//return vector of orders
std:: vector<int> Customer:: getOrders(){
    return orders;
}

//VegeterianCustomer constructor
VegetarianCustomer ::VegetarianCustomer(std::string name, int id): Customer(name,id) {};

//Distructor
VegetarianCustomer::~VegetarianCustomer() {};

//Return vector of dishes id's that Vegeterian customer orders
std::vector<int> VegetarianCustomer::order(const std::vector<Dish> &menu){
    if (orders.size() > 0) //if this customer already ordered, order the same as last time
        return orders;
    bool Veg_Dish_Found = false;
    int max_i = -1;
    int max_price = 0;
    for (unsigned int i = 0; i < menu.size(); ++i) {
        if ((menu[i].getType() == VEG) & (!Veg_Dish_Found)) { // find the vegeterian dish with smallest id and add it to orders
            orders.push_back(menu[i].getId());
            Veg_Dish_Found = true;
        }
        else if ((menu[i].getType()==BVG) && (max_price < menu[i].getPrice())){ //find the most expensive beverage
            max_price = menu[i].getPrice();
            max_i = i;
        }
    }

    //if both dishes found, add to orders.
    if((Veg_Dish_Found) & (max_i != -1)){
        orders.push_back(max_i);
    }
    else{
        orders.clear();
    }
    return orders;
};

//Return string representing Vegeterian customer
std::string VegetarianCustomer::toString() const{
    return this->getName() + ",veg";
};

//Clone vegeterian customer
VegetarianCustomer* VegetarianCustomer:: clone(){
    VegetarianCustomer* veg=new VegetarianCustomer(this->getName(),this->getId());
    for (unsigned int i = 0; i <this->getOrders().size() ; ++i) { //copy orders to the new customer's orders
        veg->orders.push_back(this->orders[i]);
    }
    return veg ;
};

//Constructor of Cheap Customer
CheapCustomer ::CheapCustomer(std::string name, int id):Customer(name,id), is_ordered(false) {};

//Distructor
CheapCustomer::~CheapCustomer() {};

//Return vector of dishes id's that Cheap customer orders
std::vector<int> CheapCustomer::order(const std::vector<Dish> &menu){
    if(!is_ordered) {
        int min_i = 0;
        int min_price = menu[0].getPrice();
        //Find cheapest dish in the menu
        for (unsigned int i = 1; i < menu.size(); ++i) {
            if (min_price > menu[i].getPrice()) {
                min_price = menu[i].getPrice();
                min_i = i;
            }
        }
        orders.push_back(min_i);
        is_ordered = true;
        return orders;
    }
    return std::vector<int>(); //Customer already ordered, and is should order once, return empty vector.
};

//Clone cheap customer
CheapCustomer* CheapCustomer:: clone(){
    return new CheapCustomer(*this);
};

//Return string represents this cheap customer
std::string CheapCustomer::toString() const{
    return this->getName() + ",chp";
};

//Constructor of spicy customer
SpicyCustomer ::SpicyCustomer(std::string name, int id): Customer(name,id),is_ordered(false),min_bvg_index(0) {};

//Distructor
SpicyCustomer::~SpicyCustomer() {};

//Return vector with dishes id's that spicy customer orders
std::vector<int> SpicyCustomer::order(const std::vector<Dish> &menu){
    if(min_bvg_index != -2) { //Customer ordered but got error
        if (!is_ordered) { //First order
            int max_i = -1;
            int max_price = 0;
            int min_bvg_price = -1;
            for (unsigned int i = 0; i < menu.size(); ++i) { //Find the most expensive spicy dish
                if ((menu[i].getType() == SPC) & (max_price < menu[i].getPrice())) {
                    max_price = menu[i].getPrice();
                    max_i = i;
                }
                //Find the cheapest Beverage dish for next orders
                if (((min_bvg_price == -1) & (menu[i].getType() == BVG)) |
                    ((menu[i].getType() == BVG) & (min_bvg_price > menu[i].getPrice()))) {
                    min_bvg_price = menu[i].getPrice();
                    min_bvg_index = i;
                }
            }
            if (max_i != -1) {
                orders.push_back(max_i);
                is_ordered = true;
            }
            else
                min_bvg_index = -2;
        }
        else { //Already ordered
            orders.clear();
            orders.push_back(menu[min_bvg_index].getId());
        }
    }
    return orders;
};

//Return string represents spicy customer
std::string SpicyCustomer::toString() const{
    return this->getName() + ",spc";
};

//Clone spicy customer
SpicyCustomer* SpicyCustomer:: clone(){
    return new SpicyCustomer(*this);
};

//Constructor of Alcoholic customer
AlchoholicCustomer ::AlchoholicCustomer(std::string name, int id): Customer(name,id),alcoholic_dishes(),is_ordered(false) {};

//Distructor
AlchoholicCustomer::~AlchoholicCustomer() {};

//Return vector of dishes id's that Alcoholic customer orders
std::vector<int> AlchoholicCustomer::order(const std::vector<Dish> &menu){
    int min_price_id;
    int min_price;
    //Create vector with alcoholic dishes
    if((orders.size() == 0) & (!is_ordered)) {
        is_ordered = true;
        for (unsigned int i = 0; i < menu.size(); ++i) {
            if (menu[i].getType() == ALC) {
                alcoholic_dishes.push_back(menu[i].getId());
            }
        }
    }
    orders.clear(); //Clear previous orders
    // Find cheapest alcoholic dish in the vector
    if (alcoholic_dishes.size() != 0) {
        min_price = menu[alcoholic_dishes[0]].getPrice();
        min_price_id = 0;
        for (unsigned int j = 1; j < alcoholic_dishes.size(); ++j) {
            if (min_price > menu[alcoholic_dishes[j]].getPrice()) {
                min_price_id = j;
                min_price = menu[j].getPrice();
            }
        }
        orders.push_back(menu[alcoholic_dishes[min_price_id]].getId());
    }
    //Remove the current cheapest alcoholic dish from vector
    std::vector<int> temp;
    int index;
    for (unsigned int i = 0; i < alcoholic_dishes.size(); ++i) {
        index = i;
        if(index != min_price_id) {
            temp.push_back(alcoholic_dishes[i]);
        }
    }
    alcoholic_dishes.clear();
    for (unsigned int l = 0; l < temp.size(); ++l) {
        alcoholic_dishes.push_back(temp[l]);
    }
    temp.clear();
    return orders;
};

//Return string represents Alcoholic customer
std::string AlchoholicCustomer::toString() const{
    return this->getName() + ",alc";
};

//Clone Alcoholic customer
AlchoholicCustomer* AlchoholicCustomer:: clone(){
    std::vector<int> clone_vec(alcoholic_dishes);
    AlchoholicCustomer* alc = new AlchoholicCustomer(this->getName(),this->getId());
    alc->alcoholic_dishes = clone_vec;
    return alc;
};






