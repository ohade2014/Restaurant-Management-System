//
// Created by Yuval Eisner & Ohad Eliyahou  on 11/10/18.
//

#include "../include/Action.h"
#include "../include/Table.h"
#include "../include/Restaurant.h"
#include <iostream>

extern Restaurant* backup;

//Constructor os BaseAction
BaseAction::BaseAction(): errorMsg(""),status(PENDING){};

BaseAction::~BaseAction() {};

//Return the status of the action
ActionStatus BaseAction::getStatus() const {
    return status;
};

//Assign action as completed
void BaseAction::complete() {
    status = COMPLETED;
};

//Assign action as error, and update error msg
void BaseAction::error(std::string errorMsg){
    status = ERROR;
    this->errorMsg = errorMsg;
};

//Return error msg
std::string BaseAction::getErrorMsg() const {
    return errorMsg;
};

//Convert Action to string
std::string BaseAction::PrintActionStatus(ActionStatus ActStatus){
    if (ActStatus == PENDING)
        return "Pending";
    else if (ActStatus == COMPLETED)
        return "Completed";
    else
        return "Error";
};

//Constructor of Open Table
OpenTable::OpenTable(int id, std::vector<Customer *> &customersList):BaseAction(), tableId(id), customers() {
    for (unsigned int i = 0; i < customersList.size() ; ++i) { //deep copy of customers
        Customer* customer = customersList[i]->clone();
        customers.push_back(customer);
    }
};

//Distructor of Open Table
OpenTable::~OpenTable() {
    for (unsigned int i = 0; i < this->customers.size() ; ++i) {
        delete customers[i];
    }
    customers.clear();
};

//Copy constructor
OpenTable::OpenTable(OpenTable& other): tableId(other.tableId),customers() {
    if (other.getStatus() == COMPLETED)
        complete();
    else
        error(other.getErrorMsg());
    for (unsigned int i = 0; i < other.customers.size(); ++i) {
        this->customers.push_back(other.customers[i]->clone());
    }
};

//Move Constructor
OpenTable::OpenTable(OpenTable&& other): tableId(other.tableId), customers() {
    if (other.getStatus() == COMPLETED)
        complete();
    else
        error(other.getErrorMsg());
    for (unsigned int i = 0; i < other.customers.size(); ++i) {
        this->customers.push_back(other.customers[i]);
        other.customers[i] = nullptr;
    }
};

//Activate Open Table and add to Actions Log
void OpenTable::act(Restaurant &restaurant) {
    if(restaurant.getTable(tableId) == nullptr || restaurant.getTable(tableId)->isOpen()){
        std::cout << "Error: Table does not exist or is already open" << std::endl;
        error("Table does not exist or is already open");
    }
    else {
        restaurant.getTable(tableId)->openTable();
        complete();
    }
    restaurant.setActionsLog(this);
};

//Return string represents this action
std::string OpenTable::toString() const {
    std::string table = "open " + std::to_string(tableId);
    for (unsigned int i = 0; i < customers.size(); ++i) {
        table = table + " " + customers[i]->toString();
    }
    if (this->getStatus() == COMPLETED)
        table = table + " " + "Completed";
    else
        table = table + " " + "Error: " + this->getErrorMsg();
    return table;
};

//Clone Open Table action
OpenTable* OpenTable::clone() {
    std::vector<Customer*> clone_customers;
    for (unsigned int i = 0; i < customers.size() ; ++i) { //Deep copy customers
        clone_customers.push_back(customers[i]); //we deleted clone on customers to fix leak
    }
    OpenTable* cloned_action = new OpenTable(tableId, clone_customers);
    if (this->getStatus() == COMPLETED)
        cloned_action->complete();
    else if (this->getStatus() == ERROR)
        cloned_action->error(this->getErrorMsg());
    return cloned_action;
};

//Constructor of Order
Order::Order(int id): BaseAction(),tableId(id){};

//Activate Order action and add to Actions Log
void Order::act(Restaurant &restaurant) {
    if(restaurant.getTable(tableId) == nullptr || !restaurant.getTable(tableId)->isOpen()) {
        std::cout << "Error: Table does not exist or is not open" << std::endl;
        error("Table does not exist or is not open");
    }
    else{
        restaurant.getTable(tableId)->order(restaurant.getMenu());
        complete();
    }
    restaurant.setActionsLog(this);
};

