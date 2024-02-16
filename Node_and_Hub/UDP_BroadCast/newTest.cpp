#include <fstream>
#include <iostream>
#include <boost/asio.hpp>

void exampleFileRead() {
    std::ifstream file("example.txt");
    if (file.is_open()) {
        std::string line;
        while (getline(file, line)) {
            std::cout << line << std::endl;
        }
        file.close();
    }
}

void exampleSocketRead(boost::asio::ip::tcp::socket& socket) {
    boost::system::error_code ec;
    boost::asio::streambuf buf;
    boost::asio::read(socket, buf, ec);
    if (!ec) {
        std::cout << &buf << std::endl;
    }
    socket.close();
}
