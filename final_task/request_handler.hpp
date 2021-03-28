#ifndef REQUEST_HANDLER_HPP
#define REQUEST_HANDLER_HPP

#include <boost/noncopyable.hpp>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include "reply.hpp"

class request_handler: private boost::noncopyable
{
private:
    std::string _doc_root;

    static bool url_decode(const std::string& in, std::string& out);
public:
    request_handler(const std::string& doc_root);

    void handle_request(const std::string& req, reply& reply);
};

#endif