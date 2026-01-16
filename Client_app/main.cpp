#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <sstream>   // ← ВОТ ЭТОГО НЕ ХВАТАЛО
#include <map>



//cd .\Client_App //для запуска клиента
//.\ClientApp.exe

#pragma comment(lib, "ws2_32.lib")

using namespace std;

struct CartItem{
    int quantity;
};

std::map<std::string, CartItem> cart; // category -> quantity

//функции корзины:
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

//команды на сервер:
std::string buildBuyCommand(){
    std::string cmd = "купить\n";
    for (const auto& [category, item] : cart)
    {
        cmd += category + " - " + std::to_string(item.quantity) + "\n";
    }
    return cmd;
}

void buy(){
    if (cart.empty())
    {
        cout << "Корзина пуста\n";
        return;
    }

    std::string cmd = buildBuyCommand();
    SendMessage(cmd);

    std::string response = ReceiveResponse();
    cout << "Чек:\n" << response << endl;

    cart.clear(); // очистка после успешной покупки
}

//помощь для клиента
void printHelp(){
    cout <<
        "Доступные команды:\n"
        "каталог                - показать каталог\n"
        "добавить <кат> <n>     - добавить в корзину\n"
        "удалить <кат> <n>      - удалить из корзины\n"
        "корзина                - показать корзину\n"
        "купить                 - оформить покупку\n"
        "выход                  - выйти\n";
}


// Глобальный сокет клиента
SOCKET clientSocket = INVALID_SOCKET;

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

bool SendMessage(const string& message)
{
    if (clientSocket == INVALID_SOCKET)
        return false;

    int result = send(clientSocket, message.c_str(), (int)message.size(), 0);
    return result != SOCKET_ERROR;
}

string ReceiveResponse()
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
    setlocale(LC_ALL, ".UTF8");
    SetConsoleOutputCP(65001);

    if (!ServerConnect("127.0.0.1", 54000))
        return 1;

    printHelp();

    while (true)
    {
        cout << "> ";
        std::string command;
        std::getline(cin, command);

        if (command == "выход")
        {
            SendMessage("выход");
            break;
        }
        else if (command == "помощь")
        {
            printHelp();
        }
        else if (command == "каталог")
        {
            SendMessage("каталог");
            cout << ReceiveResponse() << endl;
        }
        else if (command == "корзина")
        {
            printCart();
        }
        else if (command == "купить")
        {
            buy();
        }
        else if (command.rfind("добавить", 0) == 0)
        {
            std::string cat;
            int qty;
            std::string cmd;
            std::stringstream ss(command);
            ss >> cmd >> cat >> qty;
        }
        else if (command.rfind("удалить", 0) == 0)
        {
            std::string cat;
            int qty;
            std::string cmd;
            std::stringstream ss(command);
            ss >> cmd >> cat >> qty;
        }
        else
        {
            cout << "Неизвестная команда\n";
        }
    }

    ServerClose();
    return 0;
}
