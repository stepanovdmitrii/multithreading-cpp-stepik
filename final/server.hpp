#ifndef SERVER_HPP
#define SERVER_HPP

#include <boost/core/noncopyable.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include <string>
#include <memory>

#include "io_service_pool.hpp"
#include "request_handler.hpp"
#include "connection.hpp"

class server: private boost::noncopyable
{
    private:
        io_service_pool _io_service_pool;
        boost::asio::signal_set _signals;
        boost::asio::ip::tcp::acceptor _acceptor;
        request_handler _handler;
        std::shared_ptr<connection> _next_connection;

        void start_accept();
        void handle_accept(const boost::system::error_code& e);
        void handle_stop();

    public:
        explicit server(const std::string& address, std::string& port, const std::string& dir, size_t threads_count);

        void run();
};


#endif