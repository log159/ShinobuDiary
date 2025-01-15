#pragma once
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "suconfig.h"
#include "compoundgui.h"
#include "translator.h"
#include "llmhome.h"
#include <chrono>
#include <thread>

extern void ShowShinobuWindow(bool* p_open);
extern void ShowShinobuHead();					/*标头*/
extern void ShowShinobuStart();					/*从这里开始*/
extern void ShowShinobuLanguage();              /*语言设置*/
extern void ShowShinobuStyle();					/*样式设置*/
extern void ShowShinobuUser();					/*用户信息*/
extern void ShowShinobuLLM(Su::UserConfig* _uc);/*LLM配置项*/
extern void ShowShinibuTTS(Su::UserConfig* _uc);/*TTS配置项*/
extern void ShowShinobuSTT(Su::UserConfig* _uc);/*STT配置项*/
extern void ShowShinobuMT(Su::UserConfig* _uc); /*MT配置项*/
extern void ShowShinobuHelp();                  /*帮助*/
extern void ShowShinobuAbout();                 /*关于*/
