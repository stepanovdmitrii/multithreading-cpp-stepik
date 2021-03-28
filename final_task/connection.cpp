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
        if(parse_request(bytes_transferred)){
            _handler.handle_request(_request, _reply);
            _reply.set_message();
            std::vector<boost::asio::const_buffer> buffers;
            buffers.push_back(boost::asio::const_buffer(_reply.message.c_str(), _reply.message.size()));
            boost::asio::async_write(_socket, buffers,
                boost::bind(&connection::handle_write, shared_from_this(), boost::asio::placeholders::error));
        }
    }
}

void connection::handle_write(const boost::system::error_code& e){
    if(!e){
        boost::system::error_code ignored_ec;
        _socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
    }
}

bool connection::parse_request(size_t bytes_transferred){
    std::stringstream ss;
    for(size_t i = 0; i < bytes_transferred; ++i){
        std::cout << (char)_buffer[i];
        ss << _buffer[i];
    }
    std::cout<<std::endl;
    ss.seekp(0, std::ios_base::end);
    std::string method;
    ss >> method;
    if(method != "GET") return false;
    std::string path;
    ss >> path;
    size_t q = path.find('?');
    if(q != std::string::npos){
        path = path.substr(0,q);
    }
    _request = path;
    return true;
}