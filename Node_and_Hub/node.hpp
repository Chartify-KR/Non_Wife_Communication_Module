#pragma once
#ifndef NODE_HPP
#define NODE_HPP

#include "./device.hpp"
#include <iostream>


class Node : public Device {

public:
    Node(void);

public:
    void setSocket(void) override;
    void sendData(const std::string& data) override;
    void receiveData(const std::string& data) override;
};


#endif