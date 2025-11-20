#include "StoreEnt.h"

//============================Prod_info============================================
Prod_info::Prod_info(std::string fabricator, int serial_num, time_t warranty_date) : fabricator(fabricator), serial_num(serial_num), warranty_date(warranty_date)
{
}
std::string Prod_info::getFabricator()
{
	return fabricator;
}
int Prod_info::getSerialNumber()
{
	return serial_num;
}
time_t Prod_info::getWarrantyDate()
{
	return warranty_date;
}

//============================Supplier============================================
Supplier::Supplier(int id, std::string name, std::string addres) : supplier_id(id), name(name), address(addres)
{
}
void Supplier::addProduct(Product* product)
{
	products.push_back(product);
}

std::vector<Product*> Supplier::getProducts()
{
	return products;
}

std::string Supplier::getSupplierinfo()
{
	std::stringstream ss;
	ss << "Supplier ID: " << supplier_id
		<< ", Name: " << name
		<< ", Address: " << address
		<< ", Products: " << products.size();
	return ss.str();
	//Пример вывода строки: "Supplier ID: 1, Name: ABC Suppliers, Address: 123 Main St, Products: 5"
}
//============================Product============================================
Product::Product(int id, std::string name, float price, int quantity, std::string category, float dimensions, std::string fabricator, int serial_num, time_t warranty_date, Supplier* supplier)
	: product_id(id), name(name), price(price), quantity(quantity), category(category), dimensions(dimensions), special_info(new Prod_info(fabricator, serial_num, warranty_date)), supplier(supplier)
{
}
Product::~Product()
{
	delete special_info;
}
void Product::updatePrice(float newPrice)
{
	price = newPrice;
}

void Product::updateQuantity(int amount)
{
	quantity += amount;
	if (quantity < 0) quantity = 0;
}

bool Product::checkAvailability()
{
	return quantity > 0;
}

bool Product::isUnderWarranty()
{
	if (!special_info)
	{
		return false; // Если информации о продукте нет, гарантии нет
	}
	time_t currentTime = time(nullptr);
	time_t warrantyDate = special_info->getWarrantyDate();
	return warrantyDate > currentTime;
}

std::string Product::getProd_Info()
{
	std::stringstream ss;
	ss << "Product ID: " << product_id
		<< ", Name: " << name
		<< ", Price: $" << std::fixed << std::setprecision(2) << price
		<< ", Quantity: " << quantity
		<< ", Category: " << category;
	return ss.str();
}

Supplier* Product::getSupplier()
{
	return supplier;
}

Prod_info* Product::getProductInfoObj()
{
	return special_info;
}

float Product::getDimensions()
{
	return dimensions;
}

std::string Product::getCategory()
{
	return category;
}
//============================Shelf============================================
Shelf::Shelf(int shelf_id, std::string number, float capacity, Section* section) :
	shelf_id(shelf_id),
	number(number),
	capacity(capacity),
	section(section){}
