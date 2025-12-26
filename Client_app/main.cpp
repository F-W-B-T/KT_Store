#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main() {
   setlocale(LC_ALL, ".UTF8");
    SetConsoleOutputCP(65001);

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        cout << "Ошибка инициализации Winsock\n";
        return 1;
    }

    // Создание сокета
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        cout << "Ошибка создания сокета\n";
        WSACleanup();
        return 1;
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
        return 1;
    }

    cout << "Подключено к серверу!\n";

    // Отправка сообщения
    string message = "Привет, сервер!";
    send(clientSocket, message.c_str(), (int)message.size(), 0);

    // Прием ответа
    char buffer[512];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        cout << "Ответ сервера: " << buffer << endl;
    }

    // Закрытие
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
