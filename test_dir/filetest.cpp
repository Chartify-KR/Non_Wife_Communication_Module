#include <filesystem>
#include <iostream>
#include <paths.h>
namespace fs = std::filesystem;
 
int main()
{
    fs::path pth = fs::current_path();
    std::cout << "Current path is " << pth << '\n'; // (1)
    fs::current_path(fs::temp_directory_path()); // (3)
    std::cout << "Current path is " << fs::current_path() << '\n';
}