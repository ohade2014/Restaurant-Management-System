#ifndef CUSTOMER_H_
#define CUSTOMER_H_

#include <vector>
#include <string>
#include "Dish.h"

class Customer{
public:
    Customer(std::string c_name, int c_id);
    virtual ~Customer();
    virtual std::vector<int> order(const std::vector<Dish> &menu)=0;
    virtual std::string toString() const = 0;
    std::string getName() const;
    int getId() const;
    std:: vector<int> getOrders();
    virtual Customer* clone()=0;
private:
    const std::string name;
    const int id;
protected:
    std:: vector<int> orders;
};


class VegetarianCustomer : public Customer {
public:
	VegetarianCustomer(std::string name, int id);
	virtual ~VegetarianCustomer();
    std::vector<int> order(const std::vector<Dish> &menu);
    std::string toString() const;
	virtual VegetarianCustomer* clone();
private:
};


class CheapCustomer : public Customer {
public:
	CheapCustomer(std::string name, int id);
	virtual ~CheapCustomer();
    std::vector<int> order(const std::vector<Dish> &menu);
    std::string toString() const;
	virtual CheapCustomer* clone();
private:
    bool is_ordered;
};


class SpicyCustomer : public Customer {
public:
	SpicyCustomer(std::string name, int id);
	virtual ~SpicyCustomer();
    std::vector<int> order(const std::vector<Dish> &menu);
    std::string toString() const;
	virtual SpicyCustomer* clone();
private:
    bool is_ordered;
    int min_bvg_index;
};


class AlchoholicCustomer : public Customer {
public:
	AlchoholicCustomer(std::string name, int id);
	virtual ~AlchoholicCustomer();
    std::vector<int> order(const std::vector<Dish> &menu);
    std::string toString() const;
	virtual AlchoholicCustomer* clone();
private:
    std::vector<int> alcoholic_dishes;
    bool is_ordered;
};


#endif