//Return string represents order
std::string Order::toString() const{
    std::string output_string = "order " + std::to_string(tableId);
    if (this->getStatus() == COMPLETED)
        output_string = output_string + " " + "Completed";
    else
        output_string = output_string + " " + "Error: " + this->getErrorMsg();
    return output_string;
};

//Clone Order
Order* Order::clone() {
    Order* cloned_action = new Order(tableId);
    if (this->getStatus() == COMPLETED)
        cloned_action->complete();
    else if (this->getStatus() == ERROR)
        cloned_action->error(this->getErrorMsg());
    return cloned_action;
};

//Constructor of Move Customer
MoveCustomer::MoveCustomer(int src, int dst, int customerId) :BaseAction(),srcTable(src),dstTable(dst),id(customerId){}

//Activate Move Customer and Add to Actions Log
void MoveCustomer:: act(Restaurant &restaurant){
  if(((restaurant.getTable(srcTable)== nullptr)|(restaurant.getTable(dstTable)== nullptr))||((!restaurant.getTable(srcTable)->isOpen())|(!restaurant.getTable(dstTable)->isOpen()))||((restaurant.getTable(dstTable)->getAvailableSeats()==0))){
      std::cout<<"Error: Cannot move customer"<<std::endl;
      error("Cannot move customer");
  }
  else if(restaurant.getTable(srcTable)->getCustomer(id)== nullptr){
      std::cout<<"Error: Cannot move customer"<<std::endl;
      error("Cannot move customer");
  }
  else{ //move customer is possible
      restaurant.getTable(dstTable)->addCustomer(restaurant.getTable(srcTable)->getCustomer(id));
      for (unsigned int i = 0; i <restaurant.getTable(srcTable)->getOrders().size() ; ++i) {
          if(restaurant.getTable(srcTable)->getOrders()[i].first==id){
              OrderPair pair = OrderPair(restaurant.getTable(srcTable)->getOrders()[i].first, restaurant.getTable(srcTable)->getOrders()[i].second);
              restaurant.getTable(dstTable)->getOrders().push_back(pair);
              restaurant.getTable(dstTable)->AddToTotalPrice(restaurant.getTable(srcTable)->getOrders()[i].second.getPrice());
          }
      }
      restaurant.getTable(srcTable)->removeCustomer(id);
      complete();
  }
  restaurant.setActionsLog(this);
};

//Return string represents Move Customer
std::string MoveCustomer::toString() const{
    std::string output_string = "move " + std::to_string(srcTable) + " " + std::to_string(dstTable) + " " + std::to_string(id);
    if (this->getStatus() == COMPLETED)
        output_string = output_string + " " + "Completed";
    else
        output_string = output_string + " " + "Error: " + this->getErrorMsg();
    return output_string;
};

//Clone Move Customer
MoveCustomer* MoveCustomer::clone() {
    MoveCustomer* cloned_action = new MoveCustomer(srcTable, dstTable, id);
    if (this->getStatus() == COMPLETED)
        cloned_action->complete();
    else if (this->getStatus() == ERROR)
        cloned_action->error(this->getErrorMsg());
    return cloned_action;
};

//Constructor of Close
Close:: Close(int id): BaseAction(),tableId(id){};

//Activate close and add to actions log
void Close::act(Restaurant &restaurant) {
    if(restaurant.getTable(tableId)== nullptr||!restaurant.getTable(tableId)->isOpen()){
        std::cout<< "Error: Table does not exist or is not open"<<std::endl;
        error("Table does not exist or is not open");
    }
    else{
        std::cout<< "Table "+ std::to_string(tableId)+" was closed. Bill "+std::to_string(restaurant.getTable(tableId)->getBill())+"NIS"<<std::endl;
        restaurant.getTable(tableId)->closeTable();
        complete();
    }
    restaurant.setActionsLog(this);
};

