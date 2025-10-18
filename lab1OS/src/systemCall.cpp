#include "systemCall.h"
#include <iostream>

#ifdef _WIN32
#include <tchar.h>
#endif

bool PipeCreate(pipeT* pipe) {
    if (!pipe) {
        return false;
    }

#ifdef _WIN32
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    if (!CreatePipe(&pipe->read_end, &pipe->write_end, &sa, 0)) {
        return false;
    }
    
    return true;
#else
    int fds[2];
    if (::pipe(fds) == -1) {
        return false;
    }
    pipe_ptr->read_end = fds[0];
    pipe_ptr->write_end = fds[1];
    return true;
#endif
}

void PipeClose(pipeT* pipe) {
    if (!pipe) {
        return;
    }

#ifdef _WIN32
    if (pipe->read_end != INVALID_PIPE_HANDLE) {
        CloseHandle(pipe->read_end);
        pipe->read_end = INVALID_PIPE_HANDLE;
    }
    if (pipe->write_end != INVALID_PIPE_HANDLE) {
        CloseHandle(pipe->write_end);
        pipe->write_end = INVALID_PIPE_HANDLE;
    }
#else
    if (pipe->read_end != INVALID_PIPE_HANDLE) {
        close(pipe->read_end);
        pipe->read_end = INVALID_PIPE_HANDLE;
    }
    if (pipe->write_end != INVALID_PIPE_HANDLE) {
        close(pipe->write_end);
        pipe->write_end = INVALID_PIPE_HANDLE;
    }
#endif
}

void PipeCloseWriteEnd(pipeT* pipe) {
    if (!pipe) {
        return;
    }

#ifdef _WIN32
    if (pipe->write_end != INVALID_PIPE_HANDLE) {
        CloseHandle(pipe->write_end);
        pipe->write_end = INVALID_PIPE_HANDLE;
    }
#else
    if (pipe->write_end != INVALID_PIPE_HANDLE) {
        close(pipe->write_end);
        pipe->write_end = INVALID_PIPE_HANDLE;
    }
#endif
}

process ProcessCreate(const char* program, pipeT* stdin_pipe, pipeT* stdout_pipe) {
    process process_info;
    process_info.is_valid = false;

#ifdef _WIN32
    STARTUPINFOA si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    
    si.hStdInput = stdin_pipe ? stdin_pipe->read_end : GetStdHandle(STD_INPUT_HANDLE);
    si.hStdOutput = stdout_pipe ? stdout_pipe->write_end : GetStdHandle(STD_OUTPUT_HANDLE);
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    if (CreateProcessA(NULL, (LPSTR)program, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        process_info.process_info = pi;
        process_info.is_valid = true;
        CloseHandle(pi.hThread);
    }
#else
    pid_t pid = fork();
    if (pid == 0) {
        if (stdin_pipe) {
            dup2(stdin_pipe->read_end, STDIN_FILENO);
            close(stdin_pipe->read_end);
            close(stdin_pipe->write_end);
        }
        if (stdout_pipe) {
            dup2(stdout_pipe->write_end, STDOUT_FILENO);
            close(stdout_pipe->read_end);
            close(stdout_pipe->write_end);
        }
        
        execl(program, program, NULL);
        exit(1);
    } else if (pid > 0) {

        if (stdin_pipe) {
            close(stdin_pipe->read_end);  
        }
        if (stdout_pipe) {
            close(stdout_pipe->write_end); 
        }
        
        process_info.pid = pid;
        process_info.is_valid = true;
    }
#endif

    return process_info;
}

int ProcessTerminate(process* process_info) {
    if (!process_info || !process_info->is_valid) {
        return -1; 
    }

#ifdef _WIN32
    if (TerminateProcess(process_info->process_info.hProcess, 0)) {
        WaitForSingleObject(process_info->process_info.hProcess, INFINITE);
        CloseHandle(process_info->process_info.hProcess);
        process_info->is_valid = false;
        return 0;  
    } else {
        return -1; 
    }
#else
    if (kill(process_info->pid, SIGTERM) == 0) {
        int status;
        waitpid(process_info->pid, &status, 0);
        process_info->is_valid = false;
        return 0; 
    } else {
        return -1; 
    }
#endif
}

bool ReadStringFromPipe(PIPE_HANDLE pipe, std::string& output) {
    char buffer[1024];
    
#ifdef _WIN32
    DWORD bytes_read;
    if (ReadFile(pipe, buffer, sizeof(buffer) - 1, &bytes_read, NULL) && bytes_read > 0) {
        buffer[bytes_read] = '\0';
        output = buffer;
        return true;
    }
#else
    ssize_t bytes_read = read(pipe, buffer, sizeof(buffer) - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        output = buffer;
        return true;
    }
#endif
    
    return false;
}

bool WriteStringToPipe(PIPE_HANDLE pipe, const std::string& input) {
    if (input.empty()) {
        return true;
    }
    
#ifdef _WIN32
    DWORD bytes_written;
    return WriteFile(pipe, input.c_str(), input.length(), &bytes_written, NULL) && 
           bytes_written == input.length();
#else
    ssize_t bytes_written = write(pipe, input.c_str(), input.length());
    return bytes_written == (ssize_t)input.length();
#endif
}