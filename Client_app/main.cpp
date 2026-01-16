#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <locale>
#include <codecvt>

//для запуска
//cd Client_app
// ./ClientApp.exe

//для компиляции
//cl.exe /std:c++17 /EHsc main.cpp ws2_32.lib /FeClientApp.exe

#pragma comment(lib, "ws2_32.lib")

using namespace std;

struct CartItem{
    int quantity;
};

std::map<std::string, CartItem> cart;

// ПРОТОТИПЫ ВСЕХ ФУНКЦИЙ
bool ServerConnect(const string& ip, int port);
bool SendToServer(const string& message);
string ReceiveFromServer();
void ServerClose();
void printHelp();
void addToCart(const std::string& category, int quantity);
void removeFromCart(const std::string& category, int quantity);
void printCart();
std::string buildBuyCommand();
void buy();

SOCKET clientSocket = INVALID_SOCKET;

// ========== РЕАЛИЗАЦИИ ФУНКЦИЙ ==========

void addToCart(const std::string& category, int quantity){
    cart[category].quantity += quantity;
}

void removeFromCart(const std::string& category, int quantity){
    auto it = cart.find(category);
    if (it == cart.end())
        return;

    it->second.quantity -= quantity;
    if (it->second.quantity <= 0)
        cart.erase(it);
}

void printCart(){
    if (cart.empty())
    {
        cout << "Корзина пуста\n";
        return;
    }

    cout << "Корзина:\n";
    for (const auto& [category, item] : cart)
    {
        cout << category << " - " << item.quantity << "\n";
    }
}

std::string buildBuyCommand(){
    std::string cmd = "купить\n";
    for (const auto& [category, item] : cart)
    {
        cmd += category + " " + std::to_string(item.quantity) + "\n";
    }
    cmd += "END\n";
    return cmd;
}

void buy(){
    if (cart.empty())
    {
        cout << "Корзина пуста\n";
        return;
    }

    std::string cmd = buildBuyCommand();
    SendToServer(cmd);

    std::string response = ReceiveFromServer();
    cout << "Чек:\n" << response << endl;

    cart.clear();
}

void printHelp(){
    cout <<
        "Доступные команды:\n"
        "catalog                - показать каталог\n"
        "add <кат> <n>          - добавить в корзину\n"
        "delete <кат> <n>       - удалить из корзины\n"
        "crate                  - показать корзину\n"
        "buy                    - оформить покупку\n"
        "exit                   - выйти\n";
}

bool ServerConnect(const string& ip, int port)
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        cout << "Ошибка инициализации Winsock\n";
        return false;
    }

    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET)
    {
        cout << "Ошибка создания сокета\n";
        WSACleanup();
        return false;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr);

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        cout << "Не удалось подключиться к серверу\n";
        closesocket(clientSocket);
        WSACleanup();
        return false;
    }

    cout << "Подключено к серверу\n";
    return true;
}

bool SendToServer(const string& message)
{
    if (clientSocket == INVALID_SOCKET)
        return false;

    int result = send(clientSocket, message.c_str(), (int)message.size(), 0);
    return result != SOCKET_ERROR;
}

string ReceiveFromServer()
{
    char buffer[512];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

    if (bytesReceived <= 0)
        return "";

    buffer[bytesReceived] = '\0';
    return string(buffer);
}

void ServerClose()
{
    if (clientSocket != INVALID_SOCKET)
    {
        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
    }
    WSACleanup();
}

int main()
{
// Включаем поддержку UTF-8 в консоли Windows
    system("chcp 65001 > nul");  // Устанавливаем UTF-8
    
    // Настройка локали
    setlocale(LC_ALL, "ru_RU.UTF-8");

    if (!ServerConnect("127.0.0.1", 54000))
        return 1;

    printHelp();
    
    cout << "Введите команду (помощь — список команд)\n";

    while (true)
    {
        cout << "> ";
        std::string command;
        std::getline(cin, command);

        std::stringstream ss(command);
        std::string cmd;
        ss >> cmd;

        if (cmd == "exit")
        {
            SendToServer("exit");
            break;
        }
        else if (cmd == "catalog")
        {
            SendToServer("каталог");
            cout << ReceiveFromServer() << endl;
        }
        else if (cmd == "crate")
        {
            printCart();
        }
        else if (cmd == "add")
        {
            std::string category;
            int quantity;

            if (ss >> category >> quantity)
                addToCart(category, quantity);
            else
                cout << "Формат: добавить <категория> <кол-во>\n";
        }
        else if (cmd == "delete")
        {
            std::string category;
            int quantity;

            if (ss >> category >> quantity)
                removeFromCart(category, quantity);
            else
                cout << "Формат: удалить <категория> <кол-во>\n";
        }
        else if (cmd == "buy")
        {
            buy();
        }
        else if (cmd == "help" || cmd == "помощь")
        {
            printHelp();
        }
        else
        {
            cout << "Неизвестная команда. Введите 'help'\n";
        }
    }
    
    ServerClose();
    return 0;
}