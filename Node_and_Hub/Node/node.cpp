#include "./node.hpp"

Node::Node(std::string name, Department dep){
    this->nodeInfo.userNmae = name;
    this->nodeInfo.department = dep;
    this->nodeInfo.currentPath = std::filesystem::current_path();
}

int Node::run(void){
    if (this->setSocket() == -1){
        std::cerr << "Running socket is failed" << std::endl;
        return -1;
    }
    while (true){
        if (this->readySocket() == -1){
            std::cerr << "readySocket is failed" << std::endl;
        }
        // read header to know whether this message is from local or external and file or string.
        if (this->readHeader() == 0){ // socket is from local
            this->receiveDataFromLocal();                                                                                                                    
        }else{ // socket is from external(hub)
            //
        }
    }
    return 1;
}


int Node::setSocketForSend(void){
    char* const hub_address_text = "??";

    if ((this->hub_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }
    this->hub_addr.sin_family = AF_INET;
    this->hub_addr.sin_port = htons(8081);

    if (inet_pton(AF_INET, hub_address_text, &(this->hub_addr.sin_addr)) <= 0){
        std::cerr << "Invaild address / Address not supported" << std::endl;
        return -1;
    }
    if (connect(this->hub_socket, (struct sockaddr *)&(this->hub_addr), sizeof(this->hub_addr)) < 0){
        std::cerr << "Connection failed" << std::endl;
    }
    return 1;
}

void Node::sendData(std::string &content, std::string &dataType){
    // 데이터 전송 로직 구현
    if (this->setSocketForSend() == -1){
        std::cerr << "send is failed" << std::endl;
        delete this->buffer;
        exit(1);
    }
    std::string dataType = "FILE";
    send(this->hub_socket, dataType.c_str(), dataType.size(), 0);
    uint32_t dataLengthOfFile = htonl(this->dataLength);
    send(this->hub_socket, &dataLengthOfFile, sizeof(dataLengthOfFile), 0);
    send(this->hub_socket, content.c_str(), content.size(), 0);
    delete this->buffer;
}


// void Node::sendTextData(std::string &string_content){
//     if (this->setSocketForSend() == -1){
//         std::cerr << "send is failed" << std::endl;
//         delete this->buffer;
//         exit(1);
//     }
//     std::string dataType = "TEXT";

// }


void Node::receiveDataFromLocal(void){
    // 데이터 수신 로직 구현
    int bytesReceived;
    std::stringstream ss;
    std::string dataType;
    if (strcmp(this->header, "FILE") == 0){
        std::cout << "File header is set" << std::endl;
        bytesReceived = read(this->local_socket, this->buffer, sizeof(buffer));
        if (bytesReceived != this->dataLength){
            std::cerr << "byteReceived: " << bytesReceived << "dataLength: " << this->dataLength << "which is not same" << std::endl;
        }
        ss << this->buffer;
        if (!ss.eof()){
            std::cout << "successively read File data" << std::endl;
        }
        std::string content = ss.str();
        dataType = "FILE";
        this->sendData(content, dataType);
        //std::ofstream file("/Users/ojeongmin/Programming_study/Non_Wifi_Communication/test_dir/server/output.json", std::ios::binary);
    }else if (strcmp(this->header, "TEXT") == 0){
        std::cout << "File header is set" << std::endl;
        bytesReceived = read(this->local_socket, this->buffer, sizeof(buffer));
        if (bytesReceived != this->dataLength){
            std::cerr << "byteReceived: " << bytesReceived << "dataLength: " << this->dataLength << "which is not same" << std::endl;
        }
        ss << this->buffer;
        if (!ss.eof()){
            std::cout << "successively read Text data" << std::endl;
        }
        std::string content = ss.str();
        dataType = "TEXT";
        this->sendData(content, dataType);
    }
    //std::cout << "Node received data: " << data << std::endl;
}