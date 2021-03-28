#ifndef REPLY_HPP
#define REPLY_HPP

#include <boost/asio.hpp>

#include <string>
#include <vector>
#include <sstream>

struct reply {
    bool found;
    std::string content;

    std::string message;

    void set_message();
};

#endif