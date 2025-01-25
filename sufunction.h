#pragma once
#include <iostream>
#include <string.h>
#include <string>
#include "timeconfig.h"
#include "sizedef.h"

namespace Su {

	//字符串操作函数
	void MemsetStr(char* str, size_t size);
	void GetGuiMark(char* markbuf, size_t size, const char* name, const char* id);

	//外部调用操作函数
	std::string GetLunar();
	std::string execCommandWithPipe(const std::string& command);

}