#include <windows.h>

#include <iostream>
#include <string>
#include "StoreEnt.h"

using namespace std;

int main()
{
    SetConsoleOutputCP(65001);
    setlocale(LC_ALL, ".UTF8");


    // ===================== ИНИЦИАЛИЗАЦИЯ СИСТЕМЫ =====================

    // Магазин
    Shop* shop = new Shop("TechMarket");

    // Склад
    Warehouse* warehouse = new Warehouse(5000);
    shop->setWarehouse(warehouse);

    // --- Секции склада ---
    warehouse->addSection("Смартфоны", 1500);
    warehouse->addSection("Ноутбуки", 2000);
    warehouse->addSection("Часы", 500);

    // Указатели на секции (для добавления полок)
    Section* secSmart = warehouse->getSectionsList()[0];
    Section* secNote  = warehouse->getSectionsList()[1];
    Section* secWatch = warehouse->getSectionsList()[2];

    // --- Полки ---
    secSmart->addShelf("S-1", 300);
    secNote->addShelf("N-1", 500);
    secWatch->addShelf("W-1", 150);

    // --- Продавцы ---
    Seller* ivan = new Seller("Иван", shop);
    Seller* anna = new Seller("Анна", shop);

    shop->addSeller(ivan);
    shop->addSeller(anna);

    // --- Поставщики ---
    SupplierInfo* samsung = new SupplierInfo("Samsung", "Seoul Street 1");
    SupplierInfo* lenovo  = new SupplierInfo("Lenovo", "Beijing Avenue 12");

    // --- Товары ---
    Product* phone = new Product(
        "Galaxy S24", 89990, 30, "Смартфон",
        1.0f, "Samsung", 123456, time(nullptr) + 60*60*24*365, samsung
    );

    Product* laptop = new Product(
        "Lenovo Legion 5", 129990, 10, "Ноутбук",
        3.0f, "Lenovo", 987654, time(nullptr) + 60*60*24*365, lenovo
    );

    Product* watch = new Product(
        "Galaxy Watch 6", 29990, 15, "Часы",
        0.5f, "Samsung", 222333, time(nullptr) + 60*60*24*365, samsung
    );

    // Размещение товаров на полках
    secSmart->getShelvesList()[0]->addProduct(phone, 10);
    secNote->getShelvesList()[0]->addProduct(laptop, 5);
    secWatch->getShelvesList()[0]->addProduct(watch, 8);



    // ===================== МЕНЮ =====================

    int choice = -1;
    while (choice != 0)
    {
        cout << "\n==== МЕНЮ ====\n";
        cout << "1. Показать информацию о магазине\n";
        cout << "2. Показать продавцов\n";
        cout << "3. Показать склад\n";
        cout << "4. Совершить покупку\n";
        cout << "0. Выход\n";
        cout << "Ваш выбор: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            shop->getShopInfo();
            break;
        case 2:
            shop->getSellers();
            break;
        case 3:
            warehouse->getSections();
            break;
        case 4:
        {
            cout << "\n--- СОЗДАНИЕ ПОКУПКИ ---\n";
            Customer* cust = new Customer("ул. Победы 13");

            // Создаём чек у Ивана
            Check* check = ivan->createCheck(cust, "Наличные");

            check->addProduct(phone, 2);
            check->addProduct(watch, 1);

            check->getCheckDeteils();
            check->printCheck();

            break;
        }
        case 0:
            cout << "Завершение..." << endl;
            break;

        default:
            cout << "Неверный ввод!" << endl;
        }
    }

    return 0;
}
