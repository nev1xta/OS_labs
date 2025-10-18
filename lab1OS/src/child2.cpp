#include <iostream>
#include <string>
#include <cctype>

int main() {
    std::string line;
    while (std::getline(std::cin, line)) {
        std::string result;
        bool prev_space = false;
        
        for (char c : line) {
            if (std::isspace(c)) {
                if (!prev_space) {
                    result += c;
                    prev_space = true;
                }
            } else {
                result += c;
                prev_space = false;
            }
        }
        std::cout << result << std::endl;
    }
    return 0;
}