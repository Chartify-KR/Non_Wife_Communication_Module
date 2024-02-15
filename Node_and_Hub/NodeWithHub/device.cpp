#include "device.hpp"

int Device::makeDirectory(void){
    std::string currentPath = this->nodeInfo.currentPath;
    currentPath += "/receiveData"; 
    if (mkdir(currentPath.c_str(), 0666) == -1){
        if (errno == EEXIST)
            std::cerr << "dir is exist" << std::endl;
    }
    return 0;
}