//Return string represents CLose
std::string Close::toString() const{
    std::string output_string = "close "+ std::to_string(tableId);
    if (this->getStatus() == COMPLETED)
        output_string = output_string + " " + "Completed";
    else
        output_string = output_string + " " + "Error: " + this->getErrorMsg();
    return output_string;
};

//Clone close
Close* Close::clone() {
    Close* cloned_action = new Close(tableId);
    if (this->getStatus() == COMPLETED)
        cloned_action->complete();
    else if (this->getStatus() == ERROR)
        cloned_action->error(this->getErrorMsg());
    return cloned_action;
};

//Constructor of Close All
CloseAll:: CloseAll(): BaseAction(){};

//Activate Close All
void CloseAll::act(Restaurant &restaurant) {
    for (unsigned int i = 0; i < restaurant.getTables().size(); ++i) {
        if(restaurant.getTables()[i]->isOpen()){
            Close* close = new Close(i);
            close->act(restaurant);
        }
    }

};

//Return string represents Close All
std::string CloseAll::toString() const {
    return "closeall";
};

//Clone close all
CloseAll* CloseAll::clone() {
    CloseAll* cloned_action = new CloseAll();
    if (this->getStatus() == COMPLETED)
        cloned_action->complete();
    else if (this->getStatus() == ERROR)
        cloned_action->error(this->getErrorMsg());
    return cloned_action;
};

//Constructor of print menu
PrintMenu::PrintMenu(): BaseAction(){};

//Convert DishType to string represents the DishType
std::string PrintMenu::DishTypeToString(DishType type) {
    if (type == VEG)
        return "VEG";
    else if (type == ALC)
        return "ALC";
    else if (type == BVG)
        return "BVG";
    else
        return "SPC";
};

//Activate Print Menu and add to Actions log
void PrintMenu::act(Restaurant &restaurant) {
    for (unsigned int i = 0; i < restaurant.getMenu().size() ; ++i) {
        std::cout << restaurant.getMenu()[i].getName() << " " << DishTypeToString(restaurant.getMenu()[i].getType()) << " " << restaurant.getMenu()[i].getPrice() << "NIS" << std::endl;
    }
    complete();
    restaurant.setActionsLog(this);
};

//Return string represents Print Menu
std::string PrintMenu::toString() const {
    std::string output_string = "menu";
    if (this->getStatus() == COMPLETED)
        output_string = output_string + " " + "Completed";
    else
        output_string = output_string + " " + "Error: " + this->getErrorMsg();
    return output_string;
}

//Clone Print Menu
PrintMenu* PrintMenu::clone() {
    PrintMenu* cloned_action = new PrintMenu();
    if (this->getStatus() == COMPLETED)
        cloned_action->complete();
    else if (this->getStatus() == ERROR)
        cloned_action->error(this->getErrorMsg());
    return cloned_action;
};

//Constructor of Print Table Status
PrintTableStatus::PrintTableStatus(int id): BaseAction(), tableId(id) {};

//Activate Print Table Status and add to Actions Log
void PrintTableStatus::act(Restaurant &restaurant) {
    if (!restaurant.getTable(tableId)->isOpen()) { //If table is closed
        std::cout << "Table " << tableId << " status: closed" << std::endl;
    }
    else {
        std::cout << "Table " << tableId << " status: open" << std::endl;
        //Print customers
        std::cout << "Customers:" << std::endl;
        for (unsigned int i = 0; i < restaurant.getTable(tableId)->getCustomers().size(); ++i) {
            std::cout << restaurant.getTable(tableId)->getCustomers()[i]->getId() << " "
                      << restaurant.getTable(tableId)->getCustomers()[i]->getName() << std::endl;
        }
        //Print orders
        std::cout << "Orders:" << std::endl;
        for (unsigned int i = 0; i < restaurant.getTable(tableId)->getOrders().size(); ++i) {
            std::cout << restaurant.getTable(tableId)->getOrders()[i].second.getName() << " "
                      << restaurant.getTable(tableId)->getOrders()[i].second.getPrice() << "NIS "
                      << restaurant.getTable(tableId)->getOrders()[i].first << std::endl;
        }
        std::cout << "Current Bill: " << restaurant.getTable(tableId)->getBill() << "NIS" << std::endl;
    }
    complete();
    restaurant.setActionsLog(this);
};

