#include "device.hpp"

 int Device::setSocket(void){
        if ((this->sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket create failed");
        return -1;
    }
    if (setsockopt(this->sock_fd, SOL_SOCKET, SO_REUSEADDR, &(this->opt), sizeof((this->opt)))) {
        perror("setsockopt");
        return -1;
    }
    this->addr.sin_family = AF_INET;
    this->addr.sin_addr.s_addr = INADDR_ANY;
    this->addr.sin_port = htons(8080);

    // // Forcefully attaching socket to the port 8080
    if (bind(this->sock_fd, (struct sockaddr *)&(this->addr), sizeof(this->addr))<0) {
        perror("bind failed");
        return -1;
    }

    if (listen(this->sock_fd, MAX_REQUEST_NUMBER) < 0) {
        perror("listen");
        return -1;
    }
}

int Device::readySocket(void){
    socklen_t client_addr_len = sizeof(this->local_addr);
    std::cout << "Server is ready!" << std::endl; 

    this->new_socket = accept(this->sock_fd, (struct sockaddr *)&(this->local_addr), &client_addr_len);
    if (this->new_socket < 0) {
        perror("accept");
        return -1;
    }
    return 1;
}

int Device::readHeader(void){
    std::string client_ip_str;

    inet_ntop(AF_INET, &(this->local_addr.sin_addr), this->client_ip, INET_ADDRSTRLEN);
    std::cout << "Client connected from " << this->client_ip << ":" << ntohs(this->local_addr.sin_port) << std::endl;
    read(this->new_socket, this->header, 1);
    this->header[4] = '\0';

    // read length of data that client has sent. And then set the buffer.
    read(this->new_socket, &(this->dataLength), sizeof(this->dataLength));
    this->dataLength = ntohl(this->dataLength);
    this->buffer = new char[this->dataLength + 1];
    this->buffer[this->dataLength] = '\0';
    client_ip_str = this->client_ip;
    if (client_ip_str.compare("127.0.0.1") == 0){
        this->local_socket = this->new_socket;
        this->external_socket = -1;
        return 0;
    }
    this->external_socket = this->new_socket;
    this->local_socket = -1;
    return 1;
}

int Device::makeDirectory(void){
    std::string currentPath = this->nodeInfo.currentPath;
    currentPath += "/receiveData"; 
    if (mkdir(currentPath.c_str(), 0666) == -1){
        if (errno == EEXIST)
            std::cerr << "dir is exist" << std::endl;
    }
    return 0;
}