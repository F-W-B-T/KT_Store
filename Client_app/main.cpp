#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>

//cd .\Client_App
//.\ClientApp.exe

#pragma comment(lib, "ws2_32.lib")

using namespace std;

void ServerStartUp(){
WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        cout << "Ошибка инициализации Winsock\n";
    }

    // Создание сокета
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        cout << "Ошибка создания сокета\n";
        WSACleanup();
    }

    // Настройка адреса сервера
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(54000); // порт сервера
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr); // локальный сервер

    // Подключение к серверу
    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cout << "Не удалось подключиться к серверу\n";
        closesocket(clientSocket);
        WSACleanup();
    }

    cout << "Подключено к серверу!\n";
}

void SentMessege(string messege){
     send(clientSocket, message.c_str(), (int)message.size(), 0);
}

char Response(){}

void ServerClose(){

}

int main() {
   setlocale(LC_ALL, ".UTF8");
    SetConsoleOutputCP(65001);
//ServerStartUp
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        cout << "Ошибка инициализации Winsock\n";
        return 1;
    }
//ServerStartU
    // Создание сокета
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        cout << "Ошибка создания сокета\n";
        WSACleanup();
        return 1;
    }
//ServerStartUp
    // Настройка адреса сервера
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(54000); // порт сервера
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr); // локальный сервер
//ServerStartUp
    // Подключение к серверу
    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cout << "Не удалось подключиться к серверу\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
//ServerStartUp
    cout << "Подключено к серверу!\n";
//sentmessege
    // Отправка сообщения
    string message = "Привет, сервер!";
    send(clientSocket, message.c_str(), (int)message.size(), 0);
//response
    // Прием ответа
    char buffer[512];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        cout << "Ответ сервера: " << buffer << endl;
    }
//ServerClose
    // Закрытие
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
