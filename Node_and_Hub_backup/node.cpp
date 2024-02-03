#include "./node.hpp"

Node::Node(std::string name, Department dep){
    this->nodeInfo.userNmae = name;
    this->nodeInfo.department = dep;
}

int Node::run(void){
    while (true){
        if ((this->sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            std::cout << "Socket creation error" << std::endl;
            return -1;
        }

    }
}

void Node::setSocket(void) {

}

void Node::sendData(const std::string& data){
    // 데이터 전송 로직 구현
    std::cout << "Node sending data: " << data << std::endl;
}

void Node::receiveData(const std::string& data){
    // 데이터 수신 로직 구현
    std::cout << "Node received data: " << data << std::endl;
}