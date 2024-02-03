#pragma once
#ifndef NODE_HPP
#define NODE_HPP

#include "./device.hpp"
#include <iostream>


class Node : public Device {

private:
    int hub_socket;

public:
    Node(std::string name, Department dep);

public:
    int run(void);
    void sendData(const std::string& data) override;
    void receiveData(const std::string& data) override;
};



#endif