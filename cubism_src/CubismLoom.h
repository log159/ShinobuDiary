#pragma once
#include <list>
#include <string>
#include "../global.h"

struct CsmMess {
	std::string group;
	std::string handle;
	std::string parameter;
};
//---------------------------------------------

static const char* start_mark = ":";
static const char* middle_mark = ",";
static const char* end_mark = ";";

//---------------------------------------------
static const char* window_group_mark = "Window";
static const char* window_style_mark="WindowStyle";

class CubismLoom
{
	static std::list<CsmMess> messageList;
public:
	static void addMessageList(const char* gm, const char* hm, const char* pm);
	static void handleMessageUpdate();
};