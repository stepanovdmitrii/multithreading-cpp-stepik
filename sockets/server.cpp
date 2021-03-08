#include <iostream>
#include <set>
#include <algorithm>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

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

    while(true){
        fd_set set;
        FD_ZERO(&set);
        FD_SET(master_socket, &set);
        for(const auto& sd: slave_sockets){
            FD_SET(sd, &set);
        }
        int max_socket = std::max(master_socket, *std::max_element(slave_sockets.begin(), slave_sockets.end()));
        select(max_socket + 1, &set, NULL, NULL, NULL);
        for(auto it = slave_sockets.begin(); it != slave_sockets.end(); ++it){
            if(FD_ISSET(*it, &set)){
                static char buffer[1024];
                int recv_size = recv(*it, buffer, 1024, MSG_NOSIGNAL);
                if((recv_size == 0) && (errno != EAGAIN)){
                    failed_sockets.insert(*it);
                }
                else if (recv_size != 0) {
                    send(*it, buffer, recv_size, MSG_NOSIGNAL);
                }
            }
        }
        if(FD_ISSET(master_socket, &set)){
            int new_slave_socket = accept(master_socket, 0, 0);
            set_nonblock(new_slave_socket);
            slave_sockets.insert(new_slave_socket);
        }

        for(int failed_socket : failed_sockets){
            shutdown(failed_socket, SHUT_RDWR);
            close(failed_socket);
            slave_sockets.erase(failed_socket);
        }
    }

    return 0;
}