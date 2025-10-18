#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>

int main() {
    std::string line;
    while (std::getline(std::cin, line)) {
        for (char& c : line) {
            c = std::tolower(c);
        }
        std::cout << line << std::endl;
    }
    return 0;
}