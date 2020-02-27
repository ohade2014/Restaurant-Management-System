//
// Created by Yuval Eisner & Ohad Eliyahou on 11/10/18.
//

#include "../include/Restaurant.h"
#include "../include/Dish.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

//Constructor of Restaurant
Restaurant::Restaurant(): open(true),tables(),menu(),actionsLog(),has_Backup(false) {};

//Constructor by Config File
Restaurant::Restaurant(const std::string &configFilePath):open(true),tables(), menu(), actionsLog(),has_Backup(false) {
    std::string line;
    ifstream myfile;
    std::string token;
    myfile.open(configFilePath);
    while (getline(myfile, line)){ //Start read config file
        if (line == "#number of tables") { //read number of tables
            getline(myfile, line);
            getline(myfile, line);
        }
        if (line == "#tables description") { //read tables description and update tables list
            getline(myfile, line);
            while (line == "") // Skip empty lines
                getline(myfile, line);
            istringstream is(line);
            while (getline(is, token, ',')) {
                Table *table = new Table(std::stoi(token));
                this->tables.push_back(table);
            }
            getline(myfile, line);
        }
        if (line == "#Menu") { //read menu and update menu list
            int i = 0;
            while (getline(myfile, line)) {
                if (line != ""){
                    istringstream is(line);
                    getline(is, token, ',');
                    std::string name = token;
                    getline(is, token, ',');
                    DishType type = StringToDishType(token);
                    getline(is , token , ',');
                    int price = std::stoi(token);
                    Dish* dish = new Dish (i , name , price , type);
                    i++;
                    menu.push_back(*dish);
                    delete dish;
                }
            }
        }
    }
    myfile.close();
};

//Distructor Of restaurant
Restaurant::~Restaurant() {
    for (unsigned int i = 0; i < tables.size(); ++i) {
        delete tables[i];
    }
    for (unsigned int j = 0; j < actionsLog.size(); ++j) {
        delete actionsLog[j];
    }
    tables.clear();
    actionsLog.clear();
    menu.clear();
};

//Copy Constructor
Restaurant::Restaurant(Restaurant &other):open(other.open), tables(), menu(other.menu), actionsLog(),has_Backup(other.has_Backup) {
    tables = vector<Table*> (other.tables.size());
    actionsLog = vector<BaseAction*> (other.actionsLog.size());
    for (unsigned int i = 0; i < other.tables.size(); ++i) {
        tables[i] = new Table (*other.tables[i]);
    }
    for (unsigned int j = 0; j < other.actionsLog.size() ; ++j) {
        actionsLog[j] = other.actionsLog[j]->clone();
    }
};

//Copy Assignment operator
Restaurant& Restaurant::operator=(const Restaurant& other) {
    if (this != &other) {
        open = other.open;
        has_Backup = other.has_Backup;
        // Replace menu info
        menu.clear();
        for (unsigned int m = 0; m < other.menu.size(); ++m) {
            menu.push_back(other.menu[m]);
        }
        //Delete existing tables and actions log
        for (unsigned int i = 0; i < tables.size(); ++i) {
            delete tables[i];
        }
        for (unsigned int j = 0; j < actionsLog.size(); ++j) {
            delete actionsLog[j];
        }
        tables.clear();
        actionsLog.clear();
        //New tables and Actions log update
        for (unsigned int k = 0; k < other.tables.size(); ++k) {
            tables.push_back(new Table(*other.tables[k]));
        }
        for (unsigned int l = 0; l < other.actionsLog.size(); ++l) {
            actionsLog.push_back(other.actionsLog[l]->clone());
        }
    }
    return *this;
};

//Move Constructor
Restaurant::Restaurant(Restaurant&& other): open(other.open), tables(), menu(other.menu), actionsLog(),has_Backup(false) {
    tables = vector<Table*> (other.tables.size());
    actionsLog = vector<BaseAction*> (other.actionsLog.size());
    //move customers
    for (unsigned int i = 0; i < other.tables.size(); ++i) {
        tables[i] = other.tables[i];
        other.tables[i] = nullptr;
    }
    //move actions log
    for (unsigned int j = 0; j < other.actionsLog.size() ; ++j) {
        actionsLog[j] = other.actionsLog[j]->clone();
        other.actionsLog[j] = nullptr;
    }
};

//Move Assignment operator
Restaurant& Restaurant::operator=(const Restaurant&& other) {
    if (this != &other){
        open = other.open;
        has_Backup = other.has_Backup;
        //Replace menu info
        menu.clear();
        for (unsigned int m = 0; m < other.menu.size(); ++m) {
            menu.push_back(other.menu[m]);
        }
        //Delete existing tables and actions log
        for (unsigned int i = 0; i < tables.size(); ++i) {
            delete tables[i];
        }
        for (unsigned int j = 0; j < actionsLog.size(); ++j) {
            delete actionsLog[j];
        }
        tables.clear();
        actionsLog.clear();
        //Move tables and actions log
        for (unsigned int k = 0; k < other.tables.size(); ++k) {
            tables.push_back(other.tables[k]);
            delete other.tables[k];
        }
        for (unsigned int l = 0; l < other.actionsLog.size(); ++l) {
            actionsLog.push_back(other.actionsLog[l]);
            delete other.actionsLog[l];
        }
    }
    return *this;
};

