#include "StoreEnt.h"
#include "Signals.h" 

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
SupplierInfo::SupplierInfo( std::string name, std::string addres) 
: 
name(name), 
address(addres)
{
}

//============================Product============================================
Product::Product(
	std::string name, 
	float price, 
	int quantity, 
	std::string category, 
	float dimensions, 
	std::string fabricator, 
	int serial_num, 
	time_t warranty_date, 
	SupplierInfo* manufacturer,
	std::string address)
	: 
	product_id(IDGenerator::genProductID()), 
	name(name), 
	price(price), 
	quantity(quantity), 
	category(category), 
	dimensions(dimensions), 
	special_info(new Prod_info(fabricator, serial_num, warranty_date)), 
	manufacturer(new SupplierInfo(name, address))
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

SupplierInfo* Product::getSupplier()
{
	return manufacturer;
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
int Product::getQuantity() const {
	return quantity;
}

//============================Shelf============================================
Shelf::Shelf(
	std::string number, 
	float capacity, 
	Section* section) 
	:
	shelf_id(IDGenerator::genShelfID()),
	number(number),
	capacity(capacity),
	section(section)
{
	// Подключаем обработчики сигналов
	setupSignalConnections();
}

void Shelf::setupSignalConnections()
{
	// Обработчик добавления товара в чек
	GlobalSignals::productAddedToCheck().connect(
		[this](Product* product, int quantity) {
			this->onProductAddedToCheck(product, quantity);
		}
	);
}

// Обработчик сигнала добавления товара в чек
void Shelf::onProductAddedToCheck(Product* product, int quantity)
{
	if (product == nullptr || quantity <= 0) {
		return;
	}

	// Проверяем, есть ли товар на этой полке
	if (!containsProduct(product)) {
		return;
	}

	// Проверяем, достаточно ли товара на полке
	if (getProductQuantity(product) < quantity) {
		std::cout << "Сигнал: Недостаточно товара '" << product->getName()
			<< "' на полке '" << number << "'! Доступно: "
			<< getProductQuantity(product) << ", требуется: " << quantity << std::endl;
		return;
	}

	// Удаляем товар с полки
	removeProduct(product, quantity);
	std::cout << "Сигнал: Товар '" << product->getName()
		<< "' (количество: " << quantity << ") удален с полки '" << number << "'" << std::endl;
}

Shelf::~Shelf()
{
	products.clear();
}

void Shelf::addProduct(Product* product)
{
	if (product == nullptr) {
		return;
	}
	if (getFreeSpace() < product->getDimensions()) {
		return;
	}
	products.push_back(product);
	std::string category = product->getCategory();
	categoryQuantities[category]++;
}
void Shelf::addProduct(Product* product, int quantity)
{
    if (product == nullptr || quantity <= 0) {
        return;
    }

    // Проверяем, достаточно ли свободного места
    float totalNeededSpace = product->getDimensions() * quantity;

    if (getFreeSpace() < totalNeededSpace) {
        std::cout << "Недостаточно места на полке '" << number
            << "'. Нужно: " << totalNeededSpace
            << ", доступно: " << getFreeSpace() << std::endl;
        return;
    }

    // Добавляем нужное количество копий
    for (int i = 0; i < quantity; i++) {
        products.push_back(product);
    }

    // Обновляем статистику по категориям
    std::string category = product->getCategory();
    categoryQuantities[category] += quantity;

    // Обновляем вес полки
    updateWeight();
}


// Перегруженный метод removeProduct для удаления определенного количества
bool Shelf::removeProduct(Product* product, int quantity)
{
	if (product == nullptr || quantity <= 0) {
		return false;
	}

	// Ищем товар на полке
	bool found = false;
	int removedCount = 0;

	// Удаляем указанное количество экземпляров товара
	for (auto it = products.begin(); it != products.end() && removedCount < quantity; ) {
		if (*it == product) {
			it = products.erase(it);
			removedCount++;
			found = true;
		}
		else {
			++it;
		}
	}

	if (!found) {
		std::cout << "Товар не найден на полке!" << std::endl;
		return false;
	}

	// Обновляем словарь категорий
	std::string category = product->getCategory();
	if (categoryQuantities.count(category) > 0) {
		categoryQuantities[category] -= removedCount;

		if (categoryQuantities[category] <= 0) {
			categoryQuantities.erase(category);
		}
	}

	updateWeight();
	return true;
}

// Старый метод removeProduct (для обратной совместимости)
void Shelf::removeProduct(Product* product)
{
	removeProduct(product, 1);
}

// Новые вспомогательные методы
bool Shelf::containsProduct(Product* product) const
{
	return std::find(products.begin(), products.end(), product) != products.end();
}

int Shelf::getProductQuantity(Product* product) const
{
	return std::count(products.begin(), products.end(), product);
}

// Геттер для продуктов
const std::vector<Product*>& Shelf::getProducts() const {
	return products;
}

void Shelf::getProductsInfo()
{
	std::cout << "=== ТОВАРЫ НА ПОЛКЕ '" << number << "' ===" << std::endl;

	if (products.empty()) {
		std::cout << "Полка пуста" << std::endl;
		return;
	}

	// Группируем товары по имени для отображения
	std::map<std::string, int> productCounts;
	for (Product* product : products) {
		if (product != nullptr) {
			productCounts[product->getName()]++;
		}
	}

	std::cout << "Конкретные товары (" << products.size() << " шт.):" << std::endl;
	int index = 1;
	for (const auto& pair : productCounts) {
		std::cout << index++ << ". " << pair.first << " x " << pair.second << " шт." << std::endl;
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
	currentWeight = 0.0f;
	for (Product* product : products) {
		if (product != nullptr) {
			currentWeight += product->getDimensions();
		}
	}
}

bool Shelf::writeOffProduct(Product* product)
{
	if (product == nullptr) {
		return false;
	}

	// Удаляем все экземпляры товара с полки
	int removedCount = removeProduct(product, getProductQuantity(product));

	if (removedCount > 0) {
		// Удаляем объект из памяти
		delete product;
		std::cout << "ТОВАР СПИСАН: '" << product->getName() << "' с полки '" << number << "'" << std::endl;
		return true;
	}

	return false;
}

int Shelf::getProductCountByType(std::string category)
{
	if (categoryQuantities.find(category) != categoryQuantities.end()) {
		return categoryQuantities[category];
	}
	return 0;
}


//============================Section============================================
Section::Section( 
	std::string name, 
	float capacity, 
	Warehouse* warehouse) 
	:
	section_id(IDGenerator::genShelfID()),
	name(name),
	capacity(capacity),
	warehouse(warehouse)
{}

Section::~Section()
{
	for (Shelf* shelf : shelves) {
		delete shelf;
	}
}

// Метод для поиска товара в секции и удаления его с полки
bool Section::removeProductFromShelf(Product* product, int quantity)
{
	if (product == nullptr || quantity <= 0) {
		return false;
	}

	// Ищем товар на полках секции
	for (Shelf* shelf : shelves) {
		if (shelf != nullptr && shelf->containsProduct(product)) {
			int availableOnShelf = shelf->getProductQuantity(product);
			int toRemove = std::min(quantity, availableOnShelf);

			if (shelf->removeProduct(product, toRemove)) {
				quantity -= toRemove;

				if (quantity <= 0) {
					updateCategoryStatistics();
					calculateCurrentLoad();
					return true;
				}
			}
		}
	}

	updateCategoryStatistics();
	calculateCurrentLoad();
	return quantity <= 0;
}

void Section::addShelf(std::string name, float capacity)
{
	for (Shelf* existingShelf : shelves) {
		if (existingShelf->getNumber() == name) {
			std::cout << "Ошибка: полка с именем '" << name << "' уже существует!" << std::endl;
			return;
		}
	}

	int newShelfId = shelves.size() + 1;
	Shelf* newShelf = new Shelf(name, capacity, this);

	shelves.push_back(newShelf);
	updateCategoryStatistics();
	calculateCurrentLoad();
}

void Section::removeShelf(Shelf* shelf)
{
	if (shelf == nullptr) {
		std::cout << "Ошибка: попытка удалить пустую полку!" << std::endl;
		return;
	}

	auto it = std::find(shelves.begin(), shelves.end(), shelf);

	if (it == shelves.end()) {
		std::cout << "Ошибка: полка не найдена в секции '" << name << "'!" << std::endl;
		return;
	}

	std::string shelfName = shelf->getNumber();
	shelves.erase(it);
	delete shelf;
	updateCategoryStatistics();
	calculateCurrentLoad();
}

void Section::getShelves()
{
	std::cout << "=== ПОЛКИ В СЕКЦИИ '" << name << "' ===" << std::endl;

	if (shelves.empty()) {
		std::cout << "В секции нет полок" << std::endl;
		return;
	}

	for (int i = 0; i < shelves.size(); i++) {
		Shelf* shelf = shelves[i];
		if (shelf != nullptr) {
			std::cout << i + 1 << ". Полка '" << shelf->getNumber() << "'" << std::endl;
			std::cout << "   ID: " << shelf->getShelf_id() << std::endl;
			std::cout << "   Вместимость: " << shelf->getCapacity() << std::endl;
			std::cout << "   Текущий вес: " << shelf->getCurrentWeight() << std::endl;
			std::cout << "   Свободно: " << shelf->getFreeSpace() << std::endl;
			std::cout << "   Товаров: " << shelf->getProducts().size() << " шт." << std::endl;
			std::cout << "   --------------------" << std::endl;
		}
	}

	std::cout << "Всего полок: " << shelves.size() << std::endl;
}

void Section::calculateCurrentLoad()
{
	currentLoad = 0.0f;
	for (Shelf* shelf : shelves) {
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
			const auto& shelfProducts = shelf->getProducts();
			for (Product* product : shelfProducts) {
				if (product != nullptr) {
					std::string category = product->getCategory();
					categoryQuantities[category]++;
				}
			}
		}
	}
}

int Section::getCategoryCount(const std::string& category)
{
	if (category.empty()) {
		return 0;
	}

	if (categoryQuantities.empty()) {
		updateCategoryStatistics();
	}

	return categoryQuantities[category];
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

	if (warehouse != nullptr) {
		std::cout << "Принадлежит складу: ID " << warehouse->getWarehouseId() << std::endl;
	}
	else {
		std::cout << "Склад: не указан" << std::endl;
	}

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
Warehouse::Warehouse(
	float totalCapacity) 
	:
	warehouse_id(IDGenerator::genWarehouseID()),
	totalCapacity(totalCapacity)
{
	// Подключаем обработчики сигналов
	setupSignalConnections();
}

void Warehouse::setupSignalConnections()
{
	// Обработчик добавления товара в чек
	GlobalSignals::productAddedToCheck().connect(
		[this](Product* product, int quantity) {
			this->onProductAddedToCheck(product, quantity);
		}
	);

	// Обработчик удаления товара из чека
	GlobalSignals::productRemovedFromCheck().connect(
		[this](Product* product, int quantity) {
			this->onProductRemovedFromCheck(product, quantity);
		}
	);
}

// Обработчик сигнала добавления товара в чек
void Warehouse::onProductAddedToCheck(Product* product, int quantity)
{
	if (product == nullptr || quantity <= 0) {
		return;
	}

	// Обновляем глобальную статистику склада
	updateGlobalCategoryStatistics();
	std::cout << "Сигнал: Обновлена статистика склада после продажи товара '"
		<< product->getName() << "'" << std::endl;
}

// Обработчик сигнала удаления товара из чека
void Warehouse::onProductRemovedFromCheck(Product* product, int quantity)
{
	if (product == nullptr || quantity <= 0) {
		return;
	}

	// Возвращаем товар на склад
	returnProductToStock(product, quantity);
	std::cout << "Сигнал: Товар '" << product->getName()
		<< "' возвращен на склад в количестве " << quantity << " шт." << std::endl;
}

Warehouse::~Warehouse()
{
	for (Section* section : sections) {
		delete section;
	}
}

void Warehouse::addSection(std::string name, float capacity)
{
	for (Section* existingSection : sections) {
		if (existingSection->getName() == name) {
			std::cout << "Ошибка: секция с именем '" << name << "' уже существует!" << std::endl;
			return;
		}
	}

	if (getFreeSpace() < capacity) {
		std::cout << "Ошибка: недостаточно места на складе для новой секции!" << std::endl;
		return;
	}

	int newSectionId = sections.size() + 1;
	Section* newSection = new Section( name, capacity, this);

	sections.push_back(newSection);
	updateGlobalCategoryStatistics();
}

void Warehouse::removeSection(Section* section)
{
	if (section == nullptr) {
		std::cout << "Ошибка: попытка удалить пустую секцию!" << std::endl;
		return;
	}

	auto it = std::find(sections.begin(), sections.end(), section);

	if (it == sections.end()) {
		std::cout << "Ошибка: секция не найдена на складе!" << std::endl;
		return;
	}

	if (!section->getShelvesList().empty()) {
		std::cout << " ВНИМАНИЕ: В секции '" << section->getName()
			<< "' остались полки с товарами! Удаление отменено." << std::endl;
		return;
	}

	std::string sectionName = section->getName();
	sections.erase(it);
	delete section;
	updateGlobalCategoryStatistics();
}

float Warehouse::getTotalLoad()
{
	float totalLoad = 0.0f;
	for (Section* section : sections) {
		if (section != nullptr) {
			totalLoad += section->getCurrentLoad();
		}
	}
	return totalLoad;
}

void Warehouse::updateGlobalCategoryStatistics()
{
	categoryQuantities.clear();
	for (Section* section : sections) {
		if (section != nullptr) {
			const std::map<std::string, int>& sectionCategories = section->getCategoryQuantities();
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
	if (category.empty()) {
		return 0;
	}

	if (categoryQuantities.empty()) {
		updateGlobalCategoryStatistics();
	}

	auto it = categoryQuantities.find(category);
	if (it != categoryQuantities.end()) {
		return it->second;
	}

	return 0;
}

// Метод для удаления продукта со склада
bool Warehouse::removeProductFromStock(Product* product, int quantity)
{
	if (product == nullptr || quantity <= 0) {
		return false;
	}

	// Проверяем, достаточно ли товара на складе
	if (product->getQuantity() < quantity) {
		std::cout << "Недостаточно товара на складе! Доступно: " << product->getQuantity() << ", требуется: " << quantity << std::endl;
		return false;
	}

	// Ищем продукт в секциях и удаляем
	bool productFound = false;
	int remainingQuantity = quantity;

	for (Section* section : sections) {
		if (section != nullptr) {
			if (section->removeProductFromShelf(product, remainingQuantity)) {
				productFound = true;
				remainingQuantity = 0;
				break;
			}
		}
	}

	if (productFound) {
		product->updateQuantity(-quantity);
		updateGlobalCategoryStatistics();
		std::cout << "Удалено " << quantity << " единиц товара '" << product->getName() << "' со склада" << std::endl;
		return true;
	}
	else {
		std::cout << "Товар '" << product->getName() << "' не найден на складе!" << std::endl;
		return false;
	}
}

// Метод для возврата продукта на склад
bool Warehouse::returnProductToStock(Product* product, int quantity)
{
	if (product == nullptr || quantity <= 0) {
		return false;
	}

	// Увеличиваем количество товара
	product->updateQuantity(quantity);

	// Здесь можно добавить логику размещения товара на конкретных полках
	// Пока просто обновляем статистику
	updateGlobalCategoryStatistics();
	std::cout << "Товар '" << product->getName() << "' возвращен на склад в количестве " << quantity << " шт." << std::endl;

	return true;
}

//============================Customer============================================
Customer::Customer(	std::string address) 
	: 
	customer_id(IDGenerator::genCustomerID()), 
	address(address), 
	loyaltyPoints(0), 
	discountPercent(0.0F)
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
Check::Check(
	Seller* seller, 
	Customer* customer, 
	Shop* shop, 
	std::string paymentMethod) 
	:
	check_id(IDGenerator::genCheckID()),
	dateTime(time(nullptr)),
	seller(seller),
	customer(customer),
	shop(shop),
	paymentMethod(paymentMethod),
	isPaid(false) {}

Check::~Check()
{
	// При уничтожении чека, если он не оплачен, возвращаем товары на склад
	if (!isPaid && !products.empty()) {
		returnProductsToStock();
	}
}

void Check::calculateTotal()
{
	totalAmount = 0;
	for (const auto& pair : products) {
		Product* product = pair.first;
		int quantity = pair.second;

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
	if (product == nullptr || quantity <= 0) {
		return;
	}

	// Проверяем доступность товара на складе
	if (product->getQuantity() < quantity) {
		std::cout << "Недостаточно товара на складе! Доступно: " << product->getQuantity() << ", требуется: " << quantity << std::endl;
		return;
	}

	// Отправляем сигнал о добавлении товара в чек
	GlobalSignals::productAddedToCheck().emit(product, quantity);

	// Добавляем товар в чек
	if (products.find(product) != products.end()) {
		products[product] += quantity;
	}
	else {
		products[product] = quantity;
	}

	calculateTotal();
	std::cout << "Товар '" << product->getName() << "' добавлен в чек" << std::endl;
}

void Check::removeProduct(Product* product)
{
	if (product == nullptr) {
		return;
	}

	auto it = products.find(product);
	if (it != products.end()) {
		int quantity = it->second;

		// Отправляем сигнал об удалении товара из чека
		GlobalSignals::productRemovedFromCheck().emit(product, quantity);

		products.erase(it);
		calculateTotal();
		std::cout << "Товар '" << product->getName() << "' удален из чека" << std::endl;
	}
}

void Check::applyDiscount(float discountPercent)
{
	if (customer != nullptr) {
		// Логика применения скидки через систему лояльности клиента
	}
}

void Check::getCheckDeteils()
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
	// Помечаем чек как оплаченный
	isPaid = true;
	std::cout << "Чек №" << check_id << " оплачен и распечатан" << std::endl;
}

// Метод для возврата всех товаров на склад (при отмене чека)
void Check::returnProductsToStock()
{
	for (const auto& pair : products) {
		Product* product = pair.first;
		int quantity = pair.second;
		GlobalSignals::productRemovedFromCheck().emit(product, quantity);
	}
	products.clear();
	calculateTotal();
	std::cout << "Все товары из неоплаченного чека возвращены на склад" << std::endl;
}

//============================Seller============================================
Seller::Seller(
	std::string name, 
	Shop* shop) 
	:
	seller_id(IDGenerator::genSellerID()),
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
Shop::Shop(std::string name) :
	shop_id(IDGenerator::genShopID()),
	name(name),
	warehouse(nullptr) {}

// Метод для удаления товара со склада
bool Shop::removeProductFromWarehouse(Product* product, int quantity) {
	if (warehouse != nullptr) {
		return warehouse->removeProductFromStock(product, quantity);
	}
	return false;
}

// Метод для возврата товара на склад
void Shop::returnProductToWarehouse(Product* product, int quantity) {
	if (warehouse != nullptr && product != nullptr && quantity > 0) {
		warehouse->returnProductToStock(product, quantity);
	}
}

Warehouse* Shop::getWarehouse() const {
	return warehouse;
}

void Shop::setWarehouse(Warehouse* wh) {
	warehouse = wh;
}

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

	if (it != sellers.end()) {
		sellers.erase(it);
	}
}

void Shop::getSellers()
{
	std::cout << "=== ПРОДАВЦЫ МАГАЗИНА '" << name << "' ===" << std::endl;

	if (sellers.empty()) {
		std::cout << "В магазине нет продавцов" << std::endl;
		return;
	}

	for (int i = 0; i < sellers.size(); i++) {
		Seller* seller = sellers[i];
		if (seller != nullptr) {
			std::cout << i + 1 << ". ID: " << seller->getSellerId()
				<< " | Имя: " << seller->getName() << std::endl;
		}
	}

	std::cout << "Всего продавцов: " << sellers.size() << std::endl;
}

void Shop::getShopInfo()
{
	std::cout << "=== ИНФОРМАЦИЯ О МАГАЗИНЕ ===" << std::endl;
	std::cout << "ID: " << shop_id << std::endl;
	std::cout << "Название: " << name << std::endl;
	std::cout << "Количество продавцов: " << sellers.size() << std::endl;

	if (warehouse != nullptr) {
		std::cout << "Склад: есть" << std::endl;
	}
	else {
		std::cout << "Склад: нет" << std::endl;
	}
}