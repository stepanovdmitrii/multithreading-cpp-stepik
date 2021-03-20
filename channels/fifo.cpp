#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main(){
    if(-1 == mkfifo("/home/box/in.fifo", O_RDWR)){
        return -1;
    }
    if(-1 == mkfifo("/home/box/out.fifo", O_RDWR)){
        return -1;
    }

    int in = open("/home/box/in.fifo", O_RDONLY | O_NONBLOCK);
    int out = -1;

    char buffer[512];
    int bytes_read = -1;

    while((bytes_read = read(in, &buffer, 512)) > 0){
        if(out == -1){
            out = open("/home/box/out.fifo", O_WRONLY);
        }
        write(out, buffer, bytes_read);
    }
    return -1;
}