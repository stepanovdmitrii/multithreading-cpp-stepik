#include "server.hpp"

server::server(const std::string& address, std::string& port, const std::string& dir, size_t threads_count):
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
    _signals.async_wait([this](){this->handle_stop();});

    boost::asio::ip::tcp::resolver resolver(_acceptor.get_executor());
    boost::asio::ip::tcp::resolver::query query(address, port);
    boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
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
    _next_connection.reset(new connection())
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