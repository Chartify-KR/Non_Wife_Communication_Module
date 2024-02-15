#include "./node.hpp"

Node::Node(const std::string& name, const Department& dep, const std::string& myIp){
    this->nodeInfo.userNmae = name;
    this->nodeInfo.department = dep;
    // this->nodeInfo.currentPath = std::filesystem::current_path();
    this->myIp = myIp;
}

// int Node::run(void){
//     if (this->broadcastIpToNode() == -1){
//         std::cerr << "Broadcasting is failed" << std::endl;
//         return -1;
//     }
//     if (this->setSocketForNode() == -1 || this->setSocketForLocal() == -1){ // set socket for receving data.
//         std::cerr << "Running socket is failed" << std::endl;
//         return -1;
//     }
//     if (this->sendPingPongToNode() == -1 || this->sendPingPongToLocal() == -1){ // Node will play as hub.
//         std::cerr << "send is failed" << std::endl;
//         return -1;
//     }
//     while (true){
//         if (this->acceptSocket() == -1){ 
//             std::cerr << "readySocket is failed" << std::endl;
//         }
//         std::thread t(handleConnection, this->new_socket);
//     }
//     return 1;
// }

static int connectDevices(clientInfo &serverInfo, clientInfo &localInfo, std::vector<clientInfo> &vec){
    int port = 8080;
    int opt = 1;

    memset(&serverInfo, 0 , sizeof(clientInfo));
    serverInfo.socket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverInfo.socket < 0) {
        std::cerr << "Failed to create local socket" << std::endl;
        return -1;
    }
    serverInfo.address.sin_family = AF_INET;
    serverInfo.address.sin_addr.s_addr = htonl(INADDR_ANY);
    serverInfo.address.sin_port = htons(port);
    if (bind(serverInfo.socket, (struct sockaddr *)&serverInfo.address, sizeof(clientInfo)) < 0) {
        std::cerr << "Failed to bind external socket" << std::endl;
        return -1;
    }
    if (listen(serverInfo.socket, SOMAXCONN) < 0) {
        std::cerr << "Failed to listen on local socket" << std::endl;
        return -1;
    }
    while (true){
        clientInfo deviceInfo;
        if ((deviceInfo.socket = accept(serverInfo.socket, (struct sockaddr *)&deviceInfo.address, (socklen_t*)&deviceInfo.addrlen))<0) {
            perror("accept");
            return -1;
        }
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &deviceInfo.address.sin_addr, client_ip, INET_ADDRSTRLEN);
        strcpy(deviceInfo.clientIp, client_ip);
        if (strcmp("127.0.0.1", client_ip) == 0){
            localInfo = deviceInfo;
            std::cout << "Here we go local" << std::endl;
        }else{
            vec.push_back(deviceInfo);
            if (vec.size() == 1){
                break;
            }
        }
    }
    return 1;
}

int Node::run(void){
    if (this->broadcastIpToNode() == -1){
        std::cerr << "Broadcasting is failed" << std::endl;
        return -1;
    }
    clientInfo serverInfo;
    clientInfo LocalInfo;
    std::vector<clientInfo> infoVector;
    if (connectDevices(serverInfo, LocalInfo, infoVector) == -1){
        std::cerr << "Error" << std::endl;
        return -1;
    }
    while (true){
        clientInfo newInfo;
        if ((newInfo.socket = accept(serverInfo.socket, (struct sockaddr *)&newInfo.address, (socklen_t*)&newInfo.addrlen))<0) {
            perror("accept");
            return -1;
        }
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &newInfo.address.sin_addr, client_ip, INET_ADDRSTRLEN);
        if (strcmp(LocalInfo.clientIp, client_ip) == 0){
            std::thread t(handleLocalConnection, LocalInfo, infoVector[1].socket);
            t.detach();
        }else{
            std::thread t(handleNodeConnection, infoVector[1], LocalInfo.socket);
            t.detach();
        }

    }
}

void handleLocalConnection(clientInfo info, int dstSocket){
    std::string dataType;
    char *buffer; 
    readHeader(info, buffer);
    dataType = receiveData(info, buffer);
    char* content = new char[strlen(buffer) + 1];
    strcpy(content, buffer);
    delete buffer;
    sendData(content, dataType, dstSocket);
    delete content;
}

void handleNodeConnection(clientInfo info, int dstSocket){
    std::string dataType;
    char *buffer; 
    readHeader(info, buffer);
    dataType = receiveData(info, buffer);
    char* content = new char[strlen(buffer) + 1];
    strcpy(content, buffer);
    delete buffer;
    sendData(content, dataType, dstSocket);
    delete content;
}

void Node::handleConnection(int sock){
}

