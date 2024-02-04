#pragma once
#ifndef NODE_HPP
#define NODE_HPP

#include "./device.hpp"
#include <iostream>


class Node : public Device {

private:
    int hub_socket;
    struct sockaddr_in hub_addr;

public:
    Node(std::string name, Department dep);

public:
    int run(void);
    void sendData(std::string &content, std::string &dataType) override;
    // void sendTextData(std::string &string_content) override;
    int setSocketForSend(void) override;
    void receiveDataFromLocal() override;
};



#endif