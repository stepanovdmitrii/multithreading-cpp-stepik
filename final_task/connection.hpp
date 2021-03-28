#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/bind.hpp>

#include <array>
#include <string>
#include <iostream>
#include <fstream>

#include "request_handler.hpp"

class connection: public boost::enable_shared_from_this<connection>, private boost::noncopyable
{
private:
    boost::asio::ip::tcp::socket _socket;
    request_handler& _handler;
    std::string _request;
    reply _reply;
    std::array<char, 32768> _buffer;

    void handle_read(const boost::system::error_code& e, size_t bytes_transferred);
    void handle_write(const boost::system::error_code& e);
    bool parse_request(size_t bytes_transferred);
public:
    explicit connection(boost::asio::io_service& io_service, request_handler& handler);

    boost::asio::ip::tcp::socket& socket();
    void start();
};

#endif