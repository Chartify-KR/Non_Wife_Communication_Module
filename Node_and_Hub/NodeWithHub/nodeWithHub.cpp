#include "./nodeWithHub.hpp"

NodeWithHub::NodeWithHub(const std::string& name, const Department& dep, const std::string& myIp){
    this->nodeInfo.userNmae = name;
    this->nodeInfo.department = dep;
    // this->nodeInfo.currentPath = std::filesystem::current_path();
    this->myIp = myIp;
}

static int connectDevices(clientInfo &serverInfo, clientInfo &localInfo, std::vector<clientInfo> &vec){
    int port = 8080;

    memset(&serverInfo, 0 , sizeof(clientInfo));
    serverInfo.socket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverInfo.socket < 0) {
        std::cerr << "Failed to create local socket" << std::endl;
        return -1;
    }
    serverInfo.address.sin_family = AF_INET;
    serverInfo.address.sin_addr.s_addr = htonl(INADDR_ANY);
    serverInfo.address.sin_port = htons(port);
    if (bind(serverInfo.socket, (struct sockaddr *)&serverInfo.address, sizeof(serverInfo.address)) < 0) {
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
            // if (vec.size() == 0){
            //     break;
            // }
        }
        if (vec.size() == 0){
            break;
        }
    }
    return 1;
}

int NodeWithHub::run(void){
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
        inet_ntop(AF_INET, &(LocalInfo.address.sin_addr), LocalInfo.clientIp, INET_ADDRSTRLEN);
        std::cout << "Client connected from " << LocalInfo.clientIp << ":" << ntohs(LocalInfo.address.sin_port) << std::endl;
        read(LocalInfo.socket, LocalInfo.header, 1);
        LocalInfo.header[4] = '\0';
        std::cout << "header: " << LocalInfo.header << std::endl;
        if ((newInfo.socket = accept(serverInfo.socket, (struct sockaddr *)&newInfo.address, (socklen_t*)&newInfo.addrlen))<0) {
            perror("accept");
            return -1;
        }
        std::cout << "Hedddddd" << std::endl;
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
    return 1;
}

void handleLocalConnection(clientInfo info, int dstSocket){
    std::string dataType;
    char *buffer = nullptr; 
    readHeader(info, buffer);
    dataType = receiveData(info, buffer);
    char* content = new char[strlen(buffer) + 1];
    strcpy(content, buffer);
    delete buffer;
    sendData(content, dataType, dstSocket);
    delete[] content;
}

void handleNodeConnection(clientInfo info, int dstSocket){
    std::string dataType;
    char *buffer = nullptr; 
    readHeader(info, buffer);
    dataType = receiveData(info, buffer);
    char* content = new char[strlen(buffer) + 1];
    strcpy(content, buffer);
    delete buffer;
    sendData(content, dataType, dstSocket);
    delete[] content;
}

void NodeWithHub::handleConnection(int sock){
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

int readHeader(clientInfo &info, char *buffer){
    std::string client_ip_str;
    inet_ntop(AF_INET, &(info.address.sin_addr), info.clientIp, INET_ADDRSTRLEN);
    std::cout << "Client connected from " << info.clientIp << ":" << ntohs(info.address.sin_port) << std::endl;
    read(info.socket, info.header, 1);
    info.header[4] = '\0';
    std::cout << "Header: " << info.header << std::endl;
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
    std::cout << "while : " << std::endl;
    while(1){

    }
    send(sock, dataType.c_str(), dataType.size(), 0);
    uint32_t dataLength = strlen(content);
    dataLength = htonl(dataLength);
    send(sock, &dataLength, sizeof(dataLength), 0);
    send(sock, content, strlen(content), 0);
}

std::string receiveData(clientInfo &info, char *buffer){
    // 데이터 수신 로직 구현
    // std::stringstream ss;
    std::string dataType;
    if (strcmp(info.header, "FILE") == 0){
        dataType = "FILE";
    }else if (strcmp(info.header, "TEXT") == 0){
        dataType = "TEXT";
    }
    std::cout << "File header is set" << std::endl;
    read(info.socket, buffer, sizeof(buffer));
    // if (bytesReceived != this->dataLength){
    //     std::cerr << "byteReceived: " << bytesReceived << "dataLength: " << this->dataLength << "which is not same" << std::endl;
    // }
    return dataType;
}
