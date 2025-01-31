#include "sufunction.h"
#include <algorithm>
#include <stdio.h>
#include <direct.h>
#include <Windows.h>
#include <map>
#include "global.h"

namespace Su {
    void MemsetStr(char* str, size_t size) {
        memset(str, 0, size);
    }

    void GetGuiMark(char* markbuf, size_t size, const char* name, const char* id) {
        MemsetStr(markbuf, size);
        strcpy_s(markbuf, size, name);
        strcat_s(markbuf, size, "##");
        strcat_s(markbuf, size, id);
    }

    void StringSplit(std::string str, const char split,std::vector<std::string>& sv)
    {
        sv.clear();
        std::istringstream iss(str);	    // 输入流
        std::string token;			        // 接收缓冲区
        while (getline(iss, token, split))	// 以split为分隔符
            sv.push_back(token);
    }
    std::string replaceNewlinesInRange(const std::string& input, char start, char end) {
        std::string result = input;
        size_t startIdx = result.find(start);
        while (startIdx != std::string::npos) {
            size_t endIdx = result.find(end, startIdx);
            if (endIdx == std::string::npos) {
                break;
            }
            for (size_t i = startIdx; i < endIdx; ++i) {
                if (result[i] == '\n') {
                    result[i] = ' ';
                }
            }
            startIdx = result.find(start, endIdx + 1);
        }
        return result;
    }
    std::string GetLunar()
    {
        static char way[DEFPATHSIZE];
        memset(way, 0, sizeof(way));
        char* gd=_getcwd(way, sizeof(way));(void*)gd;
        std::string exeway(way);
        std::replace(exeway.begin(), exeway.end(), '\\', '/');

        std::string command = exeway + "/lunar/lunar.exe";
        command += (" " + ::getCurrentYear());
        command += (" " + ::getCurrentMonth());
        command += (" " + ::getCurrentDay());
        command += (" " + ::getCurrentHours());
        command += (" " + ::getCurrentMinutes());
        command += (" " + ::getCurrentSeconds());

        try {
            std::string output = execCommandWithPipe(command);
            std::replace(output.begin(), output.end(),' ', '\n');
            output=replaceNewlinesInRange(output, '[', ']');
            return output;
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return "Error";
        }
        return "Error";
    }

    std::string execCommandWithPipe(const std::string& command) {
        HANDLE hReadPipe, hWritePipe;
        SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE };
        if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0)) {
            throw std::runtime_error("Failed to create pipe");
        }
        PROCESS_INFORMATION pi{};
        STARTUPINFOA si{};
        si.cb = sizeof(STARTUPINFOA);
        si.dwFlags = STARTF_USESTDHANDLES;
        si.hStdOutput = hWritePipe;
        si.hStdError = hWritePipe;

        // 创建子进程
        if (!CreateProcessA(nullptr, const_cast<char*>(command.c_str()), nullptr, nullptr, TRUE, 0, nullptr, nullptr, &si, &pi)) {
            CloseHandle(hReadPipe);
            CloseHandle(hWritePipe);
            throw std::runtime_error("Failed to create process");
        }

        // 关闭写入端
        CloseHandle(hWritePipe);

        // 读取子进程输出
        static char buffer[DEFSIZEK16];
        memset(buffer, 0, sizeof(buffer));
        DWORD bytesRead;
        std::string result;
        while (ReadFile(hReadPipe, buffer, sizeof(buffer) - 1, &bytesRead, nullptr) && bytesRead > 0) {
            buffer[bytesRead] = '\0';
            result += buffer;
        }
        CloseHandle(hReadPipe);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return result;
    }

}
