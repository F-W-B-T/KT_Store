#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <ctime>
#include <vector>//удобные массивы
#include <map>//типо словари 

class Product;
class Section;
class Warehouse;
class Seller;
class Shop;
class Check;

class Prod_info {
private:
	std::string fabricator; // производитель
	int serial_num;
	time_t warranty_date;
public:
	//============================Конструктор================================
	Prod_info(std::string fabricator, int serial_num, time_t warranty_date);
	//----------------------------Доступные_Методы---------------------------
	std::string getFabricator();
	int getSerialNumber();
	time_t getWarrantyDate();
};


class Supplier {
private:
	int supplier_id;
	std::string name;
	std::string address;
	std::vector<Product*> products; // Товары этого поставщика
public:
	//============================Конструктор================================
	Supplier(int id, std::string name, std::string addres);
	//----------------------------Доступные_Методы---------------------------
	void addProduct(Product* product);          //метод добавляет товар в список товаров постовщика
	std::vector<Product*> getProducts();
	std::string getSupplierinfo(); 				//метод форматирует инфу о поставщике в строкy
};

class Product {
private:
	int product_id;
	std::string name;
	float price;
	int quantity;
	std::string category;
	float dimensions;
	Prod_info* special_info;
	Supplier* supplier;
public:
	//============================Конструктор================================
	Product(int id, std::string name, float price, int quantity, std::string category, float dimensions, std::string fabricator, int serial_num, time_t warrantydate, Supplier* supplier);
	//============================Деструктор================================
	~Product();
	//----------------------------ДоступныеМетоды---------------------------
	void updatePrice(float newPrice);
	void updateQuantity(int amount);
	bool checkAvailability();

	bool isUnderWarranty();

	std::string getProd_Info();
	Supplier* getSupplier();
	Prod_info* getProductInfoObj();
	float getDimensions();
	std::string getCategory();

	float getPrice() const { return price; }
	std::string getName() const { return name; }
};

class Shelf {
private:
	int shelf_id;
	std::string number; //имя полки по сути
	float capacity; // в контексте полки, я думаю, можно рассматривать сколько всего доступно, а currentWeight как сколько не занятого места/веса осталось 
	float currentWeight;
	Section* section; 
	std::map<std::string, int> categoryQuantities; //словарь количества продуктов
	std::vector<Product*> products;
public:
	//============================Конструктор================================
	Shelf(int shelf_id, std::string number, float capacity, Section* section); //Section наверное лучше по ссылке передавать 
	~Shelf();
	//----------------------------Доступные_Методы---------------------------
	void addProduct(Product* product);
	void removeProduct(Product* product);
	void getProducts();
	float getFreeSpace() const { return capacity - currentWeight; }
	float getCurrentWeight() const { return currentWeight; }
	std::string getNumber() const { return number; }
	int getShelf_id() const { return shelf_id; }
	float getCapacity() const { return capacity; }
	void updateWeight();
	bool writeOffProduct(Product* product);

	int getProductCountByType(std::string categody);
	const std::vector<Product*>& getProd() const { return products; } // геттер
	

	//операторы сравнения для полки по свободному местоу
	bool operator==(const Shelf& other) const { return getFreeSpace() == other.getFreeSpace(); }
	bool operator<(const Shelf& other) const { return getFreeSpace() < other.getFreeSpace(); }
	bool operator>(const Shelf& other) const { return getFreeSpace() > other.getFreeSpace(); }
};

class Section { //для секции думаю можно добавить текущуюю заполненость чтобы постоянно не вызывать функцию calculateCurrentLoad();
private:
	int section_id;
	std::string name;
	float capacity;
	float currentLoad; 
	std::map<std::string, int> categoryQuantities; //словарь полок
	std::vector<Shelf*> shelves; // конкретные полки
	Warehouse* warehouse;
public:
	//============================Конструктор================================
	Section(int section_id, std::string name, float capacity, Warehouse* warehouse);
	~Section();
	//----------------------------Доступные_Методы---------------------------
	void addShelf(std::string name, float capacity);
	void removeShelf(Shelf* shelf);
	void getShelves();
	void calculateCurrentLoad();

	void updateCategoryStatistics(); // обновляет статистику по всем полкам
	int getCategoryCount(const std::string& category); // количество товаров категории в секции

