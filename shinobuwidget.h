#pragma once
#include "global.h"
#include "suconfig.h"

extern void ShowShinobuWindow(bool* p_open);
extern void ShowShinobuHead();					/*标头*/
extern void ShowShinobuLunar();					/*日历*/
extern void ShowShinobuStart();					/*从这里开始*/
extern void ShowShinobuCubism();				/*虚拟角色界面*/
extern void ShowShinobuLanguage();              /*语言配置*/
extern void ShowShinobuGlobal();				/*全局配置*/
extern void ShowShinobuUser();					/*用户配置*/
extern void ShowShinobuCSM(Su::UserConfig* _uc);/*Live2d配置项*/
extern void ShowShinobuLLM(Su::UserConfig* _uc);/*LLM配置项*/
extern void ShowShinibuTTS(Su::UserConfig* _uc);/*TTS配置项*/
extern void ShowShinobuSTT(Su::UserConfig* _uc);/*STT配置项*/
extern void ShowShinobuMT(Su::UserConfig* _uc); /*MT配置项*/
extern void ShowShinobuHelp();                  /*帮助*/
extern void ShowShinobuAbout();                 /*关于*/
