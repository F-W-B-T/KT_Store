#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <string>

//cd .\Client_App
//.\ClientApp.exe

#pragma comment(lib, "ws2_32.lib")

using namespace std;


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

    // Пример общения
    SendMessage("LIST");
    string response = ReceiveResponse();
    cout << "Ответ сервера: " << response << endl;

    ServerClose();
    return 0;
}