#pragma once
#ifndef NODE_HPP
#define NODE_HPP

#include "./device.hpp"
#include <iostream>


class NodeWithHub : public Device {

private:
    struct sockaddr_in hub_addr;
    std::string hubIp;
    int hub_socket;
    std::string myIp;

public:
    NodeWithHub(const std::string& name, const Department& dep, const std::string& myIp);

public:
    int run(void);
    void sendDataToLocal(const char* content, std::string &dataType) override;
    void sendDataToHub(const char* content, std::string &dataType) override;
    // void sendTextData(std::string &string_content) override;

    int receiveIpFromHub(void);
    int setSocketForHub(void);
    int setSocketForLocal(void) override;
    int readHeader(void) override;
    std::string receiveData(void) override;
    // void receiveDataFromExternal(void) override;
public:
    int broadcastIpToNode(void);

};



#endif