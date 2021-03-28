#include "connection.hpp"

connection::connection(boost::asio::io_service& io_service, request_handler& handler):
_socket(io_service), _handler(handler) {}

boost::asio::ip::tcp::socket& connection::socket() { return _socket; }

void connection::start(){
    _socket.async_read_some(boost::asio::buffer(_buffer),
    boost::bind(&connection::handle_read, shared_from_this(),
    boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void connection::handle_read(const boost::system::error_code& e, size_t bytes_transferred){
    if(!e){

        if(parse_request()){
            _handler.handle_request(_request, _reply);
            _reply.set_message();
            boost::asio::async_write(_socket, )
        }
    }
}