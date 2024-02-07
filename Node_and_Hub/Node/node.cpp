#include "./node.hpp"

Node::Node(std::string name, Department dep){
    this->nodeInfo.userNmae = name;
    this->nodeInfo.department = dep;
    this->nodeInfo.currentPath = std::filesystem::current_path();
}

int Node::run(void){
    if (this->setSocket() == -1){ // set socket for receving data.
        std::cerr << "Running socket is failed" << std::endl;
        return -1;
    }
    if (this->setSocketForHub() == -1 || this->setSocketForLocal() == -1){
        std::cerr << "send is failed" << std::endl;
        exit(1);
    }
    std::string dataType;

    while (true){
        if (this->acceptSocket() == -1){ 
            std::cerr << "readySocket is failed" << std::endl;
        }
        // read header to know whether this message is from local or external and file or string.
        this->readHeader();
        dataType = this->receiveData();
        char* content = new char[strlen(this->buffer) + 1];
        strcpy(content, this->buffer);
        delete this->buffer;
        if (this->new_socket == this->local_socket){ // socket is from local. Therefore, send data to Hub
            sendDataToHub(content, dataType);
        }else{ // // socket is from Hub. Therefore, send data to Local
            sendDataToLocal(content, dataType);
        }
        delete content;
    }
    return 1;
}

int Node::readHeader(void){
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
    return 1;
}

int Node::setSocketForHub(void){
    const char* hub_address_text = "??";
    char ping[5] = "PING";
    char pong[5];

    if ((this->hub_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }
    this->hub_addr.sin_family = AF_INET;
    this->hub_addr.sin_port = htons(HUBPORT);

    if (inet_pton(AF_INET, hub_address_text, &(this->hub_addr.sin_addr)) <= 0){
        std::cerr << "Invaild address / Address not supported" << std::endl;
        return -1;
    }
    if (connect(this->hub_socket, (struct sockaddr *)&(this->hub_addr), sizeof(this->hub_addr)) < 0){
        std::cerr << "Connection failed" << std::endl;
        return -1;
    }
    send(this->hub_socket, ping, sizeof(ping), 0);
    std::cout << "Ping is sended to Hub" << std::endl;
    read(this->hub_socket, pong, sizeof(pong));
    pong[4] = '\0';
    if (strcmp(pong, "PONG") == 0){
        std::cout << "Pong is sended from Hub";
    }else{
        std::cerr << "Hub: " << pong << ", connection failed" << std::endl;
        exit(1);
    }
    return 1;
}

int Node::setSocketForLocal(void){
    const char* localhost = "127.0.0.1";
    char ping[5] = "PING";
    char pong[5];
    
    if (this->local_socket = socket(AF_INET, SOCK_STREAM, 0) < 0){
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }
    this->local_addr.sin_family = AF_INET;
    this->local_addr.sin_port = htons(LOCALPORT);
    if (inet_pton(AF_INET, localhost, &(this->local_addr.sin_addr)) <= 0){
        std::cerr << "Invaild address / Address not supported" << std::endl;
        return -1;
    }
    if (connect(this->local_socket, (struct sockaddr *)&(this->local_addr), sizeof(this->local_addr)) < 0){
        std::cerr << "Connection failed" << std::endl;
        return -1;
    }
    send(this->local_socket, ping, sizeof(ping), 0);
    std::cout << "Ping is sended to Hub" << std::endl;
    read(this->local_socket, pong, sizeof(pong));
    pong[4] = '\0';
    if (strcmp(pong, "PONG") == 0){
        std::cout << "Pong is sended from Hub";
    }else{
        std::cerr << "Hub: " << pong << ", connection failed" << std::endl;
        exit(1);
    }
    return 1;

}

void Node::sendDataToHub(const char* content, std::string &dataType){
    // 데이터 전송 로직 구현
    send(this->hub_socket, dataType.c_str(), dataType.size(), 0);
    uint32_t dataLength = htonl(this->dataLength);
    send(this->hub_socket, &dataLength, sizeof(dataLength), 0);
    send(this->hub_socket, content, strlen(content), 0);
}

void Node::sendDataToLocal(const char* content, std::string &dataType){
    // 데이터 전송 로직 구현
    send(this->local_socket, dataType.c_str(), dataType.size(), 0);
    uint32_t dataLength = htonl(this->dataLength);
    send(this->local_socket, &dataLength, sizeof(dataLength), 0);
    send(this->local_socket, content, strlen(content), 0);
}

std::string Node::receiveData(void){
    // 데이터 수신 로직 구현
    int bytesReceived;
    // std::stringstream ss;
    std::string dataType;
    if (strcmp(this->header, "FILE") == 0){
        dataType = "FILE";
    }else if (strcmp(this->header, "TEXT") == 0){
        dataType = "TEXT";
    }
    std::cout << "File header is set" << std::endl;
    bytesReceived = read(this->local_socket, this->buffer, sizeof(buffer));
    if (bytesReceived != this->dataLength){
        std::cerr << "byteReceived: " << bytesReceived << "dataLength: " << this->dataLength << "which is not same" << std::endl;
    }
    // ss << this->buffer;
    // if (!ss.eof()){
    //     std::cout << "successively read File data" << std::endl;
    // }
    // std::string content = ss.str();
    // dataType = "FILE";
    // this->sendData(content, dataType);
    return dataType;
    //std::cout << "Node received data: " << data << std::endl;
}

// void Node::receiveDataFromExternal(void){

// }

// void Node::sendTextData(std::string &string_content){
//     if (this->setSocketForSend() == -1){
//         std::cerr << "send is failed" << std::endl;
//         delete this->buffer;
//         exit(1);
//     }
//     std::string dataType = "TEXT";

// }