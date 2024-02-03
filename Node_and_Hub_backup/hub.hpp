#pragma once
#ifndef HUB_HPP
#define HUB_HPP

#include "./device.hpp"
#include <iostream>

class Hub : public Device {
public:
    void setSocket() override;
    void sendData(const std::string& data) override;
    void receiveData(const std::string& data) override;
};


#endif