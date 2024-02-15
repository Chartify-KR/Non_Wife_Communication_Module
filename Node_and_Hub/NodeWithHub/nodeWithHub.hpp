#pragma once
#ifndef NODE_HPP
#define NODE_HPP

#include "./device.hpp"
#include <iostream>
#include <vector>
#include <thread>


class NodeWithHub : public Device {

private:
    std::string hubIp;
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
    int readHeader(int &sock);
    std::string receiveData(void) override;
    void handleConnection(int sock);
    
    void handleLocalConnection(clientInfo info);
    void handleNodeConnection(clientInfo info);
    // void receiveDataFromExternal(void) override;
public:
    int broadcastIpToNode(void);

};



#endif