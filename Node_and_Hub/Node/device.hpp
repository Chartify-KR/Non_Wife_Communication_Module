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
#define LOCALPORT 8080
#define HUBPORT 8081



enum Department{
    PreExamination = 1,
    MainExamination = 2
};

typedef struct clientInfo_s{
    int socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char clientIp[16];
    char header[5];
    std::string dataType;
    
} clientInfo;

typedef struct nodeInfo_s{
    std::string ip;
    std::__fs::filesystem::path currentPath;
    std::string userNmae;
    Department department;
} NodeInfo;

class Device {

    protected:
        int sock_fd;
        int opt;
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
        int acceptSocket(void);
        int makeDirectory(void);
};      


#endif