int Node::broadcastIpToNode(void){
    int sockfd;
    struct sockaddr_in broadcastAddr; // 브로드캐스트 주소 구조체

    if (this->nodeInfo.department == PreExamination){
        this->myIp += ",1";
    }else{
        this->myIp += ",2";
    }
    const char* msg = this->myIp.c_str();
    // UDP 소켓 생성
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        return -1;
    }
    
    // 브로드캐스트를 활성화
    int broadcastEnable=1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0) {
        perror("Error in setting Broadcast option");
        return -1;
    }
    // 브로드캐스트 주소 설정
    memset(&broadcastAddr, 0, sizeof(broadcastAddr));
    broadcastAddr.sin_family = AF_INET;
    broadcastAddr.sin_port = htons(8080); // 대상 포트
    broadcastAddr.sin_addr.s_addr = inet_addr("255.255.255.255"); // 브로드캐스트 주소
    // 메시지 보내기
    if (sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr *)&broadcastAddr, sizeof(broadcastAddr)) < 0) {
        perror("sendto failed");
        return -1;
    }
    std::cout << "Broadcast message sent.\n";
    close(sockfd);
    return 0;
}

int readHeader(clientInfo &info, char *buffer){
    std::string client_ip_str;
    inet_ntop(AF_INET, &(info.address.sin_addr), info.clientIp, INET_ADDRSTRLEN);
    // std::cout << "Client connected from " << this->client_ip << ":" << ntohs(this->local_addr.sin_port) << std::endl;
    read(info.socket, info.header, 1);
    info.header[4] = '\0';
    uint32_t dataLength;
    // read length of data that client has sent. And then set the buffer.
    read(info.socket, &(dataLength), dataLength);
    dataLength = ntohl(dataLength);
    buffer = new char[dataLength + 1];
    buffer[dataLength] = '\0';
    client_ip_str = info.clientIp;
    return 1;
}

void sendData(const char* content, std::string &dataType, int &sock){
    // 데이터 전송 로직 구현
    send(sock, dataType.c_str(), dataType.size(), 0);
    uint32_t dataLength = strlen(content);
    dataLength = htonl(dataLength);
    send(sock, &dataLength, sizeof(dataLength), 0);
    send(sock, content, strlen(content), 0);
}

// int Node::sendPingPongToNode(void){
//     char ping[5] = "PING";
//     char pong[5];


//     send(this->node_socket, ping, sizeof(ping), 0);
//     std::cout << "Ping is sended to Hub" << std::endl;
//     read(this->node_socket, pong, sizeof(pong));
//     pong[4] = '\0';
//     if (strcmp(pong, "PONG") == 0){
//         std::cout << "Pong is sended from Hub";
//     }else{
//         std::cerr << "Hub: " << pong << ", connection failed" << std::endl;
//         exit(1);
//     }
//     return 1;
// }

static void pingPongThreadConnection(){

}

// int Node::sendPingPongToLocal(void){
//     char localPing[6]; 
//     char localPong[6] = "LOCAL";
//     char nodePing[5];
//     char nodePong[5] = "NODE";

//     socklen_t addrlen = sizeof(this->local_addr);
//     while (true){
//         if (this->local_socket = accept(this->local_socket, (struct sockaddr *)&(this->local_addr), &addrlen)){
//         perror("accept");
//         exit(EXIT_FAILURE);
//         }

//         std::thread t(pingPongThreadConnection, this->local_socket);
//     }
   
//     read(this->local_socket, ping, sizeof(ping));
//     ping[4] = '\0';
//     if (strcmp(ping, "LOCAL") == 0){
//         std::cout << "Ping is sended from DingDong: " << ping << std::endl;
//     }else{
//         std::cerr << "Hub: " << pong << ", connection failed" << std::endl;
//         exit(1);
//     }
//     send(this->local_socket, pong, sizeof(pong), 0);
//     std::cout << "Pong is sended" << std::endl; 
//     pong[4] = '\0';
//     return 1;
// }


std::string receiveData(clientInfo &info, char *buffer){
    // 데이터 수신 로직 구현
    int bytesReceived;
    // std::stringstream ss;
    std::string dataType;
    if (strcmp(info.header, "FILE") == 0){
        dataType = "FILE";
    }else if (strcmp(info.header, "TEXT") == 0){
        dataType = "TEXT";
    }
    std::cout << "File header is set" << std::endl;
    bytesReceived = read(info.socket, buffer, sizeof(buffer));
    // if (bytesReceived != this->dataLength){
    //     std::cerr << "byteReceived: " << bytesReceived << "dataLength: " << this->dataLength << "which is not same" << std::endl;
    // }
    return dataType;
}