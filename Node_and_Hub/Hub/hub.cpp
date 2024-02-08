#include "hub.hpp"

Hub::Hub(std::string userName, Department dep){
    this->nodeInfo.userNmae = userName;
    this->nodeInfo.department = dep;
    this->nodeInfo.currentPath = std::filesystem::current_path(); 
}

