#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_SERVER "localhost"
#define DEFAULT_PORT "12345"

int __cdecl main(void)
{
    WSAData socket_spec;
    int result_value = WSAStartup(MAKEWORD(2, 2), &socket_spec);
    if (result_value != 0)
    {
        std::cerr << "WSAStartup failed with error: " << result_value << std::endl;
        return 1;
    };

    struct addrinfo hints;
    struct addrinfo* result = NULL;
    struct addrinfo* succeeded = NULL;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    result_value = getaddrinfo(DEFAULT_SERVER, DEFAULT_PORT, &hints, &result);
    if (result_value != 0)
    {
        std::cerr << "getaddrinfo failed with error: " << result_value << std::endl;
        WSACleanup();
        return 1;
    }

    SOCKET connect_socket = INVALID_SOCKET;

    for (succeeded = result; succeeded != nullptr; succeeded = succeeded->ai_next)
    {
        connect_socket = socket(succeeded->ai_family, succeeded->ai_socktype, succeeded->ai_protocol);
        if (connect_socket == INVALID_SOCKET)
        {
            std::cerr << "socket failed with error: " << WSAGetLastError() << std::endl;
            WSACleanup();
            return 1;
        }
        result_value = connect(connect_socket, succeeded->ai_addr, (int)succeeded->ai_addrlen);
        if (result_value == SOCKET_ERROR)
        {
            closesocket(connect_socket);
            connect_socket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (connect_socket == INVALID_SOCKET)
    {
        std::cerr << "unable to connect to server: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    const char* send_buf = "Message from client to server";
    char recv_buf[DEFAULT_BUFLEN];
    int recv_buf_len = DEFAULT_BUFLEN;

    result_value = send(connect_socket, send_buf, (int)strlen(send_buf), 0);
    if (result_value == SOCKET_ERROR) {
        std::cerr << "send failed with error: " << WSAGetLastError() << std::endl;
        closesocket(connect_socket);
        WSACleanup();
        return 1;
    }

    std::cout << "bytes sent: " << result_value << std::endl;

    result_value = shutdown(connect_socket, SD_SEND);
    if (result_value == SOCKET_ERROR) {
        std::cerr << "shutdown failed with error: " << WSAGetLastError() << std::endl;
        closesocket(connect_socket);
        WSACleanup();
        return 1;
    }

    do {
        result_value = recv(connect_socket, recv_buf, recv_buf_len, 0);
        if (result_value > 0)
            std::cout << "bytes received: " << result_value << std::endl;
        else if (result_value == 0)
            std::cout << "connection closed" << std::endl;
        else
            std::cerr << "recv failed with error: " << WSAGetLastError() << std::endl;

    } while (result_value > 0);

    closesocket(connect_socket);
    WSACleanup();

    return 0;
}