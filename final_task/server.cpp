#include "server.hpp"

server::server(const std::string& address, int port, const std::string& dir, size_t threads_count):
_io_service_pool(threads_count),
_signals(_io_service_pool.get_service()),
_acceptor(_io_service_pool.get_service()),
_next_connection(),
_handler(dir)
{
    _signals.add(SIGINT);
    _signals.add(SIGTERM);
    #if defined(SIGQUIT)
    _signals.add(SIGQUIT);
    #endif
    _signals.async_wait(boost::bind(&server::handle_stop, this));
    boost::asio::ip::address_v4 addr = boost::asio::ip::address_v4::from_string(address);
    boost::asio::ip::tcp::endpoint endpoint(addr, port);
    _acceptor.open(endpoint.protocol());
    _acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    _acceptor.bind(endpoint);
    _acceptor.listen();

    start_accept();
}

void server::run(){
    _io_service_pool.run();
}

void server::start_accept(){
    _next_connection.reset(new connection(_io_service_pool.get_service(), _handler));
    _acceptor.async_accept(_next_connection->socket(),
        boost::bind(&server::handle_accept, this, boost::asio::placeholders::error));
}

void server::handle_accept(const boost::system::error_code& e){
    if(!e){
        _next_connection->start();
    }

    start_accept();
}

void server::handle_stop(){
    _io_service_pool.stop();
}