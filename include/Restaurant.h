#ifndef RESTAURANT_H_
#define RESTAURANT_H_

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include "Dish.h"
#include "Table.h"
#include "Action.h"


class Restaurant{		
public:
	Restaurant();
    Restaurant(const std::string &configFilePath);
    virtual ~Restaurant();
    Restaurant(Restaurant &other);
	Restaurant& operator=(const Restaurant& other);
	Restaurant(Restaurant&& other);
	Restaurant& operator=(const Restaurant&& other);
	void start();
    Customer* buildCustomerByType (std::string name , std::string type , int id);
    int getNumOfTables() const;
    Table* getTable(int ind);
	const std::vector<BaseAction*>& getActionsLog() const; // Return a reference to the history of actions
    std::vector<Dish>& getMenu();
    std::vector<Table*> getTables();
    DishType StringToDishType(std::string string_);
    void setActionsLog(BaseAction* action);
    bool HasBackup();
    void HasBackup(bool backup);
private:
    bool open;
    std::vector<Table*> tables;
    std::vector<Dish> menu;
    std::vector<BaseAction*> actionsLog;
    bool has_Backup;
};

#endif