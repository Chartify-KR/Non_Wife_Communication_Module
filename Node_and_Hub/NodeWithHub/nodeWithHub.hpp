#pragma once
#ifndef NODEWITHHUB_HPP
#define NODEWITHHUB_HPP

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
    
    // void sendTextData(std::string &string_content) override;

    int receiveIpFromHub(void);
    int setSocketForNode(void);
    int sendPingPongToNode(void);
    int setSocketForLocal(void);
    void handleConnection(int sock);
    
    // void receiveDataFromExternal(void) override;
public:
    int broadcastIpToNode(void);

};


char *readHeader(clientInfo &info);
void sendData(const char* content, std::string &dataType, int &sock);
void handleLocalConnection(clientInfo info, int dstSocket);
void handleNodeConnection(clientInfo info, int dstSocket);
#endif