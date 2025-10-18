#include "systemCall.h"
#include <iostream>
#include <string>

int main() {
    pipeT pipe1, pipe2, pipe3;
    process child1, child2;
    
    pipe1.read_end = INVALID_PIPE_HANDLE;
    pipe1.write_end = INVALID_PIPE_HANDLE;
    pipe2.read_end = INVALID_PIPE_HANDLE;
    pipe2.write_end = INVALID_PIPE_HANDLE;
    pipe3.read_end = INVALID_PIPE_HANDLE;
    pipe3.write_end = INVALID_PIPE_HANDLE;
    
    std::cout << "Creating pipes and processes..." << std::endl;
    
    if (!PipeCreate(&pipe1) || !PipeCreate(&pipe2) || !PipeCreate(&pipe3)) {
        std::cerr << "Failed to create pipes" << std::endl;
        return 1;
    }
    
    #ifdef _WIN32
        child1 = ProcessCreate("child1.exe", &pipe1, &pipe2);
        child2 = ProcessCreate("child2.exe", &pipe2, &pipe3);
#   else
        child1 = ProcessCreate("./child1", &pipe1, &pipe2);
        child2 = ProcessCreate("./child2", &pipe2, &pipe3);
    #endif
    
    if (!child1.is_valid || !child2.is_valid) {
        std::cerr << "Failed to create child processes" << std::endl;
        PipeClose(&pipe1);
        PipeClose(&pipe2);
        PipeClose(&pipe3);
        return 1;
    }
    
    std::cout << "Ready. Enter strings (empty line to exit):" << std::endl;
    
    std::string input;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);
        
        if (input.empty()) {
            break;
        }
        
        if (WriteStringToPipe(pipe1.write_end, input + "\n")) {
            std::string result;
            if (ReadStringFromPipe(pipe3.read_end, result)) {
                std::cout << "Result: " << result;
            } else {
                std::cerr << "Failed to read result" << std::endl;
                break;
            }
        } else {
            std::cerr << "Failed to send data" << std::endl;
            break;
        }
    }

    std::cout << "Program is ending..." << std::endl;

    PipeCloseWriteEnd(&pipe1);
    PipeCloseWriteEnd(&pipe2);
    PipeCloseWriteEnd(&pipe3);

    if (child1.is_valid) {
        std::cout << "exit_code for child1 -- " << ProcessTerminate(&child1) << std::endl;
    }
    if (child2.is_valid) {
        std::cout << "exit_code for child2 -- " << ProcessTerminate(&child2) << std::endl;
    }
    
    PipeClose(&pipe1);
    PipeClose(&pipe2);
    PipeClose(&pipe3);
    
    std::cout << "Program finished" << std::endl;
    return 0;
}