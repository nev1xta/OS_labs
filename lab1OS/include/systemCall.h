#ifndef SYSTEMCALL_H
#define SYSTEMCALL_H

#include <string>

#ifdef _WIN32
    #include <windows.h>
    #define PIPE_HANDLE HANDLE
    #define INVALID_PIPE_HANDLE INVALID_HANDLE_VALUE
#else
    #include <unistd.h>
    #include <sys/wait.h>
    #define PIPE_HANDLE int
    #define INVALID_PIPE_HANDLE -1
#endif

typedef struct {
    PIPE_HANDLE read_end;
    PIPE_HANDLE write_end;
} pipeT;

typedef struct {
#ifdef _WIN32
    PROCESS_INFORMATION process_info;
#else
    pid_t pid;
#endif
    bool is_valid;
} process;

bool PipeCreate(pipeT* pipe);
void PipeClose(pipeT* pipe);
void PipeCloseWriteEnd(pipeT* pipe);

process ProcessCreate(const char* program, pipeT* stdin_pipe, pipeT* stdout_pipe);
int ProcessTerminate(process* process_info);

bool ReadStringFromPipe(PIPE_HANDLE pipe, std::string& output);
bool WriteStringToPipe(PIPE_HANDLE pipe, const std::string& input);

#endif