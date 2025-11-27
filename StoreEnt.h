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
class IDGenerator;

class IDGenerator { //чтобы использовать в конструкторах, пример : product_id(IDGenerator::genProductID())
public:
    static void init() {
        std::srand(std::time(nullptr));
    }

    static int genProductID()   { return std::rand(); }
    static int genSupplierID()  { return std::rand(); }
    static int genShelfID()     { return std::rand(); }
    static int genSectionID()   { return std::rand(); }
    static int genWarehouseID() { return std::rand(); }
    static int genCustomerID()  { return std::rand(); }
    static int genSellerID()    { return std::rand(); }
    static int genShopID()      { return std::rand(); }

    // чек тоже генератор рандомный
    static int genCheckID()     { return std::rand(); }
};

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

//класс нужно переписать полностью!!!! по образцу prod_info так как этот класс мы используем для спец информации как и prod_info
class SupplierInfo {
private:
	std::string name;
	std::string address;
public:
	//============================Конструктор================================
	SupplierInfo(std::string name, std::string addres);
	//----------------------------Доступные_Методы---------------------------
	std::string getName() const { return name; }
    std::string getAddress() const { return address; }
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
	SupplierInfo* manufacturer;
public:
	//============================Конструктор================================
	Product(
		std::string name, 
		float price, int quantity, 
		std::string category, 
		float dimensions, 
		std::string fabricator, 
		int serial_num, 
		time_t warrantydate, 
		SupplierInfo* manufacturer,
		std::string address
	);
	//============================Деструктор================================
	~Product();
	//----------------------------ДоступныеМетоды---------------------------
	void updatePrice(float newPrice);
	void updateQuantity(int amount);
	bool checkAvailability();

	bool isUnderWarranty();

	std::string getProd_Info();
	SupplierInfo* getSupplier();
	Prod_info* getProductInfoObj();
	float getDimensions();
	std::string getCategory();
	int getQuantity() const { return quantity; }

	float getPrice() const { return price; }
	std::string getName() const { return name; }
};

class Shelf {
private:
	int shelf_id;
	std::string number;
	float capacity;
	float currentWeight;
	Section* section;
	std::map<std::string, int> categoryQuantities;
	std::vector<Product*> products;

public:
	//============================Конструктор================================
	Shelf(
		std::string number, 
		float capacity, 
		Section* section);
	~Shelf();

	//----------------------------Доступные_Методы---------------------------
	void addProduct(Product* product);
	void addProduct(Product* product, int quantity); // перегрузка
	void removeProduct(Product* product);
	bool removeProduct(Product* product, int quantity); // перегруженная версия
	void getProductsInfo();
	float getFreeSpace() const { return capacity - currentWeight; }
	float getCurrentWeight() const { return currentWeight; }
	std::string getNumber() const { return number; }
	int getShelf_id() const { return shelf_id; }
	float getCapacity() const { return capacity; }
	void updateWeight();
	bool writeOffProduct(Product* product);
	int getProductCountByType(std::string category);
	const std::vector<Product*>& getProducts() const { return products; }

	// Новые методы для работы с сигналами
	void setupSignalConnections();
	void onProductAddedToCheck(Product* product, int quantity);
	bool containsProduct(Product* product) const;
	int getProductQuantity(Product* product) const;

	//операторы сравнения для полки по свободному местоу
	bool operator==(const Shelf& other) const { return getFreeSpace() == other.getFreeSpace(); }
	bool operator<(const Shelf& other) const { return getFreeSpace() < other.getFreeSpace(); }
	bool operator>(const Shelf& other) const { return getFreeSpace() > other.getFreeSpace(); }
};

class Section {
private:
	int section_id;
	std::string name;
	float capacity;
	float currentLoad;
	std::map<std::string, int> categoryQuantities;
	std::vector<Shelf*> shelves;
	Warehouse* warehouse;

public:
	//============================Конструктор================================
	Section(
		std::string name, 
		float capacity, 
		Warehouse* warehouse
	);
	~Section();

	//----------------------------Доступные_Методы---------------------------
	void addShelf(std::string name, float capacity);
	void removeShelf(Shelf* shelf);
	void getShelves();
	void calculateCurrentLoad();
	void updateCategoryStatistics();
	int getCategoryCount(const std::string& category);
	bool hasFreeSpace() { return currentLoad < capacity; };
	void getSectionInfo();

