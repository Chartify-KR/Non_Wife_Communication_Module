#pragma once
#ifndef DEVICE_HPP
# define DEVICE_HPP

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>

#define MAX_REQUEST_NUMBER 200

enum Department{
    PreExamination = 1,
    MainExamination = 2
};

typedef struct nodeInfo_s{
    std::string ip;
    int port;
    std::string userNmae;
    Department department;
} NodeInfo;

class Device {

    protected:
        struct sockaddr_in addr;
        int sock_fd;
        int opt;
        char *buffer;
        NodeInfo nodeInfo;

    public:
        virtual ~Device() {}
        

        virtual int run(void) = 0;
        int setSocket(void);

        // 데이터를 전송하는 함수
        virtual void sendData(const std::string& data) = 0;

        // 데이터를 수신하는 함수
        virtual void receiveData(const std::string& data) = 0;
};

#endif