#pragma once
#ifndef NODE_HPP
#define NODE_HPP

#include "./device.hpp"
#include <iostream>


class Node : public Device {

private:
    struct sockaddr_in hub_addr;
    int hub_socket;

public:
    Node(std::string name, Department dep);

public:
    int run(void);
    void sendDataToLocal(const char* content, std::string &dataType, int &dest_socket) override;
    void sendDataToHub(const char* content, std::string &dataType) override;
    // void sendTextData(std::string &string_content) override;

    int setSocketForHub(void);
    int readHeader(void) override;
    std::string receiveData(void) override;
    // void receiveDataFromExternal(void) override;
};



#endif