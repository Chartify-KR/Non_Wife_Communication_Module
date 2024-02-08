#include "nodeWithHub.hpp"

int main(int argc, char **argv, char **env){
    if (argc != 4) {
        std::cerr << "Argument number is not 2" << std::endl;
        return -1;
    }

    std::string Username = argv[1];
    int Dep = std::atoi(argv[2]);
    Department department = static_cast<Department>(Dep);
    std::string myIp = argv[3];

    NodeWithHub *node = new NodeWithHub(Username, department, myIp);
    if (node->run() == -1){
        std::cerr << "program failed" << std::endl;
        delete node;
        return (-1);
    }
    delete node;
    return (0);
}