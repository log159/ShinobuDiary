#include "CubismLoom.h"
#include <iostream>
std::list<CsmMess> CubismLoom::messageList;
void CubismLoom::addMessageList(const char* gm, const char* hm, const char* pm)
{
	//添加新消息
	CsmMess cm;
	cm.group = std::string(gm);
	cm.handle = std::string(hm);
	cm.parameter = std::string(pm);
	messageList.push_back(cm);

	//然后处理完毕
	while (!messageList.empty()) {
		handleMessageUpdate();
	}
}

void CubismLoom::handleMessageUpdate()
{
	if (messageList.empty())
		return;
	CsmMess cm = messageList.front();
	messageList.pop_front();

	std::cout <<u8"处理消息：" << cm.group << " " << cm.handle << " " << cm.parameter << std::endl;

	if (strcmp(cm.group.c_str(), window_group_mark) == 0) {
		if (strcmp(cm.handle.c_str(), window_style_mark) == 0) {
			// 置顶不穿透
			if (cm.parameter == "0")GlobalConfig::getInstance()->window_cubism_style_id = 0;
			// 穿透不置顶
			else if (cm.parameter == "1")GlobalConfig::getInstance()->window_cubism_style_id = 1;
			// 置顶并且穿透
			else if (cm.parameter == "2")GlobalConfig::getInstance()->window_cubism_style_id = 2;
			// 不置顶并且不穿透
			else if (cm.parameter == "3")GlobalConfig::getInstance()->window_cubism_style_id = 3;
		}
	}
}
