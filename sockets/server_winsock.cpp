#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <stdlib.h>
#include <iostream>

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_SERVER "localhost"
#define DEFAULT_PORT "12345"

int __cdecl main(void) {
    WSAData socket_spec;
    int result_value = WSAStartup(MAKEWORD(2, 2), &socket_spec);
    if (result_value != 0) {
        std::cerr << "WSAStartup failed with error: " << result_value << std::endl;
        return 1;
    };

    struct addrinfo hints;
    struct addrinfo* result = NULL;

    ZeroMemory(&hints, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    result_value = getaddrinfo(DEFAULT_SERVER, DEFAULT_PORT, &hints, &result);
    if (result_value != 0) {
        std::cerr << "getaddrinfo failed with error: " << result_value << std::endl;
        WSACleanup();
        return 1;
    }

    SOCKET listen_socket = INVALID_SOCKET;
    SOCKET client_socket = INVALID_SOCKET;

    listen_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (listen_socket == INVALID_SOCKET) {
        std::cerr << "socket failed with error: " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    result_value = bind(listen_socket, result->ai_addr, (int)result->ai_addrlen);
    if (result_value == SOCKET_ERROR) {
        std::cerr << "bind failed with error: " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        closesocket(listen_socket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    result_value = listen(listen_socket, SOMAXCONN);
    if (result_value == SOCKET_ERROR) {
        std::cerr << "listen failed with error: " << WSAGetLastError() << std::endl;
        closesocket(listen_socket);
        WSACleanup();
        return 1;
    }

    client_socket = accept(listen_socket, NULL, NULL);
    if (client_socket == INVALID_SOCKET) {
        std::cerr << "accept failed with error: " << WSAGetLastError() << std::endl;
        closesocket(listen_socket);
        WSACleanup();
        return 1;
    }

    closesocket(listen_socket);

    int send_result_value;
    char recv_buf[DEFAULT_BUFLEN];
    int recv_buf_len = DEFAULT_BUFLEN;

    do {
        result_value = recv(client_socket, recv_buf, recv_buf_len, 0);
        if (result_value > 0) {
            std::cout << "bytes received: " << result_value << std::endl;

            send_result_value = send(client_socket, recv_buf, result_value, 0);
            if (send_result_value == SOCKET_ERROR) {
                std::cerr << "send failed with error: " << WSAGetLastError() << std::endl;
                closesocket(client_socket);
                WSACleanup();
                return 1;
            }
            std::cout << "bytes sent: " << send_result_value << std::endl;
        }
        else if (result_value == 0)
            std::cout << "connection closing..." << std::endl;
        else {
            std::cerr << "recv failed with error: " << WSAGetLastError() << std::endl;
            closesocket(client_socket);
            WSACleanup();
            return 1;
        }

    } while (result_value > 0);

    result_value = shutdown(client_socket, SD_SEND);
    if (result_value == SOCKET_ERROR) {
        std::cerr << "shutdown failed with error: %d\n" << WSAGetLastError() << std::endl;
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    closesocket(client_socket);
    WSACleanup();

    return 0;
}