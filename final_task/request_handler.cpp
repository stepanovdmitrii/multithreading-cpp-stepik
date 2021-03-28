#include "request_handler.hpp"

request_handler::request_handler(const std::string& doc_root)
:_doc_root(doc_root) {
}

bool request_handler::url_decode(const std::string& in, std::string& out){
    out.clear();
    out.reserve(in.size());
    for(size_t i = 0; i < in.size(); ++i){
        if(in[i] == '%'){
            if(i + 3 <= in.size()){
                int value = 0;
                std::istringstream iss(in.substr(i + 1, 2));
                if(iss >> std::hex >> value){
                    out += static_cast<char>(value);
                    i+= 2;
                }
                else{
                    return false;
                }
            }
            else{
                return false;
            }
        }
        else if(in[i] == '+') {
            out += ' ';
        }
        else{
            out += in[i];
        }
    }
    return true;
}

void request_handler::handle_request(const std::string& request, reply& reply){
    std::string path;
    if(false == url_decode(request, path)){
        reply.found = false;
        reply.content.clear();
        return;
    }

    if(path.empty() || path.find("..") != std::string::npos || path[0] != '/'){
        reply.found = false;
        reply.content.clear();
        return;
    }

    if(path.size() == 1 && path[0] == '/'){
        path = "/index.html";
    }
    std::string full_path = _doc_root + path;
    std::ifstream file(full_path, std::ios::in | std::ios::binary);
    if(!file){
        reply.found = false;
        reply.content.clear();
        return;
    }

    reply.found = true;
    char buf[4096];
    while (file.read(buf, 4096).gcount() > 0)
    {
        reply.content.append(buf, file.gcount());
    }
}