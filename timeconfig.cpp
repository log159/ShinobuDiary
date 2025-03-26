#include "timeconfig.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

#define GETCURRENT(S)\
auto now = std::chrono::system_clock::now();\
auto timeT = std::chrono::system_clock::to_time_t(now);\
std::tm timeStruct;\
localtime_s(&timeStruct, &timeT);\
std::ostringstream oss;\
oss << std::put_time(&timeStruct, S);\
return oss.str();

std::string getCurrentTime() {

    auto now = std::chrono::system_clock::now(); 
    auto timeT = std::chrono::system_clock::to_time_t(now); 
    std::tm timeStruct; 
    localtime_s(&timeStruct, &timeT); 
    auto duration = now.time_since_epoch(); 
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000; 
    std::ostringstream oss; 
    oss << std::put_time(&timeStruct, "%Y-%m-%d %H:%M:%S"); 
    oss << "." << std::setfill('0') << std::setw(3) << milliseconds; 
    return oss.str();
}
std::string getCurrentYear(){
    GETCURRENT("%Y")
}
std::string getCurrentMonth(){
    GETCURRENT("%m")
}
std::string getCurrentDay(){
    GETCURRENT("%d")
}
std::string getCurrentHours(){
    GETCURRENT("%H")
}
std::string getCurrentMinutes(){
    GETCURRENT("%M")
}
std::string getCurrentSeconds(){
    GETCURRENT("%S")
}
