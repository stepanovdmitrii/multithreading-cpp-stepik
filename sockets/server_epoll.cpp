#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>

#define MAX_EVENTS 32

int set_nonblock(int fd)
{
    int flags;
#if defined(O_NONBLOCK)
    if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
    {
        flags = 0;
    }
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#else
    flags = 1;
    return ioctl(fd, FIOBIO, &flags);
#endif
}

int main(int argc, char **argv)
{
    int master_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    sockaddr_in sock_addr;
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(12345);
    sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(master_socket, (sockaddr *)&sock_addr, sizeof(sock_addr));

    set_nonblock(master_socket);

    listen(master_socket, SOMAXCONN);

    int epoll = epoll_create1(0);

    epoll_event event;
    event.data.fd = master_socket;
    event.events = EPOLLIN;

    epoll_ctl(epoll, EPOLL_CTL_ADD, master_socket, &event);

    while (true)
    {
        epoll_event events[MAX_EVENTS];
        int n = epoll_wait(epoll, events, MAX_EVENTS, -1);
        for (unsigned int i = 0; i < n; ++i)
        {
            if (events[i].data.fd == master_socket)
            {
                int slave_socket = accept(master_socket, 0, 0);
                set_nonblock(slave_socket);

                epoll_event slave_event;
                slave_event.data.fd = slave_socket;
                slave_event.events = EPOLLIN;

                epoll_ctl(epoll, EPOLL_CTL_ADD, slave_socket, &slave_event);
            }
            else
            {
                static char buffer[1024];
                int recv_result = recv(events[i].data.fd, buffer, 1024, MSG_NOSIGNAL);
                if((recv_result == 0) && (errno != EAGAIN)){
                    shutdown(events[i].data.fd, SHUT_RDWR);
                    close(events[i].data.fd);
                }
                else if (recv_result > 0){
                    send(events[i].data.fd, buffer, recv_result, MSG_NOSIGNAL);
                }
            }
        }
    }

    return 0;
}