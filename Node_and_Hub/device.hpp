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

class Device {

protected:
    struct sockaddr_in addr;
    int sock;
    char *buffer;

public:
    virtual ~Device() {}

    virtual void setSocket() = 0;

    // 데이터를 전송하는 함수
    virtual void sendData(const std::string& data) = 0;

    // 데이터를 수신하는 함수
    virtual void receiveData(const std::string& data) = 0;
};

#endif