	// Новый метод для работы с сигналами
	bool removeProductFromShelf(Product* product, int quantity);

	//======Геттреы========
	int getSectionId() const { return section_id; }
	std::string getName() const { return name; }
	float getCapacity() const { return capacity; }
	float getCurrentLoad() const { return currentLoad; }
	const std::map<std::string, int>& getCategoryQuantities() const { return categoryQuantities; }
	const std::vector<Shelf*>& getShelvesList() const { return shelves; }
};

class Warehouse {
private:
	int warehouse_id;
	float totalCapacity;
	std::vector<Section*> sections;
	std::map<std::string, int> categoryQuantities;
	void updateCapacity(); // на данный момент не реализованно 

public:
	//============================Конструктор/Деструктор================================
	Warehouse(
		float totalCapacity
	);
	~Warehouse();

	//----------------------------Доступные_Методы---------------------------
	void addSection(std::string name, float capacity);
	void removeSection(Section* section);
	void getSections(); // на данный момент не реализованно 
	float getTotalLoad();
	float getFreeSpace() { return totalCapacity - getTotalLoad(); };
	void updateGlobalCategoryStatistics();
	int getGlobalCategoryCount(const std::string& category);

	// Новые методы для работы с сигналами
	void setupSignalConnections();
	void onProductAddedToCheck(Product* product, int quantity);
	void onProductRemovedFromCheck(Product* product, int quantity);
	bool removeProductFromStock(Product* product, int quantity);
	bool returnProductToStock(Product* product, int quantity);

	//======Геттреы========
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
	Customer(std::string address); //предполагается что loyaltyPoints = 0
	//----------------------------Доступные_Методы---------------------------
	void updateLoyaltyPoints(int points);
	float applyDiscount();
	std::string getCustomerInfo();

	int getCustomerId() const { return customer_id; }
};

class Check {
private:
	const int check_id;
	const time_t dateTime;
	float totalAmount;
	Seller* seller;
	Customer* customer;
	std::map<Product*, int> products;
	std::string paymentMethod;
	Shop* shop;
	bool isPaid; // Новое поле для отслеживания статуса оплаты

public:
	//============================Конструктор================================
	Check(Seller* seller, Customer* customer, Shop* shop, std::string paymentMethod);
	~Check(); // Добавлен деструктор

	//----------------------------Доступные_Методы---------------------------
	void calculateTotal();
	void addProduct(Product* product, int quantity);
	void removeProduct(Product* product);
	void applyDiscount(float discountPercent);
	void getCheckDeteils();
	void printCheck();

	// Новый метод для возврата товаров на склад
	void returnProductsToStock();

	//======Геттреы========
	time_t getDateTime() const { return dateTime; }
	float getTotalAmount() const { return totalAmount; }
	int getCheckID() const { return check_id; }
	bool getIsPaid() const { return isPaid; } // Новый геттер
};

class Seller {
private:
	int seller_id;
	std::string name;
	Shop* shop;
	std::vector<Check*> salesHistory;
public:
	//============================Конструктор================================
	Seller(std::string name, Shop* shop);
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
	std::vector<Seller*> sellers;
	Warehouse* warehouse;

public:
	//============================Конструктор================================
	Shop(std::string name);

	//----------------------------Доступные_Методы---------------------------
	// Управление продавцами
	void addSeller(Seller* seller);
	void removeSeller(Seller* seller);
	void getSellers();

	// Управление складом
	void setWarehouse(Warehouse* warehouse);
	bool removeProductFromWarehouse(Product* product, int quantity);
	void returnProductToWarehouse(Product* product, int quantity);

	// Статистика и информация
	void getShopInfo();
	float getTotalShopSales(); // сумма продаж всех продавцов
	int getTotalChecksCount(); // общее количество чеков

	//======Геттеры========
	int getShopId() const { return shop_id; }
	std::string getName() const { return name; }
	Warehouse* getWarehouse() const { return warehouse; }
	const std::vector<Seller*>& getSellersList() const { return sellers; }
};
