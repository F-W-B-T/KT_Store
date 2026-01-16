#include <iostream>
#include <sstream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include "StoreEnt.h"
#include <vector>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

Shop* g_shop = nullptr;

//функции сервера:
std::string buildCatalogResponse(Warehouse* wh)
{
    std::string response = "КАТАЛОГ\n";

    const auto& stats = wh->getGlobalCategoryStatistics();

    for (const auto& pair : stats)
    {
        response += pair.first + " - " + std::to_string(pair.second) + "\n";
    }

    response += "END\n";
    return response;
}

std::string buildHelpResponse()
{
    std::string response;
    response += "ДОСТУПНЫЕ КОМАНДЫ:\n";
    response += "каталог          - показать категории товаров и количество\n";
    response += "купить           - покупка товаров по категориям\n";
    response += "                  формат:\n";
    response += "                  категория количество\n";
    response += "                  ...\n";
    response += "                  END\n";
    response += "помощь           - показать список команд\n";
    response += "END\n";
    return response;
}


DWORD WINAPI HandleClient(LPVOID lpParam)
{
    SOCKET clientSocket = (SOCKET)lpParam;
    char buffer[512];

    cout << "Поток клиента запущен\n";

    while (true)
    {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

        if (bytesReceived <= 0)
        {
            cout << "Клиент отключился\n";
            break;
        }

        buffer[bytesReceived] = '\0';
        cout << "Получено от клиента: " << buffer << endl;

        // ================== ПОМОЩЬ ==================
        if (strcmp(buffer, "помощь") == 0){
            std::string response = buildHelpResponse();
            send(clientSocket, response.c_str(), response.size(), 0);
            continue;
        }

        // ================== КАТАЛОГ ==================
        if (strcmp(buffer, "каталог") == 0)
        {
            std::string response =
                buildCatalogResponse(g_shop->getWarehouse());

            send(clientSocket, response.c_str(), response.size(), 0);
            continue;
        }

        // ================== ПОКУПКА ==================
        if (strcmp(buffer, "купить") == 0)
        {
            Check* check = new Check(
                g_shop->getSeller(),
                nullptr,
                g_shop,
                "наличные"
            );

            bool error = false;

            while (true)
            {
                int bytes = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
                buffer[bytes] = '\0';

                if (strcmp(buffer, "END") == 0)
                    break;

                std::string category;
                int quantity;

                std::stringstream ss(buffer);
                ss >> category >> quantity;

                bool ok = g_shop->getWarehouse()
                    ->removeProductsByCategory(category, quantity, check);

                if (!ok)
                {
                    error = true;
                    break;
                }
            }

            if (error)
            {
                check->returnProductsToStock();
                delete check;

                std::string err =
                    "ОШИБКА: недостаточно товара\n";
                send(clientSocket, err.c_str(), err.size(), 0);
                continue;
            }

            check->calculateTotal();

            std::ostringstream out;
            out << "ЧЕК\n";
            check->printCheck(); // если печатает в cout — допустимо

            std::string done = "ПОКУПКА ЗАВЕРШЕНА\n";
            send(clientSocket, done.c_str(), done.size(), 0);
            continue;
        }

        // ================== ЭХО ==================
        send(clientSocket, buffer, bytesReceived, 0);
    }

    closesocket(clientSocket);
    return 0;
}


