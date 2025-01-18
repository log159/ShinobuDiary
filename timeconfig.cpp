#include "timeconfig.h"

std::string getCurrentTime() {
    // 获取当前时间点
    auto now = std::chrono::system_clock::now();

    // 转换为 time_t，用于提取年月日时分秒
    auto timeT = std::chrono::system_clock::to_time_t(now);

    // 提取时间结构（线程安全）
    std::tm timeStruct;
#ifdef _WIN32
    localtime_s(&timeStruct, &timeT); // Windows 平台线程安全方式
#else
    localtime_r(&timeT, &timeStruct); // POSIX 平台线程安全方式
#endif

    // 提取毫秒部分
    auto duration = now.time_since_epoch();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000;

    // 格式化时间
    std::ostringstream oss;
    oss << std::put_time(&timeStruct, "%Y-%m-%d %H:%M:%S");
    oss << "." << std::setfill('0') << std::setw(3) << milliseconds;
    return oss.str();
}