#pragma once
#ifndef NODE_HPP
#define NODE_HPP

#include "./device.hpp"
#include <iostream>


class Node : public Device {

public:
    Node(std::string name, Department dep);

public:
    int run(void);
    void setSocket(void) override;
    void sendData(const std::string& data) override;
    void receiveData(const std::string& data) override;
};


#endif