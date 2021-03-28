#ifndef IO_SERVICE_POOL_HPP
#define IO_SERVICE_POOL_HPP

#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include <vector>
#include <iostream>

class io_service_pool: private boost::noncopyable
{
private:
    std::vector<boost::shared_ptr<boost::asio::io_service>> _io_services;
    std::vector<boost::shared_ptr<boost::asio::io_service::work>> _io_services_works;
    size_t _next_service;

public:
    io_service_pool(size_t pool_size);
    
    void run();
    void stop();
    boost::asio::io_service& get_service();
};

#endif