Shelf::~Shelf()
{
	products.clear(); // указатели удаляются, но объекты Product остаются //по совету ИИ, сами пока не определились
}
void Shelf::addProduct(Product* product)
{
	if (product == nullptr) {
		return;
	}
	if (getFreeSpace() < product->getDimensions()) { //функции getWeight нет // но это пока
		return;
	}
	products.push_back(product);
	std::string category = product->getCategory();
	categoryQuantities[category]++;
}
void Shelf::removeProduct(Product* product)
{
	if (product == nullptr) {
		return;
	}

	// Удаляем из вектора//ИИ
	bool foundInVector = false;
	for (auto it = products.begin(); it != products.end(); ++it) {
		if (*it == product) {
			products.erase(it);
			foundInVector = true;
			break;
		}
	}
	if (!foundInVector) {
		std::cout << "Товар не найден на полке!" << std::endl;//ИИ проверки
		return;
	}
	// Обновляем словарь категорий
	std::string category = product->getCategory();
	if (categoryQuantities.count(category) > 0) {
		categoryQuantities[category]--;

		if (categoryQuantities[category] == 0) {
			categoryQuantities.erase(category);
			std::cout << "Категория '" << category << "' удалена с полки" << std::endl;
		}
	}
	updateWeight();
}
void Shelf::getProducts()
{
	std::cout << "=== ТОВАРЫ НА ПОЛКЕ '" << number << "' ===" << std::endl;

	if (products.empty()) {
		std::cout << "Полка пуста" << std::endl;
		return;
	}

	// Выводим конкретные товары
	std::cout << "Конкретные товары (" << products.size() << " шт.):" << std::endl;
	for (int i = 0; i < products.size(); i++) {
		Product* product = products[i];
		if (product != nullptr) {
			std::cout << i + 1 << ". " << product->getName()
				<< " | Категория: " << product->getCategory()
				<< " | Цена: " << product->getPrice() << " руб." << std::endl;
		}
	}

	// Выводим статистику по категориям
	std::cout << "--- Статистика по категориям ---" << std::endl;
	if (categoryQuantities.empty()) {
		std::cout << "Нет товаров по категориям" << std::endl;
	}
	else {
		for (const auto& pair : categoryQuantities) {
			std::cout << "• " << pair.first << ": " << pair.second << " шт." << std::endl;
		}
	}
}
void Shelf::updateWeight()
{
	currentWeight = 0.0f; // обнуляем перед расчетом
	// Временная логика: каждый товар = 1 единица веса
	for (Product* product : products) {
		if (product != nullptr) {
			currentWeight += product->getDimensions(); 
		}
	}
}

bool Shelf::writeOffProduct(Product* product)//ИИ
{
	if (product == nullptr) {
		return false;
	}

	// 1. Ищем товар на полке
	auto it = std::find(products.begin(), products.end(), product);
	if (it == products.end()) {
		std::cout << "Товар не найден на полке для списания!" << std::endl;
		return false;
	}

	// 2. Удаляем из вектора конкретных объектов
	products.erase(it);

	// 3. Обновляем словарь категорий
	std::string category = product->getCategory();
	if (categoryQuantities.find(category) != categoryQuantities.end()) {
		categoryQuantities[category]--;
		if (categoryQuantities[category] <= 0) {
			categoryQuantities.erase(category);
		}
	}

	// 4. Обновляем вес
	updateWeight();

	// 5. ? ОСОБЕННОСТЬ СПИСАНИЯ: удаляем объект из памяти
	delete product;

	std::cout << "??  ТОВАР СПИСАН: '" << product->getName()<< "' с полки '" << number << "'" << std::endl;
	return true;
}

int Shelf::getProductCountByType(std::string category)
{
	// Ищем категорию в словаре
	if (categoryQuantities.find(category) != categoryQuantities.end()) {
		return categoryQuantities[category];
	}

	// Если категории нет - то 0
	return 0;
}


//============================Section============================================
Section::Section(int section_id, std::string name, float capacity, Warehouse* warehouse) :
	section_id(section_id),
	name(name),
	capacity(capacity),
	warehouse(warehouse)
{}

Section::~Section()
{
	for (Shelf* shelf : shelves) {// Удаляем все полки
		delete shelf;
	}
}

void Section::addShelf(std::string name, float capacity)
{
	// Проверяем, нет ли полки с таким же именем
	for (Shelf* existingShelf : shelves) {
		if (existingShelf->getNumber() == name) {
			std::cout << "Ошибка: полка с именем '" << name << "' уже существует!" << std::endl;
			return;
		}
	}

	// Создаем новую полку
	int newShelfId = shelves.size() + 1;
	Shelf* newShelf = new Shelf(newShelfId, name, capacity, this);

	// Добавляем в вектор
	shelves.push_back(newShelf);

	// Обновляем статистику
	updateCategoryStatistics();
	calculateCurrentLoad();
}

