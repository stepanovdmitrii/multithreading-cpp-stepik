#include "server.hpp"

server::server(const std::string& address, std::string& port, const std::string& dir, size_t threads_count):
_io_service_pool(threads_count),
_signals()
{

}