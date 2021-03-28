#include "io_service_pool.hpp"

io_service_pool::io_service_pool(size_t pool_size)
:_next_service(0){
    for(size_t idx = 0; idx < pool_size; ++idx){
        boost::shared_ptr<boost::asio::io_service> io_service(new boost::asio::io_service);
        boost::shared_ptr<boost::asio::io_service::work> io_service_work(new boost::asio::io_service::work(*io_service));
        _io_services.push_back(io_service);
        _io_services_works.push_back(io_service_work);
    }
}

void io_service_pool::run(){
    std::vector<boost::shared_ptr<boost::thread>> workers;
    for(size_t idx = 0; idx < _io_services.size(); ++idx){
        boost::shared_ptr<boost::thread> thread_ptr(new boost::thread(
            boost::bind(&boost::asio::io_service::run, _io_services[idx])));
        workers.push_back(thread_ptr);
    }

    for(size_t idx = 0; idx < workers.size(); ++idx){
        workers[idx]->join();
    }
}

void io_service_pool::stop(){
    for(size_t idx = 0; idx < _io_services.size(); ++idx){
        _io_services[idx]->stop();
    }
}

boost::asio::io_service& io_service_pool::get_service(){
    auto& result = *_io_services[_next_service];
    ++_next_service;
    if(_next_service == _io_services.size()){
        _next_service = 0;
    }
    return result;
}