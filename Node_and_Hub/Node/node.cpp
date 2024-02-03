#include "./node.hpp"

Node::Node(std::string name, Department dep){
    this->nodeInfo.userNmae = name;
    this->nodeInfo.department = dep;
}

int Node::run(void){
    int ret;

    if (ret = setSocket() == -1){
        std::cerr << "Running socket failed" << std::endl;
        return -1;
    }
    while (true){
        
    }
    return 1;
}

void Node::sendData(const std::string& data){
    // 데이터 전송 로직 구현
    std::cout << "Node sending data: " << data << std::endl;
}

void Node::receiveData(const std::string& data){
    // 데이터 수신 로직 구현
    std::cout << "Node received data: " << data << std::endl;
}