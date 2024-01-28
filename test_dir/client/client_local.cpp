#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>

int main() {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    char format;
    char file_buffer[1024];

    while (1){
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            std::cout << "Socket creation error" << std::endl;
            return -1;
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(8080);

        // Convert IPv4 and IPv6 addresses from text to binary form
        if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
            std::cout << "Invalid address/ Address not supported" << std::endl;
            return -1;
        }

        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            std::cout << "Connection Failed" << std::endl;
            return -1;
        }
        
        
        std::cout << "Choose message format. Do you want to prefer file: (Y/n)";
        std::cin >> format;

        if (format == 'y'){
            send(sock, "FILE", 4, 0);
            std::ifstream file("/Users/ojeongmin/Programming_study/Non_Wifi_Communication/test_dir/client/test.json", std::ios::binary);
            std::stringstream ss;
            // file.seekg(0, std::ios::beg);
            ss << file.rdbuf();
            if (!ss.eof()) {
                std::cout << "success" << std::endl;
            } else {
                if (file.eof()) {
                    std::cout << "End of file reached" << std::endl;
                } else {
                    std::cerr << "File read error" << std::endl;
                }
            }

            if (file.is_open()) {
                // while (file.read(file_buffer, sizeof(file_buffer))) {
                //     std::cout << "debug2" << std::endl;
                //     send(sock, file_buffer, file.gcount(), 0);
                // }
                std::string file_content = ss.str();
                send(sock, file_content.c_str(), file_content.size(), 0);
                file.close();
            }
            memset(file_buffer, 0, sizeof(file_buffer));
        }
        else if (format == 'n'){
            std::cout << "You choose n, input your message: ";
            std::cin >> buffer;
            send(sock, buffer, strlen(buffer), 0);
            std::cout << "new message sent" << std::endl;
            valread = read(sock, buffer, 1024);
            std::cout << "Server: " << buffer << std::endl;

            // Close the socket
            // close(sock);
            memset(buffer, 0, strlen(buffer));
        }

    }

    return 0;
}
