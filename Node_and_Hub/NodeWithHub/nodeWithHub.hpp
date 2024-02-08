#pragma once
#ifndef NODE_HPP
#define NODE_HPP

#include "./device.hpp"
#include <iostream>


class NodeWithHub : public Device {

private:
    struct sockaddr_in node_addr;
    std::string hubIp;
    int node_socket;
    std::string myIp;

public:
    NodeWithHub(const std::string& name, const Department& dep, const std::string& myIp);

public:
    int run(void);
    void sendDataToLocal(const char* content, std::string &dataType) override;
    void sendDataToHub(const char* content, std::string &dataType) override;
    // void sendTextData(std::string &string_content) override;

    int receiveIpFromHub(void);
    int setSocketForNode(void);
    int sendPingPongToLocal(void) override;
    int sendPingPongToNode(void);
    int setSocketForLocal(void);
    int readHeader(void) override;
    std::string receiveData(void) override;
    // void receiveDataFromExternal(void) override;
public:
    int broadcastIpToNode(void);

};



#endif