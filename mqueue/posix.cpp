#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <string.h>

#include <fstream>
#include <iostream>

int main(){
    mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 8192;
    attr.mq_curmsgs = 0;
    mqd_t qid = mq_open("/test.mq", O_CREAT | O_RDONLY, 0777, &attr);
    if(qid == -1){
        std::cout << "error mq_open: " << errno << std::endl;
    }
    char msg[2048];
    memset(msg, 0, 2048);
    ssize_t recv = mq_receive(qid, msg, 2048, nullptr);
    std::ofstream out("/home/box/message.txt");
    out << msg;
}