void Section::removeShelf(Shelf* shelf)
{
	if (shelf == nullptr) {
		std::cout << "Ошибка: попытка удалить пустую полку!" << std::endl;
		return;
	}

	//Ищем полку в векторе
	auto it = std::find(shelves.begin(), shelves.end(), shelf);

	if (it == shelves.end()) {
		std::cout << "Ошибка: полка не найдена в секции '" << name << "'!" << std::endl;
		return;
	}

	//Сохраняем имя полки для сообщения
	std::string shelfName = shelf->getNumber();

	// Удаляем полку из вектора
	shelves.erase(it);

	//удаляем объект из памяти
	delete shelf;

	// Обновляем статистику
	updateCategoryStatistics();
	calculateCurrentLoad();
}

void Section::getShelves()//ИИ
{
	std::cout << "=== ПОЛКИ В СЕКЦИИ '" << name << "' ===" << std::endl;

	if (shelves.empty()) {
		std::cout << "В секции нет полок" << std::endl;
		return;
	}

	// Выводим информацию о каждой полке
	for (int i = 0; i < shelves.size(); i++) {
		Shelf* shelf = shelves[i];
		if (shelf != nullptr) {
			std::cout << i + 1 << ". Полка '" << shelf->getNumber() << "'" << std::endl;
			std::cout << "   ID: " << shelf->getShelf_id() << std::endl;
			std::cout << "   Вместимость: " << shelf->getCapacity() << std::endl;
			std::cout << "   Текущий вес: " << shelf->getCurrentWeight() << std::endl;
			std::cout << "   Свободно: " << shelf->getFreeSpace() << std::endl;
			//std::cout << "   Товаров: " << shelf->products.size() << " шт." << std::endl; //проблемная строчка // не понял зачем она?
			std::cout << "   --------------------" << std::endl;
		}
	}

	std::cout << "Всего полок: " << shelves.size() << std::endl;
}
void Section::calculateCurrentLoad()
{
	currentLoad = 0.0f; // обнуляем перед расчетом
	for (Shelf* shelf : shelves) {// Суммируем текущий вес всех полок в секции
		if (shelf != nullptr) {
			currentLoad += shelf->getCurrentWeight();
		}
	}
}
void Section::updateCategoryStatistics()
{
	categoryQuantities.clear();

	for (Shelf* shelf : shelves) {
		if (shelf != nullptr) {
			// Проходим по всем товарам на полке и считаем вручную
			for (Product* product : shelf->getProd()) {
				if (product != nullptr) {
					std::string category = product->getCategory();
					categoryQuantities[category]++;
				}
			}
		}
	}
}
int Section::getCategoryCount(const std::string& category)//ИИ
{
	// Проверяем пустую категорию
	if (category.empty()) {
		return 0;
	}
	// Проверяем, обновлена ли статистика
	if (categoryQuantities.empty()) {
		// Можно автоматически обновить статистику
		updateCategoryStatistics();
	}
	// Быстрый поиск в словаре
	return categoryQuantities[category];
	// Если категории нет - оператор [] создаст ее с значением 0
}
void Section::getSectionInfo()
{
	std::cout << "=== ИНФОРМАЦИЯ О СЕКЦИИ ===" << std::endl;
	std::cout << "ID: " << section_id << std::endl;
	std::cout << "Название: " << name << std::endl;
	std::cout << "Вместимость: " << capacity << std::endl;
	std::cout << "Текущая загрузка: " << currentLoad << std::endl;
	std::cout << "Свободно: " << (capacity - currentLoad) << std::endl;
	std::cout << "Заполненность: " << (currentLoad / capacity * 100) << "%" << std::endl;
	std::cout << "Количество полок: " << shelves.size() << std::endl;

	// Информация о складе
	if (warehouse != nullptr) {
		std::cout << "Принадлежит складу: ID " << warehouse->getWarehouseId() << std::endl;
	}
	else {
		std::cout << "Склад: не указан" << std::endl;
	}

	// Статистика по категориям
	std::cout << "--- Статистика по категориям ---" << std::endl;
	if (categoryQuantities.empty()) {
		std::cout << "Нет данных о категориях" << std::endl;
	}
	else {
		for (const auto& pair : categoryQuantities) {
			std::cout << "• " << pair.first << ": " << pair.second << " шт." << std::endl;
		}
	}

	std::cout << "================================" << std::endl;
}
//============================Warehouse============================================
Warehouse::Warehouse(int warehouse_id, float totalCapacity) :
	warehouse_id(warehouse_id),
	totalCapacity(totalCapacity){}

