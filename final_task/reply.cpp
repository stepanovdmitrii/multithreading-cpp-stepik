#include "reply.hpp"

void reply::set_message(){
    std::stringstream ss;
    if(found){
        ss << "HTTP/1.0 200 OK\r\n";
        ss << "Content-length: " << content.size() << "\r\n";
        ss << "Content-Type: text/html\r\n";
        ss << "\r\n";
        ss << content;
    }
    else{
        ss << "HTTP/1.0 404 NOT FOUND\r\n";
        ss << "Content-Type: text/html\r\n";
        ss << "\r\n";
    }

    ss >> message;
}