//Return string represents Print Table Status
std::string PrintTableStatus::toString() const {
    std::string output_string = "status "+ std::to_string(tableId);
    if (this->getStatus() == COMPLETED)
        output_string = output_string + " " + "Completed";
    else
        output_string = output_string + " " + "Error: " + this->getErrorMsg();
    return output_string;
};

//Clone Print Table Status
PrintTableStatus* PrintTableStatus::clone() {
    PrintTableStatus* cloned_action = new PrintTableStatus(tableId);
    if (this->getStatus() == COMPLETED)
        cloned_action->complete();
    else if (this->getStatus() == ERROR)
        cloned_action->error(this->getErrorMsg());
    return cloned_action;
};

//Constructor of Print Actions Log
PrintActionsLog:: PrintActionsLog(): BaseAction(){};

//Activate Prints Actions Log and add to Actions Log
void PrintActionsLog::act(Restaurant &restaurant) {
    for (unsigned int i = 0; i < restaurant.getActionsLog().size() ; ++i) {
        std::cout <<  restaurant.getActionsLog()[i]->toString() << std::endl;
    }
    complete();
    restaurant.setActionsLog(this);
};

//Return string represents Print Actions Log
std::string PrintActionsLog::toString() const {
    std::string output_string = "log";
    if (this->getStatus() == COMPLETED)
        output_string = output_string + " " + "Completed";
    else
        output_string = output_string + " " + "Error: " + this->getErrorMsg();
    return output_string;
};

//Clone Print Actions Log
PrintActionsLog* PrintActionsLog::clone() {
    PrintActionsLog* cloned_action = new PrintActionsLog();
    if (this->getStatus() == COMPLETED)
        cloned_action->complete();
    else if (this->getStatus() == ERROR)
        cloned_action->error(this->getErrorMsg());
    return cloned_action;
};

//Constructor of Backup restaurant
BackupRestaurant::BackupRestaurant(): BaseAction() {};

//Activate Backup Restaurant and add to Actions Log
void BackupRestaurant::act(Restaurant &restaurant) {
    if (!restaurant.HasBackup()){
        restaurant.HasBackup(true);
        backup = new Restaurant(restaurant);
    }
    else{
        *backup = restaurant;
    }

    complete();
    restaurant.setActionsLog(this);
};

//Return string represents Backup Restaurant
std::string BackupRestaurant::toString() const {
    std::string output_string = "backup";
    if (this->getStatus() == COMPLETED)
        output_string = output_string + " " + "Completed";
    else
        output_string = output_string + " " + "Error: " + this->getErrorMsg();
    return output_string;
};

//Clone Backup Restaurant
BackupRestaurant* BackupRestaurant::clone() {
    BackupRestaurant* cloned_action = new BackupRestaurant();
    if (this->getStatus() == COMPLETED)
        cloned_action->complete();
    else if (this->getStatus() == ERROR)
        cloned_action->error(this->getErrorMsg());
    return cloned_action;
};

//Constructor of Restore restaurant
RestoreResturant::RestoreResturant(): BaseAction() {};

//Activate Restore restaurant and add to Actions Log
void RestoreResturant::act(Restaurant &restaurant) {
    if(backup == nullptr){
        error("No backup available");
        std::cout << "Error: No backup available" << std::endl;
        restaurant.setActionsLog(this);
    }
    else{
        restaurant = *backup;
        complete();
        restaurant.setActionsLog(this);
    }
};

//Return string represents Restore Restaurant
std::string RestoreResturant::toString() const {
    std::string output_string = "restore";
    if (this->getStatus() == COMPLETED)
        output_string = output_string + " " + "Completed";
    else
        output_string = output_string + " " + "Error: " + this->getErrorMsg();
    return output_string;
};

//Clone Restore Restaurant
RestoreResturant* RestoreResturant::clone() {
    RestoreResturant* cloned_action = new RestoreResturant();
    if (this->getStatus() == COMPLETED)
        cloned_action->complete();
    else if (this->getStatus() == ERROR)
        cloned_action->error(this->getErrorMsg());
    return cloned_action;
};



