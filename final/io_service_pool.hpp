#ifndef IO_SERVICE_POOL_HPP
#define IO_SERVICE_POOL_HPP

#include <boost/core/noncopyable.hpp>
#include <boost/asio.hpp>

#include <vector>
#include <memory>
#include <thread>

class io_service_pool: private boost::noncopyable
{
private:
    std::vector<std::shared_ptr<boost::asio::io_service>> _io_services;
    std::vector<std::shared_ptr<boost::asio::io_service::work>> _io_services_works;
    size_t _next_service;

public:
    io_service_pool(size_t pool_size);
    
    void run();
    void stop();
    boost::asio::io_service& get_service();
};

#endif