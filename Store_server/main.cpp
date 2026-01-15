#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include "StoreEnt.h"
#include <vector>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

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

        // echo-ответ
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

int main()
{
    RunServer();
    return 0;
}