//Return true if this restaurant has backup, else return false
bool Restaurant::HasBackup() {
    return has_Backup;
};

//Change the value of has_Backup to be the input boolean value
void Restaurant::HasBackup(bool backup) {
    has_Backup = backup;
};

//Start Restaurant
void Restaurant::start() {
    open = true;
    std::cout << "Restaurant is now open!" << std::endl;
    std::string input;
    std::getline(cin,input);
    int customerId = 0;
    while (input.substr(0,input.find(" ")) != "closeall"){
        if (input.substr(0,input.find(" ")) == "open"){ //Activate Open Table
            std:: vector<Customer*> customersList;
            int tableId;
            std::string name;
            std::string type;
            input = input.substr(input.find(" ")+1);
            tableId = std::stoi(input.substr(0,input.find(" ")));
            input = input.substr(input.find(" ")+1);
            if (input.size() > 1) {
                while (input != "") {
                    //Create customers by the input information
                    name = input.substr(0, input.find(","));
                    input = input.substr(input.find(",") + 1);
                    type = input.substr(0, input.find(" "));
                    Customer *customer = buildCustomerByType(name, type, customerId);
                    customerId++;
                    customersList.push_back(customer);
                    if (getTable(tableId) != nullptr && !getTable(tableId)->isOpen())
                        tables[tableId]->addCustomer(customer);
                    input = input.substr(input.find(" ") + 1); //next customer
                    if (input.size() == 3) { //All customers have been read
                        input = "";
                    }
                }
            }
            OpenTable* open = new OpenTable(tableId,  customersList);
            open->act(*this);
            if(open->getStatus() == ERROR){ //delete customers if open did not happened
                for (unsigned int i = 0; i < customersList.size(); ++i) {
                    delete customersList[i];
                }
            }
        }
        else if (input.substr(0,input.find(" ")) == "order") { //Activate Order
            if(menu.size() != 0) {
                int tableId = std::stoi(input.substr(input.find(" ") + 1));
                Order *order = new Order(tableId);
                order->act(*this);
            }
        }
        else if (input.substr(0,input.find(" ")) == "move") { //Activate Move Customer
            int srcId = std::stoi(input.substr(input.find(" ")+1));
            input = input.substr(input.find(" ")+1);
            int dstId = std::stoi(input.substr(input.find(" ")+1));
            input = input.substr(input.find(" ")+1);
            int customerId = std::stoi(input.substr(input.find(" ")+1));
            MoveCustomer* move = new MoveCustomer(srcId, dstId , customerId);
            move->act(*this);
        }
        else if(input.substr(0,input.find(" ")) == "close"){ //Activate close
            int tableId = std::stoi(input.substr(input.find(" ")+1));
            Close* close = new Close(tableId);
            close->act(*this);
        }
        else if(input.substr(0,input.find(" ")) == "menu"){ //Activate Print Menu
            PrintMenu* menu = new PrintMenu();
            menu->act(*this);
        }
        else if(input.substr(0,input.find(" ")) == "status"){ //Activate Print Table Status
            int tableId = std::stoi(input.substr(input.find(" ")+1));
            PrintTableStatus* status = new PrintTableStatus(tableId);
            status->act(*this);
        }
        else if(input.substr(0,input.find(" ")) == "log"){ //Activate Print Actions Log
            PrintActionsLog* log = new PrintActionsLog();
            log->act(*this);
        }
        else if(input.substr(0,input.find(" ")) == "backup"){ //Activate Backup Restaurant
                BackupRestaurant* backup1 = new BackupRestaurant();
                backup1->act(*this);
        }
        else if(input.substr(0,input.find(" ")) == "restore"){ //Activate Restore Restaurant
            RestoreResturant* restore = new RestoreResturant();
            restore->act(*this);
        }
        std::getline(cin,input);
    }
    //Activate Close All
    CloseAll* closeAll = new CloseAll();
    closeAll->act(*this);
    delete closeAll;
};

// Create customer by its type
Customer* Restaurant::buildCustomerByType (std::string name , std::string type , int id){
    Customer* customer;
    if (type == "veg"){
        customer = new VegetarianCustomer (name, id);
    }
    else if (type == "chp"){
        customer = new CheapCustomer(name, id);
    }
    else if (type == "alc"){
        customer = new AlchoholicCustomer(name, id);
    }
    else if (type == "spc"){
        customer = new SpicyCustomer(name, id);
    }
    return customer;
};

//Return Number Of Tables
int Restaurant::getNumOfTables() const {
    return tables.size();
};

//Return table with the index inserted
Table* Restaurant::getTable(int ind){
    return tables[ind];
};

// Return Actions Log vector
const std::vector<BaseAction*>& Restaurant::getActionsLog() const {
    return actionsLog;
};

//Return menu vector
std::vector<Dish>& Restaurant::getMenu(){
    return menu;
};

//Return tables vector
std::vector<Table*> Restaurant::getTables(){
    return tables;
};

//Convert String to DishType
DishType Restaurant::StringToDishType(std::string string_){
    if (string_ == "ALC")
        return ALC;
    else if (string_ == "BVG")
        return BVG;
    else if (string_ == "VEG")
        return VEG;
    else
        return SPC;
};

//Add action to Actions Log
void Restaurant::setActionsLog(BaseAction* action) {
    actionsLog.push_back(action);
};