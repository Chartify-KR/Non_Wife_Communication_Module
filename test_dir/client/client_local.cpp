#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>

int main(int argc, char** argv) {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    char format;
    char file_buffer[1024];
    std::string dataType;
    std::string filename = argv[1];
    std::string fileMsg = "/Users/ojeongmin/Programming_study/Non_Wifi_Communication/test_dir/client/" + filename;
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
            dataType = "FILE";
            send(sock, dataType.c_str(), dataType.size(), 0);
            std::ifstream file(fileMsg, std::ios::binary);
            std::stringstream ss;
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
                std::string file_content = ss.str();
                send(sock, file_content.c_str(), file_content.size(), 0);
                file.close();
            }
            memset(file_buffer, 0, sizeof(file_buffer));
        }
        else if (format == 'n'){
            std::string string_data;
            u_int32_t data_length;
            dataType = "TEXT";
            send(sock, dataType.c_str(), dataType.size(), 0);
            
            std::cout << "You choose n, input your message: ";
            std::cin >> string_data;
            data_length = htonl(string_data.size());
            send(sock, &data_length, sizeof(data_length), 0);
            
            send(sock, string_data.c_str(), sizeof(buffer), 0);
            std::cout << "new message sent" << std::endl;
            valread = read(sock, buffer, 1024);
            std::cout << "Server: " << buffer << std::endl;
            memset(buffer, 0, sizeof(buffer));
            string_data.clear();
        }

    }

    return 0;
}