#include "./nodeWithHub.hpp"

NodeWithHub::NodeWithHub(const std::string& name, const Department& dep, const std::string& myIp){
    this->nodeInfo.userNmae = name;
    this->nodeInfo.department = dep;
    // this->nodeInfo.currentPath = std::filesystem::current_path();
    this->myIp = myIp;
}

 int NodeWithHub::setSocketForLocal(void){
    if ((this->local_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket create failed");
        return -1;
    }
    if (setsockopt(this->sock_fd, SOL_SOCKET, SO_REUSEADDR, &(this->opt), sizeof((this->opt)))) {
        perror("setsockopt");
        return -1;
    }
    this->local_addr.sin_family = AF_INET;
    this->local_addr.sin_addr.s_addr = INADDR_ANY;
    this->local_addr.sin_port = htons(8080);

    // // Forcefully attaching socket to the port 8080
    if (bind(this->local_socket, (struct sockaddr *)&(this->local_addr), sizeof(this->local_addr))<0) {
        perror("bind failed");
        return -1;
    }

    if (listen(this->local_socket, MAX_REQUEST_NUMBER) < 0) {
        perror("listen");
        return -1;
    }
}

int NodeWithHub::setSocketForNode(void){
    if ((this->node_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket create failed");
        return -1;
    }
    if (setsockopt(this->node_socket, SOL_SOCKET, SO_REUSEADDR, &(this->opt), sizeof((this->opt)))) {
        perror("setsockopt");
        return -1;
    }
    this->local_addr.sin_family = AF_INET;
    this->local_addr.sin_addr.s_addr = INADDR_ANY;
    this->local_addr.sin_port = htons(8080);

    // // Forcefully attaching socket to the port 8080
    if (bind(this->node_socket, (struct sockaddr *)&(this->node_addr), sizeof(this->node_addr))<0) {
        perror("bind failed");
        return -1;
    }

    if (listen(this->node_socket, MAX_REQUEST_NUMBER) < 0) {
        perror("listen");
        return -1;
    }
}



// int NodeWithHub::run(void){
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

int NodeWithHub::run(void){
    if (this->broadcastIpToNode() == -1){
        std::cerr << "Broadcasting is failed" << std::endl;
        return -1;
    }
    int port = 8080;
    while (true){
        int opt = 1;
        int server_fd;
        clientInfo info;

        memset(&info, 0 , sizeof(clientInfo));
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd < 0) {
            std::cerr << "Failed to create local socket" << std::endl;
            return -1;
        }
        info.address.sin_family = AF_INET;
        info.address.sin_addr.s_addr = htonl(INADDR_ANY);
        info.address.sin_port = htons(port++);
        if (bind(server_fd, (struct sockaddr *)&info.address, sizeof(clientInfo)) < 0) {
            std::cerr << "Failed to bind external socket" << std::endl;
            return -1;
        }
        if (listen(server_fd, SOMAXCONN) < 0) {
            std::cerr << "Failed to listen on local socket" << std::endl;
            return -1;
        }
        if ((info.socket = accept(server_fd, (struct sockaddr *)&info.address, (socklen_t*)&info.addrlen))<0) {
            perror("accept");
            return -1;
        }
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &info.address.sin_addr, client_ip, INET_ADDRSTRLEN);
        strcpy(info.clientIp, client_ip);
        if (strcmp("127.0.0.1", client_ip) == 0){
            std::thread localThread(handleLocalConnection, info);
            localThread.detach();
        }else{
            std::thread nodeThread(handleNodeConnection, info);
            nodeThread.detach();
        }

    }
}

void NodeWithHub::handleLocalConnection(clientInfo info){

}

void NodeWithHub::handleNodeConnection(clientInfo info){
    
}

void NodeWithHub::handleConnection(int sock){

        std::string dataType;
        this->readHeader();
        dataType = this->receiveData();
        char* content = new char[strlen(this->buffer) + 1];
        strcpy(content, this->buffer);
        delete this->buffer;
        if (sock == this->local_socket){ // socket is from local. Therefore, send data to Hub
            sendDataToHub(content, dataType);
        }else{ // // socket is from Hub. Therefore, send data to Local
            sendDataToLocal(content, dataType);
        }
        delete content;
}

int NodeWithHub::broadcastIpToNode(void){
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

int NodeWithHub::readHeader(clientInfo &info){
    std::string client_ip_str;
    char header[4];

    inet_ntop(AF_INET, &(info.address.sin_addr), info.clientIp, INET_ADDRSTRLEN);
    // std::cout << "Client connected from " << this->client_ip << ":" << ntohs(this->local_addr.sin_port) << std::endl;
    read(info.socket, header, 1);
    header[4] = '\0';

    uint32_t dataLength;
    // read length of data that client has sent. And then set the buffer.
    read(info.socket, (dataLength), dataLength);
    this->dataLength = ntohl(this->dataLength);
    this->buffer = new char[this->dataLength + 1];
    this->buffer[this->dataLength] = '\0';
    client_ip_str = this->client_ip;
    return 1;
}



int NodeWithHub::sendPingPongToNode(void){
    char ping[5] = "PING";
    char pong[5];


    send(this->node_socket, ping, sizeof(ping), 0);
    std::cout << "Ping is sended to Hub" << std::endl;
    read(this->node_socket, pong, sizeof(pong));
    pong[4] = '\0';
    if (strcmp(pong, "PONG") == 0){
        std::cout << "Pong is sended from Hub";
    }else{
        std::cerr << "Hub: " << pong << ", connection failed" << std::endl;
        exit(1);
    }
    return 1;
}

static void pingPongThreadConnection(){

}

int NodeWithHub::sendPingPongToLocal(void){
    char localPing[6]; 
    char localPong[6] = "LOCAL";
    char nodePing[5];
    char nodePong[5] = "NODE";

    socklen_t addrlen = sizeof(this->local_addr);
    while (true){
        if (this->local_socket = accept(this->local_socket, (struct sockaddr *)&(this->local_addr), &addrlen)){
        perror("accept");
        exit(EXIT_FAILURE);
        }

        std::thread t(pingPongThreadConnection, this->local_socket);
    }
   
    read(this->local_socket, ping, sizeof(ping));
    ping[4] = '\0';
    if (strcmp(ping, "LOCAL") == 0){
        std::cout << "Ping is sended from DingDong: " << ping << std::endl;
    }else{
        std::cerr << "Hub: " << pong << ", connection failed" << std::endl;
        exit(1);
    }
    send(this->local_socket, pong, sizeof(pong), 0);
    std::cout << "Pong is sended" << std::endl; 
    pong[4] = '\0';
    return 1;
}

void NodeWithHub::sendDataToHub(const char* content, std::string &dataType){
    // 데이터 전송 로직 구현
    send(this->node_socket, dataType.c_str(), dataType.size(), 0);
    uint32_t dataLength = htonl(this->dataLength);
    send(this->node_socket, &dataLength, sizeof(dataLength), 0);
    send(this->node_socket, content, strlen(content), 0);
}

void NodeWithHub::sendDataToLocal(const char* content, std::string &dataType){
    // 데이터 전송 로직 구현
    send(this->local_socket, dataType.c_str(), dataType.size(), 0);
    uint32_t dataLength = htonl(this->dataLength);
    send(this->local_socket, &dataLength, sizeof(dataLength), 0);
    send(this->local_socket, content, strlen(content), 0);
}

std::string NodeWithHub::receiveData(void){
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
    //std::cout << "NodeWithHub received data: " << data << std::endl;
}