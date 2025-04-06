#pragma once
#include "./global.h"
#include "./suconfig.h"

enum SUERROR : int{
	CUBISM_NOT_RUNNING
};
struct ShinobuExList;

extern void ShowShinobuWindow(bool* p_open);
extern void ShowShinobuHead();					/*标头*/
extern void ShowShinobuStart();					/*从这里开始*/
extern void ShowShinobuStyleEditor();			/*样式界面(独立)*/
extern void ShowShinobuInteractively();			/*交互界面(独立)*/
extern void ShowShinobuCubism();				/*虚拟角色界面*/
extern void ShowShinobuLanguage();              /*语言配置*/
extern void ShowShinobuStyle();					/*样式配置*/
extern void ShowShinobuGlobal();				/*全局配置*/
extern void ShowShinobuUser();					/*用户配置*/
extern void ShowShinobuHelp();                  /*项目帮助*/
extern void ShowShinobuAbout();                 /*项目关于*/
extern void ShowShinobuErrorWindow();			/*错误内容*/
extern void ShowShinobuDebugWindow();			/*输出内容*/


extern void ShowShinobuError(SUERROR se);									/*错误内容输出*/
extern void ShowShinobuCSM(Su::UserConfig* _uc);							/*Live2d配置项*/
extern void ShowShinobuLLM(Su::UserConfig* _uc);							/*LLM配置项*/
extern void ShowShinibuTTS(Su::UserConfig* _uc);							/*TTS配置项*/
extern void ShowShinobuSTT(Su::UserConfig* _uc);							/*STT配置项*/
extern void ShowShinobuMT(Su::UserConfig* _uc);								/*MT配置项*/
extern void ShowShinobuCsmGeometry(Su::UserConfig& uc,LAppModel* lam);
extern void ShowShinobuCsmLook(Su::UserConfig& uc, LAppModel* lam);
extern void ShowShinobuCsmBreath(Su::UserConfig& uc, LAppModel* lam);
extern void ShowShinobuCsmBlink(Su::UserConfig& uc, LAppModel* lam);
extern void ShowShinobuCsmAnimation(Su::UserConfig& uc, LAppModel* lam);
extern void ShowShinobuCsmHitareas(Su::UserConfig& uc, LAppModel* lam);
extern void ShowShinobuCsmRendering(Su::UserConfig& uc, LAppModel* lam);
extern void ShowShinobuCsmObserve(Su::UserConfig& uc, LAppModel* lam);
extern void ShowShinobuCsmObserveSeparate(Su::UserConfig& uc, LAppModel* lam);
extern void ShowShinobuCsmObserveItems(Su::UserConfig& uc, LAppModel* lam);

//shinobu func
extern void RemoveUserHandle(int re_userid);
extern void AddMotionStart(int uid, int gid, int nid);
extern void AddExpressionStart(int uid, int nid);


