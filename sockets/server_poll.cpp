#include <iostream>
#include <set>
#include <algorithm>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#define POLL_SIZE 2048

int set_nonblock(int fd){
    int flags;
#if defined(O_NONBLOCK)
    if(-1 == (flags = fcntl(fd, F_GETFL, 0))){
        flags = 0;
    }
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#else
    flags = 1;
    return ioctl(fd, FIOBIO, &flags);
#endif
}

int main(int argc, char** argv){
    int master_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    std::set<int> slave_sockets;
    std::set<int> failed_sockets;
    
    sockaddr_in sock_addr;
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(12345);
    sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(master_socket, (sockaddr*)&sock_addr, sizeof(sock_addr));

    set_nonblock(master_socket);

    listen(master_socket, SOMAXCONN);

    pollfd set[POLL_SIZE];
    set[0].fd = master_socket;
    set[0].events = POLLIN;

    while(true){
        unsigned int idx = 1;
        for(auto it = slave_sockets.begin(); it != slave_sockets.end(); ++it){
            set[idx].fd = *it;
            set[idx].events = POLLIN;
            ++idx;
        }

        unsigned int set_size = 1 + slave_sockets.size();
        
        poll(set, set_size, -1);

        for(unsigned int i = 0; i < set_size; ++i){
            if(set[i].events & POLLIN){
                if(i){
                    static char buffer[1024];
                    int recv_size = recv(set[i].fd, buffer, 1024, MSG_NOSIGNAL);
                    if((recv_size == 0) && (errno != EAGAIN)){
                        failed_sockets.insert(set[i].fd);
                    }
                    else if (recv_size > 0) {
                        send(set[i].fd, buffer, recv_size, MSG_NOSIGNAL);
                    }
                }
                else{
                    int slave_socket = accept(master_socket, 0, 0);
                    set_nonblock(slave_socket);
                    slave_sockets.insert(slave_socket);
                }
            }
        }

        for(int failed_socket : failed_sockets){
            shutdown(failed_socket, SHUT_RDWR);
            close(failed_socket);
            slave_sockets.erase(failed_socket);
        }
    }

    return 0;
}