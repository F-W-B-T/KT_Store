#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include "StoreEnt.h"

#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main()
{
    setlocale(LC_ALL, ".UTF8");
    SetConsoleOutputCP(65001);

    //setlocale(LC_ALL, "rus");
    //SetConsoleOutputCP(65001);
    //SetConsoleCP(65001);

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2),&wsaData)!=0){
        cout<< "Ошибка инициализации Winsock\n";
        return 1;
    } 

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(serverSocket == INVALID_SOCKET){
        cout << "Ошибка создания сокета\n";
        WSACleanup();
        return 1;
    }

    // Настройка адреса
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(54000);     // порт
    serverAddr.sin_addr.s_addr = INADDR_ANY; // любой IP

    // bind
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        cout << "Ошибка bind\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // listen
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        cout << "Ошибка listen\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "TCP сервер запущен. Ожидание клиента...\n";

    // accept (надо будет с ним что-то сделать)
    sockaddr_in clientAddr;
    int clientSize = sizeof(clientAddr);
    SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientSize);

    if (clientSocket == INVALID_SOCKET)
    {
        cout << "Ошибка accept\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "Клиент подключился\n";

    // Прием данных
    char buffer[512];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived > 0)
    {
        buffer[bytesReceived] = '\0';
        cout << "Получено: " << buffer << endl;
        send(clientSocket, buffer, bytesReceived, 0); // echo
    }

    // Закрытие
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
return 0;
}


  /*  

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
*/
    