Warehouse::~Warehouse()
{
	for (Section* section : sections) {
		delete section;
	}
}

void Warehouse::addSection(std::string name, float capacity)
{
	// Проверяем, нет ли секции с таким же именем
	for (Section* existingSection : sections) {
		if (existingSection->getName() == name) {
			std::cout << "Ошибка: секция с именем '" << name << "' уже существует!" << std::endl;
			return;
		}
	}

	// Проверяем, есть ли свободное место на складе
	if (getFreeSpace() < capacity) {
		std::cout << "Ошибка: недостаточно места на складе для новой секции!" << std::endl;
		return;
	}

	// Создаем новую секцию
	int newSectionId = sections.size() + 1;
	Section* newSection = new Section(newSectionId, name, capacity, this);

	// Добавляем в вектор
	sections.push_back(newSection);

	// Обновляем глобальную статистику
	updateGlobalCategoryStatistics();
}

void Warehouse::removeSection(Section* section)//ИИ
{
	if (section == nullptr) {
		std::cout << "Ошибка: попытка удалить пустую секцию!" << std::endl;
		return;
	}

	// Ищем секцию в векторе
	auto it = std::find(sections.begin(), sections.end(), section);

	if (it == sections.end()) {
		std::cout << "Ошибка: секция не найдена на складе!" << std::endl;
		return;
	}

	// Проверяем, что секция пуста (нет полок с товарами)
	if (!section->getShelvesList().empty()) {
		std::cout << " ВНИМАНИЕ: В секции '" << section->getName()
			<< "' остались полки с товарами! Удаление отменено." << std::endl;
		return;
	}

	// Сохраняем имя секции для сообщения
	std::string sectionName = section->getName();

	// Удаляем секцию из вектора
	sections.erase(it);

	// ? ВАЖНО: удаляем объект секции из памяти
	delete section;

	// Обновляем глобальную статистику
	updateGlobalCategoryStatistics();

}

float Warehouse::getTotalLoad()
{
	{
		float totalLoad = 0.0f;

		// Суммируем загрузку всех секций
		for (Section* section : sections) {
			if (section != nullptr) {
				totalLoad += section->getCurrentLoad();
			}
		}

		return totalLoad;
	}
}

void Warehouse::updateGlobalCategoryStatistics()
{
	// Очищаем текущую статистику
	categoryQuantities.clear();

	// Проходим по всем секциям склада
	for (Section* section : sections) {
		if (section != nullptr) {
			// Получаем статистику категорий из секции
			const std::map<std::string, int>& sectionCategories = section->getCategoryQuantities();

			// Суммируем статистику по категориям
			for (const auto& pair : sectionCategories) {
				const std::string& category = pair.first;
				int quantity = pair.second;
				categoryQuantities[category] += quantity;
			}
		}
	}
}

int Warehouse::getGlobalCategoryCount(const std::string& category)
{
	// Проверяем пустую категорию
	if (category.empty()) {
		return 0;
	}

	// Проверяем, обновлена ли статистика
	if (categoryQuantities.empty()) {
		updateGlobalCategoryStatistics(); // автоматически обновляем если пусто
	}

	// Быстрый поиск в словаре
	auto it = categoryQuantities.find(category);
	if (it != categoryQuantities.end()) {
		return it->second;
	}

	return 0;
}

