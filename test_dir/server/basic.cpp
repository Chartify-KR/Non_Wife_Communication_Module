#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <cstring>
#include <arpa/inet.h>


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

    while (1){
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);

        std::cout << "Server is ready!" << std::endl;
        new_socket = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (new_socket < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        
        char client_ip[INET_ADDRSTRLEN]; // INET_ADDRSTRLEN = 16
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        std::cout << "Client connected from " << client_ip << ":" << ntohs(client_addr.sin_port) << std::endl;

        read(new_socket, header, 4);
        header[4] = '\0';
        if (strcmp(header, "FILE!") == 0){
            std::cout << "file header is set" << std::endl;
            std::ofstream file("/Users/ojeongmin/Programming_study/Non_Wifi_Communication/test_dir/server/output.json", std::ios::binary);
            int bytesReceived;
            std::stringstream ss;
            if (file.is_open()) {
                std::cout << "debug1" << std::endl;
                bytesReceived = read(new_socket, buffer, sizeof(buffer));
                ss << buffer;
                file << ss.rdbuf();
                std::string str = ss.str().c_str();
                std::cout << str.size() << std::endl;
                file.close();
            }
        }
        else{
            uint32_t dataLength;
            read(new_socket, &dataLength, sizeof(dataLength));
            dataLength = ntohl(dataLength);
            std::cout << "data len: " << dataLength << std::endl;
            char* data_buffer = new char[dataLength + 1];
            read(new_socket, data_buffer, dataLength);
            data_buffer[dataLength] = '\0';

            // valread = read(new_socket , data_buffer, 1024);
            std::cout << "data buffer: " << data_buffer << std::endl;
            send(new_socket , message , sizeof(message) , 0 );
            printf("Hello message sent\n");
            delete[] data_buffer;
        }
        memset(buffer, 0, sizeof(buffer));
    }


    return 0;
}