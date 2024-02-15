#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstring>
#include <arpa/inet.h>
#include <thread>

void handleConnection(int sock) {
    std::cout << "here" << std::endl;
    // 클라이언트와의 통신 처리
    char buffer[1024] = {0};
    char header[5] = {0};
    std::string message = "Hello from server!!!!!!!!!!!";
    read(sock, header, 1024);
    
    uint32_t dataLength;
    read(sock, &dataLength, sizeof(dataLength));
    dataLength = ntohl(dataLength);
    std::cout << "data len: " << dataLength << std::endl;
    char* data_buffer = new char[dataLength + 1];
    read(sock, data_buffer, dataLength);
    data_buffer[dataLength] = '\0';

    // valread = read(new_socket , data_buffer, 1024);
    std::cout << "data buffer: " << data_buffer << std::endl;
    send(sock , message.c_str() , sizeof(message) , 0 );
    std::cout << "Hello message sented" << std::endl;
    delete[] data_buffer;
    std::cout << "thread is detached" << std::endl;
    close(sock);
}

int main() {
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    const char *message = "Hello from server";
    char header[5];

    // printf("sin_len = %u\n", address.sin_len);
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) { //socket을 만드는 함수, AF_INET ipv4 address family를 사용할 예정이다. 
        perror("socket failed"); //SOCK_STREAM 은 TCP를 프로토콜로 사용하고 마지막 0은 프로토콜을 AF_INET에 알맞게 설정
        exit(EXIT_FAILURE);
    }
    // printf("server_fd = %d\n", server_fd);
    
    // // Forcefully attaching socket to the port 8080
    int ret = (SO_REUSEADDR | SO_REUSEPORT);
    // printf("SO_REUSEADDR = %d\nSO_REUSEPORT = %d\n result = %d,   &opt = %p\n", SO_REUSEADDR, SO_REUSEPORT, ret, &opt);
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    // printf("------------after-----------\n");
    // printf("&opt = %p\n", &opt);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    // // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

//    std::vector<std::thread> threads; // 클라이언트 처리를 위한 스레드 저장

    while(true) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
            perror("accept");
            continue;
        }

        // 새 스레드를 생성하여 클라이언트와의 통신을 처리
        // threads.push_back(std::move(std::thread(handleConnection, new_socket)));
        std::thread t(handleConnection, new_socket);
        t.detach();
    }

    // 모든 스레드가 종료될 때까지 기다림
    // for (std::thread &th : threads) {
    //     if (th.joinable()) {
    //         th.join();
    //         std::cout << "threads are joined" << std::endl;
    //     }
    // }
    return 0;
}