//============================Customer============================================
//============================Customer============================================
Customer::Customer(int customer_id, std::string address) : customer_id(customer_id), address(address), loyaltyPoints(0), discountPercent(0.0F)
{
}
void Customer::updateLoyaltyPoints(int points) {
	loyaltyPoints += points;
	// Обновляем скидку на основе loyaltyPoints
	if (loyaltyPoints >= 1000) {
		discountPercent = 10.0F;
	}
	else if (loyaltyPoints >= 500) {
		discountPercent = 5.0F;
	}
	else if (loyaltyPoints >= 100) {
		discountPercent = 2.0F;
	}
}
float Customer::applyDiscount() {
	return discountPercent;
}
std::string Customer::getCustomerInfo()
{
	std::stringstream ss;
	ss << "Customer ID: " << customer_id
		<< ", Address: " << address
		<< ", Loyalty Points: " << loyaltyPoints
		<< ", Discount: " << discountPercent << "%";
	return ss.str();
}
//============================Check============================================
Check::Check(int check_id, Seller* seller, Customer* customer, Shop* shop, std::string paymentMethod) : 
	check_id(check_id), 
	dateTime(time(nullptr)), 
	seller(seller), 
	customer(customer), 
	shop(shop), 
	paymentMethod(paymentMethod){}
void Check::calculateTotal()
{
	totalAmount = 0;
	for (const auto& pair : products) {
		Product* product = pair.first;    // ключ - указатель на товар//ИИ
		int quantity = pair.second;       // значение - количество//ИИ

		if (product != nullptr && quantity > 0) {
			totalAmount += product->getPrice() * quantity; 
		}
	}
	if (customer != nullptr) {
		float discountPercent = customer->applyDiscount();
		if (discountPercent > 0) {
			totalAmount *= (1 - discountPercent / 100.0f);
		}
	}
}
void Check::addProduct(Product* product, int quantity)
{
	if (product == nullptr || quantity <= 0) { //тут проверка на наличие товара 
		return;
	}
	if (products.find(product) != products.end()) { // тут проверка на наличие товара в чеке, если есть то увеличиваем его количество
		products[product] += quantity;
	}
	else {//если нет то добавляем 
		products[product] = quantity;
	}
	calculateTotal();//апдейт суммы чека
}
void Check::removeProduct(Product* product)
{
	if (product == nullptr) {
		return;
	}
	auto it = products.find(product);//ищем товар в словаре 
	if (it != products.end()) {
		products.erase(it);//удаляем его
		calculateTotal();//апдейт суммы чека
	}
}
void Check::applyDiscount(float discountPercent)
{
	if (customer != nullptr) {
		//тут нужно придумать формулу расчета с чками лояльности
	}
}
void Check::getCheckDeteils() //для работы нужны след функции: seller->getName()	customer->getCustomerId()	ну и геттер для цены товара product->price
{
	std::cout << "=== ДЕТАЛИ ЧЕКА ===" << std::endl;
	std::cout << "Чек №: " << check_id << std::endl;
	std::cout << "Дата: " << std::ctime(&dateTime);

	if (seller != nullptr) {
		std::cout << "Продавец: " << seller->getName() << std::endl;
	}

	if (customer != nullptr) {
		std::cout << "Покупатель: ID " << customer->getCustomerId() << std::endl;
	}

	std::cout << "Способ оплаты: " << paymentMethod << std::endl;
	std::cout << "--- Товары ---" << std::endl;

	// Выводим все товары
	for (const auto& pair : products) {
		Product* product = pair.first;
		int quantity = pair.second;

		if (product != nullptr) {
			std::cout << product->getName() << " x " << quantity
				<< " = " << (product->getPrice() * quantity) << " руб." << std::endl;
		}
	}

	std::cout << "---------------" << std::endl;
	std::cout << "ИТОГО: " << totalAmount << " руб." << std::endl;

	if (customer != nullptr) {
		float discount = customer->applyDiscount();
		if (discount > 0) {
			std::cout << "Скидка: " << discount << "%" << std::endl;
			std::cout << "К ОПЛАТЕ: " << totalAmount << " руб." << std::endl;
		}
	}
}
void Check::printCheck()
{
	//печать чека // можно добавить папку файлами txt всех чеков
}
//============================Seller============================================
Seller::Seller(int seller_id, std::string name, Shop* shop) :
	seller_id(seller_id),
	name(name),
	shop(shop){}
