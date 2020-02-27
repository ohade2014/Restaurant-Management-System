//
// Created by Yuval Eisner & Ohad Eliyahou on 11/6/18.
//
#include "../include/Table.h"
#include <iostream>
using namespace std;

//Constructor of Table
Table::Table(int t_capacity): capacity(t_capacity), open(false), total_price(0), available_seats(t_capacity), customersList(), orderList(){
};

//Distructor of Table
Table::~Table() {
    if (this->isOpen()){
        for (unsigned int i = 0; i < customersList.size(); ++i) {
            delete customersList[i];
        }
        customersList.clear();
    }
};

//Copy Constructor
Table::Table (Table &other):capacity(other.capacity), open(other.open), total_price(other.total_price), available_seats(other.available_seats), customersList(), orderList(){
    for (unsigned int j = 0; j < other.orderList.size(); ++j) {
        orderList.push_back(other.orderList[j]);
    }
    for (unsigned int i = 0; i <other.customersList.size() ; ++i) {
        customersList.push_back(other.customersList[i]->clone());
    }
};

//Copy Assignment operator
Table& Table::operator=(const Table& other){
    if (this != &other){
        capacity = other.capacity;
        open = other.open;
        total_price = other.total_price;
        available_seats = other.available_seats;
        //Clear existing customers
        for (unsigned int i = 0; i < customersList.size(); ++i) {
            delete customersList[i];
        }
        customersList.clear();
        //Add new customers
        for (unsigned int j = 0; j < other.customersList.size(); ++j) {
            customersList.push_back(other.customersList[j]);
        }
        //Replace orders
        orderList.clear();
        for (unsigned int k = 0; k < other.orderList.size() ; ++k) {
            orderList.push_back(other.orderList[k]);
        }
    }
    return *this;
};

//Move constructor
Table::Table(Table&& other): capacity(other.capacity), open(other.open), total_price(other.total_price), available_seats(other.available_seats),customersList(),orderList(){
    for (unsigned int j = 0; j < other.orderList.size(); ++j) {
        orderList.push_back(other.orderList[j]);
    }
    for (unsigned int i = 0; i < other.customersList.size() ; ++i) {
        customersList.push_back(other.customersList[i]);
        other.customersList[i] = nullptr;
    }
};

//Move assignment operator
Table& Table::operator=(Table&& other){
    if(this!=&other){
        this->capacity = other.capacity;
        this->open = other.open;
        this->total_price = other.total_price;
        this->available_seats = other.available_seats;
        //Clear existing customers
        for (unsigned int j = 0; j < customersList.size() ; ++j) {
            delete customersList[j];
        }
        customersList.clear();
        //Add new customers
        for (unsigned int i = 0; i < other.customersList.size() ; ++i) {
            customersList.push_back(other.customersList[i]);
            other.customersList[i] = nullptr;
        }
        //Replace orders
        orderList.clear();
        for (unsigned int k = 0; k < other.orderList.size() ; ++k) {
            orderList.push_back(other.orderList[k]);
        }
    }
    return *this;
};

// Return capacity
int Table::getCapacity() const{
    return capacity;
};

// Return available seats
int Table:: getAvailableSeats()const{
    return available_seats;
};

// Add customer to the table
void Table::addCustomer(Customer* customer){
    if (available_seats!=0){
        customersList.push_back(customer);
        available_seats--;
    }
    else{
        std::cout << "Cannot Add Customer" <<std::endl;
    }

};

//Remove customer from table
void Table::removeCustomer(int id){
    //Remove customer from customers list
    vector<Customer*> temp = vector<Customer*>();
    for (unsigned int i = 0; i < customersList.size(); ++i) {
        if(customersList[i]->getId() != id) {
            temp.push_back(customersList[i]);
        }
    }
    customersList.clear();
    for (unsigned int l = 0; l < temp.size(); ++l) {
        customersList.push_back(temp[l]);
    }
    temp.clear();
    //Remove customer's orders from OrdersList
    vector<OrderPair> temp2;
    for (unsigned int m = 0; m < orderList.size(); ++m) {
        if(orderList[m].first!=id){
            temp2.push_back(orderList[m]);
        }
        else{
            total_price = total_price - orderList[m].second.getPrice();
        }
    }
    orderList.clear();
    for (unsigned int n = 0; n < temp2.size(); ++n) {
        orderList.push_back(temp2[n]);
    }
    temp2.clear();
    // Update table's information
    available_seats++;
    if(available_seats==capacity){
        this->closeTable();
    }
};

//Return the customer with the input id
Customer* Table::getCustomer(int id){
    for (unsigned int i = 0; i < customersList.size(); ++i) {
        if(customersList[i]->getId() == id){
            return customersList[i];
        }
    }
    return nullptr; //id not found
};

// Return customers list
std::vector<Customer*>& Table::getCustomers(){
    return customersList;
};

// Return orders list
std::vector<OrderPair>& Table::getOrders(){
    return orderList;
};

// Make an order from table and return the dishes id's ordered
void Table::order(const std::vector<Dish> &menu){
    for (unsigned int i = 0; i < customersList.size(); ++i) {
        std::vector<int> orders = vector<int>(customersList[i]->order(menu));
        for (unsigned int j = 0; j < orders.size(); ++j) {
            OrderPair pair = OrderPair(customersList[i]->getId(), menu[orders[j]]);
            orderList.push_back(pair);
            total_price = total_price + menu[orders[j]].getPrice();
            std::cout << customersList[i]->getName() << " ordered " << menu[orders[j]].getName() << endl;
        }
    }
};

//Update table info to be open
void Table::openTable(){
    open = true;
};

//Close table and clear its contents
void Table::closeTable(){
    total_price = 0;
    for (unsigned int i = 0; i < customersList.size(); ++i) { //delete all customers
        delete customersList[i];
    }
    available_seats = capacity;
    customersList.clear();
    orderList.clear();
    open = false;
};

//Return total price
int Table::getBill(){
    return total_price;
};

//Adds to total price the add value inserted
void Table::AddToTotalPrice(int add){
    total_price = total_price + add;
};

//Return true if table is open and false if closed
bool Table::isOpen(){
    return open;
};