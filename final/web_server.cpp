#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <syslog.h>
#include <string.h>

#include <cstdlib>
#include <iostream>
#include <string>

#include "server.hpp"

struct startup_args
{
    std::string ip;
    std::string port;
    std::string directory;
};

void demonize()
{
    pid_t pid = fork();

    if (pid < 0)
    {
        std::cerr << "fork() failed: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }

    if (pid > 0)
    {
        exit(EXIT_SUCCESS);
    }

    umask(0);

    pid_t sid = setsid();

    if (sid < 0)
    {
        std::cerr << "setsid() failed: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }

    if (chdir("/") < 0)
    {
        std::cerr << "chdir() failed: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

startup_args parse_args(int argc, char **argv)
{
    startup_args result;
    int opt = -1;
    while ((opt = getopt(argc, argv, "h:p:d:")) != -1)
    {
        switch (opt)
        {
        case 'h':
            result.ip = optarg;
            break;
        case 'p':
            result.port = atoi(optarg);
            break;
        case 'd':
            result.directory = optarg;
            break;
        default:
            break;
        }
    }

    return result;
}

int main(int argc, char **argv)
{
    demonize();
    startup_args arg = parse_args(argc, argv);
    try
    {
        server s(arg.ip, arg.port, arg.directory);
        s.run();
    }
    catch (std::exception &e)
    {
        exit(EXIT_FAILURE);
    }
    return 0;
}