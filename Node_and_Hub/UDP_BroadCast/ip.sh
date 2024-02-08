#!/bin/bash

# macOS에서 Wi-Fi 인터페이스(en0)의 IP 주소를 얻습니다.
IP_ADDR=$(ifconfig en0 | grep inet | grep -v inet6 | awk '{print $2}')

# IP 주소를 출력합니다.
echo "My IP address is: $IP_ADDR"

./send "$IP_ADDR"