void RunServer()
{
    setlocale(LC_ALL, ".UTF8");
    SetConsoleOutputCP(65001);

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        cout << "Ошибка инициализации Winsock\n";
        return;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET)
    {
        cout << "Ошибка создания сокета\n";
        WSACleanup();
        return;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(54000);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        cout << "Ошибка bind\n";
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        cout << "Ошибка listen\n";
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    cout << "TCP сервер запущен\n";

    vector<HANDLE> clientThreads;

    while (true)
    {
        if (clientThreads.size() >= 20)
        {
            cout << "Достигнут лимит клиентов (20)\n";
            Sleep(1000);
            continue;
        }

        sockaddr_in clientAddr;
        int clientSize = sizeof(clientAddr);

        SOCKET clientSocket = accept(
            serverSocket,
            (sockaddr*)&clientAddr,
            &clientSize
        );

        if (clientSocket == INVALID_SOCKET)
            continue;

        cout << "Подключился новый клиент\n";

        HANDLE hThread = CreateThread(
            NULL,
            0,
            HandleClient,
            (LPVOID)clientSocket,
            0,
            NULL
        );

        if (hThread != NULL)
        {
            clientThreads.push_back(hThread);
        }
        else
        {
            closesocket(clientSocket);
        }
    }

    closesocket(serverSocket);
    WSACleanup();
}

// Массивы данных
static const std::vector<std::string> CATEGORIES = {
    "phone", "laptop", "keyboard", "mouse", "monitor", 
    "headphones", "tablet", "router", "flash drive", "webcam"
};

static const std::vector<std::string> COMPANY_NAMES = {
    "Samsung", "Apple", "Xiaomi", "HP", "Lenovo",
    "Dell", "Logitech", "Asus", "Acer", "Huawei"
};

// Генерация случайных адресов для компаний
static std::string generateRandomAddress() {
    std::vector<std::string> cities = {"Москва", "Санкт-Петербург", "Новосибирск", "Екатеринбург", "Казань"};
    std::vector<std::string> streets = {"ул. Ленина", "ул. Пушкина", "пр. Мира", "ул. Советская", "ул. Центральная"};
    
    return IDGenerator::getRandomElement(streets) + ", " + 
           std::to_string(1 + std::rand() % 200) + ", " + 
           IDGenerator::getRandomElement(cities);
}

// Функция создания случайных поставщиков
static SupplierInfo* createRandomSupplier() {
    std::string company = IDGenerator::getRandomElement(COMPANY_NAMES);
    std::string address = generateRandomAddress();
    return new SupplierInfo(company, address);
}

// Функция создания случайного товара
static Product* generateRandomProductEx(
    const std::string& specificCategory = "",
    const std::string& specificCompany = "",
    float minPrice = 100.0f,
    float maxPrice = 1000.0f,
    int minQuantity = 1,
    int maxQuantity = 1
) {
    // Выбор категории: если указана конкретная, используем её, иначе случайную
    std::string category = specificCategory.empty() ? 
        IDGenerator::getRandomElement(CATEGORIES) : specificCategory;
    
    // Выбор компании: если указана конкретная, используем её, иначе случайную
    std::string company = specificCompany.empty() ?
        IDGenerator::getRandomElement(COMPANY_NAMES) : specificCompany;
    
    // Генерация уникального ID
    static int productCounter = 0;
    productCounter++;
    std::string name = category + "_" + company + "_" + std::to_string(productCounter);
    
    // Генерация цены в заданном диапазоне
    float price = minPrice + (std::rand() % static_cast<int>((maxPrice - minPrice) * 100)) / 100.0f;
    
    // Генерация количества в заданном диапазоне
    int quantity = minQuantity + std::rand() % (maxQuantity - minQuantity + 1);
    
    float dimensions = IDGenerator::genDimensions();
    std::string fabricator = company;
    int serial_num = IDGenerator::genSerialNumber();
    time_t warranty_date = IDGenerator::genWarrantyDate();
    
    SupplierInfo* manufacturer = createRandomSupplier();
    
    return new Product(
        name, price, quantity, category, dimensions,
        fabricator, serial_num, warranty_date, manufacturer
    );
}

void initialization(){
    g_shop = new Shop("Магазин", 100);

    Seller* seller = new Seller("Яковлев Ярослав", g_shop);
    for (int i = 0; i< 25; i++){
        g_shop->addProductToWarehouse(generateRandomProductEx());
    }
}

int main()
{
    initialization();
    RunServer();
    return 0;
}