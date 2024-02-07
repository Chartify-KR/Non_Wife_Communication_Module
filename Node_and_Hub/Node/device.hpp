#pragma once
#ifndef DEVICE_HPP
# define DEVICE_HPP

#include <iostream>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>
#include <filesystem>

#define MAX_REQUEST_NUMBER 200
#define LOCAL_SOCKET 1
#define EXTERNAL_SOCKET 2
enum Department{
    PreExamination = 1,
    MainExamination = 2
};


typedef struct nodeInfo_s{
    std::string ip;
    int port;
    std::filesystem::path currentPath;
    std::string userNmae;
    Department department;
} NodeInfo;

class Device {

    protected:
        struct sockaddr_in addr;
        struct sockaddr_in local_addr;
        int sock_fd;
        int opt;
        int local_socket;
        int new_socket;
        char header[5];
        char client_ip[INET_ADDRSTRLEN]; 
        char *buffer;
        std::string sender;
        uint32_t dataLength;
        NodeInfo nodeInfo;

    public:
        virtual ~Device() {}
        

        virtual int run(void) = 0;
        int setSocket(void);
        int readySocket(void);
        int makeDirectory(void);
        // 데이터를 전송하는 함수
        virtual int readHeader(void) = 0;
        virtual void sendDataToLocal(const char* content, std::string &dataType, int &dest_socket) = 0;
        virtual void sendDataToHub(const char* content, std::string &dataType) = 0;
        // virtual void sendTextData(std::string &string_content) = 0;
        // 데이터를 수신하는 함수
        virtual std::string receiveData(void) = 0;
        // virtual void receiveDataFromExternal(void) = 0;
};      


#endif