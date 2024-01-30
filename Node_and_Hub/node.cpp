#include "./node.hpp"

Node::Node(void){
    
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