	bool hasFreeSpace() { return currentLoad < capacity; };
	void getSectionInfo();
	//======Геттреы========
	int getSectionId() const { return section_id; }
	std::string getName() const { return name; }
	float getCapacity() const { return capacity; }
	float getCurrentLoad() const { return currentLoad; }
	const std::map<std::string, int>& getCategoryQuantities() const { return categoryQuantities; }
	const std::vector<Shelf*>& getShelvesList() const { return shelves; } // ✅ новый метод //метод для получения чего-то
};

class Warehouse {
private:
	int warehouse_id;
	float totalCapacity;
	std::vector<Section*> sections; // ветор секций
	std::map<std::string, int> categoryQuantities; //словарь по категориям всего склада
	void updateCapacity();// на данный момент не реализованно 
public:
	//============================Конструктор/Деструктор================================
	Warehouse(int werehouse_id, float totalCapacity);
	~Warehouse();
	//----------------------------Доступные_Методы---------------------------
	void addSection(std::string name, float capacity);
	void removeSection(Section* section);
	void getSections();// на данный момент не реализованно 
	float getTotalLoad();
	float getFreeSpace() { return totalCapacity - getTotalLoad(); };

	void updateGlobalCategoryStatistics(); // обновляет статистику по всему складу
	int getGlobalCategoryCount(const std::string& category); // количество товаров категории на всем складе

	int getWarehouseId() const { return warehouse_id; }
	float getTotalCapacity() const { return totalCapacity; }
};


class Customer {
private:
	int customer_id;
	std::string address;
	int loyaltyPoints;
	float discountPercent;

public:
	//============================Конструктор================================
	Customer(int customer_id, std::string address); //предполагается что loyaltyPoints = 0
	//----------------------------Доступные_Методы---------------------------
	void updateLoyaltyPoints(int points);
	float applyDiscount();
	std::string getCustomerInfo();

	int getCustomerId() const { return customer_id; }
};

class Check {
private:
	const int check_id;
	const time_t dateTime; //наверное следует прописать const для установки времени
	float totalAmount;
	Seller* seller;//будет позже
	Customer* customer;
	std::map<Product*, int> products;// думаю можно не делать переменную quantities для продуктов, думаю можно обойтись словарем словарей где будет указан и продукт и его количество 
	std::string paymentMethod; //нах он нужен нам??? // бот говорит нужен :(
	Shop* shop; //будет позже

public:
	//============================Конструктор================================
	Check(int check_id, Seller* seller, Customer* customer, Shop* shop, std::string paymentMethod);
	//----------------------------Доступные_Методы---------------------------
	void calculateTotal();
	void addProduct(Product* product, int quantity);
	void removeProduct(Product* product);
	void applyDiscount(float discountPercent);
	void getCheckDeteils();
	void printCheck(); //бесполезная функция //Все еще так думаю

	time_t getDateTime() const { return dateTime; }//что это описано ниже если коротко то inline
	float getTotalAmount() const { return totalAmount; }
	int getCheckID() const { return check_id; }
};

class Seller {
private:
	int seller_id;
	std::string name;
	Shop* shop;
	std::vector<Check*> salesHistory;
public:
	//============================Конструктор================================
	Seller(int seller_id, std::string name, Shop* shop);
	//----------------------------Доступные_Методы---------------------------
	Check* createCheck(Customer* customer, std::string paymentMethod);// переделал метод создания чека
	void addToSalesHistory(Check* check);// метод для истории продаж 
	void getSalesHistory();// это потом опишу 
	float getTotalSales();
	void getSallerInfo();
	int generateCheckId();
	//Геттеры
	int getSellerId() const { return seller_id; }// const возврат по значению //т.е. только чтение // метод inline - обращение на прямую -
	std::string getName() const { return name; } // -(тип оптимизон такой, если в cpp описать то будет обращение к gfvznb)
};

class Shop {
private:
	int shop_id;
	std::string name;
	std::vector<Seller*> sellers;//массив продавцов
	Warehouse* warehouse;
public:
	//============================Конструктор================================
	Shop(int shop_id, std::string name);
	//----------------------------Доступные_Методы---------------------------
	//Управление продавцом
	void addSeller(Seller* seller);
	void removeSeller(Seller* seller);
	void getSellers();

	/* Получение общей статистики(через агрегацию)//ИИ
	//float getTotalShopSales(); // сумма продаж всех продавцов
	//int getTotalChecksCount(); // общее количество чеков
	*/

	void getShopInfo();

	int getShopId() const { return shop_id; }
	std::string getName() const { return name; }
	Warehouse* getWarehouse() { return warehouse; }
};
