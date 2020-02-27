#ifndef TABLE_H_
#define TABLE_H_

#include <vector>
#include "Customer.h"
#include "Dish.h"

typedef std::pair<int, Dish> OrderPair;

class Table{
public:
    Table(int t_capacity);
    virtual ~Table();
    Table (Table &other);
    Table& operator=(const Table& other);
    Table(Table&&other);
    Table& operator=(Table&& other);
    int getCapacity() const;
    int  getAvailableSeats()const;
    void addCustomer(Customer* customer);
    void removeCustomer(int id);
    Customer* getCustomer(int id);
    std::vector<Customer*>& getCustomers();
    std:: vector<OrderPair>& getOrders();
    void order(const std::vector<Dish> &menu);
    void openTable();
    void closeTable();
    int getBill();
    void AddToTotalPrice(int add);
    bool isOpen();
private:
    int capacity;
    bool open;
    int total_price;
    int available_seats;
    std::vector<Customer*> customersList;
    std::vector<OrderPair> orderList; //A list of pairs for each order in a table - (customer_id, Dish)
};


#endif