Check* Seller::createCheck(Customer* customer, std::string paymentMethod)
{
	if (customer == nullptr) {
		return nullptr; //проверка покупателя
	}
	if (shop == nullptr) {
		return nullptr;
	}
	Check* newCheck = new Check(
		//нужна функция для генерации ID 
		//для Check Seller Custumer 
		generateCheckId(), // НЕТ РЕАЛИЗАЦИИ!!!!!!
		//time(nullptr), //нужно переделать конструктор чека
		this, //тут поле указателя продавца // указываем текущего продавца
		customer,
		shop,
		paymentMethod
	);
	salesHistory.push_back(newCheck);//добавляем в конец истории продаж 
	return newCheck;
}
void Seller::addToSalesHistory(Check* check)
{
	if (check == nullptr) {
		return;
	}
	salesHistory.push_back(check);
}
void Seller::getSalesHistory()
{
	//тут будет вывод, наверное в консоль, хотя можно создать еще папку с файлами конкретного прадовца// по сути логи
}
float Seller::getTotalSales()
{
	float total = 0.0f;
	for (Check* check : salesHistory) {
		if (check != nullptr) {
			total += check->getTotalAmount();
		}
	}
	return total;
}
void Seller::getSallerInfo()
{
	std::cout << "=== ИНФОРМАЦИЯ О ПРОДАВЦЕ ===" << std::endl;
	std::cout << "ID: " << seller_id << std::endl;
	std::cout << "Имя: " << name << std::endl;
	std::cout << "Всего продаж: " << salesHistory.size() << " чеков" << std::endl;
	std::cout << "Общая выручка: " << getTotalSales() << " руб." << std::endl;

	// Если есть связь с магазином//ИИ
	if (shop != nullptr) {
		std::cout << "Магазин: " << shop->getName() << std::endl;
	}

	// Можно добавить средний чек//ИИ
	if (!salesHistory.empty()) {
		float averageCheck = getTotalSales() / salesHistory.size();
		std::cout << "Средний чек: " << averageCheck << " руб." << std::endl;
	}
}
int Seller::generateCheckId()
{
	static int nextCheckId = 1;
	return nextCheckId++;
}
//============================Shop============================================
Shop::Shop(int shop_id, std::string name) :
	shop_id(shop_id),
	name(name),
	warehouse(nullptr){}

void Shop::addSeller(Seller* seller)
{
	if (seller == nullptr) {
		return;
	}
	sellers.push_back(seller);
}

void Shop::removeSeller(Seller* seller)
{
	if (seller == nullptr) {
		return;
	}
	auto it = std::find(sellers.begin(), sellers.end(), seller);

	if (it != sellers.end()) {//удаляем 
		sellers.erase(it);
	}
}

void Shop::getSellers()//сделал ИИ
{
	std::cout << "=== ПРОДАВЦЫ МАГАЗИНА '" << name << "' ===" << std::endl;

	if (sellers.empty()) {
		std::cout << "В магазине нет продавцов" << std::endl;
		return;
	}

	// Выводим информацию о каждом продавце
	for (int i = 0; i < sellers.size(); i++) {
		Seller* seller = sellers[i];
		if (seller != nullptr) {
			std::cout << i + 1 << ". ID: " << seller->getSellerId()
				<< " | Имя: " << seller->getName() << std::endl;
		}
	}

	std::cout << "Всего продавцов: " << sellers.size() << std::endl;
}
void Shop::getShopInfo()//сделал ИИ
{
	std::cout << "=== ИНФОРМАЦИЯ О МАГАЗИНЕ ===" << std::endl;
	std::cout << "ID: " << shop_id << std::endl;
	std::cout << "Название: " << name << std::endl;
	std::cout << "Количество продавцов: " << sellers.size() << std::endl;

	// Информация о складе
	if (warehouse != nullptr) {
		std::cout << "Склад: есть" << std::endl;
		// Можно добавить информацию о заполненности склада, когда реализуем Warehouse
	}
	else {
		std::cout << "Склад: нет" << std::endl;
	}
}

