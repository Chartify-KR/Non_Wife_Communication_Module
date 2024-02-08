#pragma once
#ifndef HUB_HPP
#define HUB_HPP

#include "./device.hpp"
#include <iostream>
#include <vector>

class Hub : public Device {

private:
    std::vector<nodeInfo_s> node;

public:
    Hub(std::string userName, Department dep);

public:
    void setSocket() override;
    void sendData(const std::string& data) override;
    void receiveData(const std::string& data) override;
};


#endif