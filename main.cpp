// main.cpp : Nithin
// Desc : the starting point of a chat server app

#pragma comment(lib, "Ws2_32.lib")

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

using namespace std;

const int kPort = 3001;
const int kBufferLength = 4096;
const int kByteBufferZeroErr = 1;

const int WINSOCK_MAJOR_VERSION = 2;
const int WINSOCK_MINOR_VERSION = 2;

int main()
{

    // initialise winsock
    WSADATA wsData;

    WORD ver = MAKEWORD(WINSOCK_MAJOR_VERSION, WINSOCK_MINOR_VERSION);

    int wsOK = WSAStartup(ver, &wsData);

    if (wsOK != 0)
    {
        std::cerr << "Can't initialise winsock! Quitting." << std::endl;
        return 1;
    }

    // create a socket
    SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);

    if (listening == INVALID_SOCKET)
    {
        std::cerr << "Can't create socket! Quitting." << std::endl;
        return 1;
    }

    // bind socket to IP address and port to a socket
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(kPort);           // host to network short
    hint.sin_addr.S_un.S_addr = INADDR_ANY; // could also use inet_pton

    bind(listening, (sockaddr *)&hint, sizeof(hint));

    // tell winsock the socket is for listening
    listen(listening, SOMAXCONN);

    // wait for a connection
    sockaddr_in client;
    int clientSize = sizeof(client);

    SOCKET clientSocket = accept(listening, (sockaddr *)&client, &clientSize);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cerr << "Invalid client socket! Quitting." << std::endl;
        return 1;
    }

    char host[NI_MAXHOST];    // client's remote name
    char service[NI_MAXHOST]; // service (i.e. port) the client is connected on

    ZeroMemory(host, NI_MAXHOST); // memset(host, 0 , NI_MAXHOST); in unix based systems
    ZeroMemory(service, NI_MAXHOST);

    if (getnameinfo((sockaddr *)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXHOST, 0) == 0)
    {
        std::cout << "in here" << std::endl;
        std::cout << host << " connected on port " << service << std::endl;
    }
    else
    {
        std::cout << "NO ! in here" << std::endl;
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        std::cout << host << " port " << ntohs(client.sin_port) << std::endl;
    }

    // close listening socket
    closesocket(listening); // if we do not need other clients

    // while loop: accept and echo message back to client
    char buf[kBufferLength];
    while (true)
    {
        ZeroMemory(buf, kBufferLength);

        // wait for client to send data;

        int bytesReceived = recv(clientSocket, buf, kBufferLength, 0);

        if (bytesReceived == SOCKET_ERROR)
        {
            std::cerr << "Error in receive! Quitting." << std::endl;
            break;
        }
        if (bytesReceived == 0)
        {
            std::cout << "Client disconnected." << std::endl;
            break;
        }

        // else send info back

        // echo message back to client;
        send(clientSocket, buf, bytesReceived + kByteBufferZeroErr, 0);
    }

    // close the socket

    closesocket(clientSocket);

    // clean up winsock
    WSACleanup();

    return 0;
}