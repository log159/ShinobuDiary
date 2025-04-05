#pragma once
#include <map>
#include <queue>
#include "imgui.h"
#include "./filesetting.h"
#include "./sizedef.h"
#include "./somemacros.h"
#include "./sufunction.h"
enum class LAN : int;
#define MARK_LIST \
    X(LANGUAGESEL_ID) \
    X(USER_NUM) \
    X(THEMESEL_ID) \
    X(FONTSEL_NAME) \
    X(LLMSEL_ID) \
    X(TTSSEL_ID) \
    X(STTSEL_ID) \
    X(MTSEL_ID) \
    X(VITS_SIMPLE_API_MODEL_ID) \
    X(VITS_SIMPLE_API_SPEAKER_ID) \
    X(VITS_SIMPLE_API_EMOTION_ID) \
    X(GPT_SOVITS_TARGET_LANGUAGE_ID) \
    X(MYSELF_VITS_URL) \
    X(FONT_GLOBAL_SCALE) \
    X(WINDOW_MAIN_CLOSE) \
    X(WINDOW_MAIN_ALPHA) \
    X(WINDOW_MAIN_STYLE_ID) \
    X(WINDOW_MAIN_DOCK_ID) \
    X(WINDOW_MAIN_TRANSPARENT_ID) \
    X(WINDOW_MAIN_FORECASTFPS) \
    X(WINDOW_MAIN_ADDTIMEFPS) \
    X(WINDOW_MAIN_FAST_ID) \
    X(WINDOW_CUBISM_STYLE_ID) \
    X(WINDOW_CUBISM_ADDTIMEFPS) \
    X(ENABLE_WIDGET) \
    X(ENABLE_CUBISM) \
    X(ENABLE_TEMPLATE) \
    X(ENABLE_TTS) \
    X(ENABLE_MT) \
    X(ENABLE_ORIGINAL) \
    X(ENABLE_STT) \
    X(CUBISM_CONFIG_MODEL_DIR) \
    X(CUBISM_CG_CUBISM_TS_S) \
    X(CUBISM_CG_CUBISM_TX_T) \
    X(CUBISM_CG_CUBISM_TY_T) \
    X(CUBISM_CG_CUBISM_TS_X) \
    X(CUBISM_CG_CUBISM_TS_Y) \
    X(CUBISM_CG_CUBISM_TX_S) \
    X(CUBISM_CG_CUBISM_TY_S) \
    X(CUBISM_CG_CUBISM_TX_P) \
    X(CUBISM_CG_CUBISM_TY_P) \
    X(KEY) \
    X(APPID) \
    X(SECRET) \
    X(BASEURL) \
    X(ADDRESS) \
    X(PORT)

enum INIMARK {
#define X(name) name,
    MARK_LIST
#undef X
};
static std::map<INIMARK, const char*> inimark_map = {
#define X(name) {INIMARK::name, #name},
    MARK_LIST
#undef X
};

static const char* INIGROUPMARKSTR = "INIMARKGROUP%d";
static const char* vits_simple_api_model_list[] = { "vits","bert-vits2","w2v2-vits" };
static const char* gpt_sovits_target_language_list[] = { "zh","ja","en" };

class GlobalTemp {
public:
    static std::ostringstream                               DebugOss;
    static bool                                             ShowStyleEditor;
    static int                                              CubismFrameCount;
    static HWND                                             WindowMainHandle;
    static WNDCLASSEXW                                      WindowMainWc;
    static bool                                             RefreshTable;
    static bool                                             RefreshCubism;
    static const LPCWSTR                                    CubismDirectXClassName;
    static const LPCWSTR                                    CubismWindowClassName;
    static std::queue<std::pair<int, std::string>>          CubismModelMessage;
    static std::queue<std::pair<int, std::pair<int, int>>>  CubismMotionMessage;
    static std::queue<std::pair<int, int>>                  CubismExpressionMessage;
    static bool                                             CubismIsRunning;
    static bool                                             TableBools[DEFSIZE][DEFSIZEK16];
    static float                                            GuiButtonWidth;
    static bool                                             WindowCubismShow;

};

class GlobalConfig {
public:
    ::LAN           select_lan;
    int             user_num                       = 0;
    int             select_theme_id                = 0;         /*{ 0,1,2 }*/
    int             window_main_close              = 0;         /*{ 0,1 }*/
    int             window_main_style_id           = 0;         /*{ 0,1 }*/
    int             window_main_dock_id            = 0;         /*{ 0,1 }*/
    int             window_main_transparent_id     = 0;         /*{ 0,1 }*/
    float           window_main_forecastfps        = 120.0f;    /*10.0f-120.0f*/
    float           window_main_alpha              = 100.0f;    /*30.0f-100.0f*/
    int             window_main_addtimefps         = 0;         /* 0-100 */
    int             window_main_fast_id            = 0;         /*{ 0,1 }*/
    int             window_cubism_style_id         = 0;         /*{ 0,1,2,3 }*/
    int             window_cubism_addtimefps       = 0;         /*0-100*/
    std::string     select_font                    = "";
    int             fonts_size                     = 0;
    ImGuiStyle      defaultStyle;
public:
    const char** fonts_list = NULL;
    const char** fonts_name_list = NULL;
public:
    static GlobalConfig* getInstance() {
        static GlobalConfig instance;
        return &instance;
    }
    static void GlobalConfigInit(GlobalConfig* gc);
    static void GlobalConfigSave(GlobalConfig* gc = GlobalConfig::getInstance());

private:
    GlobalConfig() {
        //FileSetting 
        FileSetting::RefreshRead();
        //GlobalConfig
        GlobalConfigInit(this);
    }
    GlobalConfig(const GlobalConfig&);
    GlobalConfig& operator=(const GlobalConfig&);
    ~GlobalConfig();
};


//第一种实现 宏魔法
//#define LANGUAGE_ENUM_DEF(E,...) enum class LAN:int {E, ##__VA_ARGS__};
//LANGUAGE_ENUM_DEF(ZH, EN, JA)
//#define PAIR(key,value) {key, value}
//#define MAP_TYPE_DEF(KEY_TYPE,VALUE_TYPE) static std::map<KEY_TYPE, VALUE_TYPE>
//#define LANGUAGE_MAP(KEY_TYPE,VALUE_TYPE,...) \
//MAP_TYPE_DEF(KEY_TYPE,VALUE_TYPE)language_map = {__VA_ARGS__};
//static const char* language_list[] = { u8"简体中文", u8"English",u8"日本語" };
//LANGUAGE_MAP(int, LAN, PAIR(0, LAN::ZH), PAIR(1, LAN::EN),PAIR(2, LAN::JA))
//#define LANGUAGE_ADD(T,...) \
//static const char* ARR[] = {T,##__VA_ARGS__};\
//static const size_t ARR_SIZE = sizeof(ARR) / sizeof(ARR[0]);
//#define LANGUAGE_LAM(...) (([]()->const char* {\
//        LANGUAGE_ADD(__VA_ARGS__)\
//        return ((int)::GlobalConfig::getInstance()->select_lan >= ARR_SIZE) ? "???" : ARR[(int)::language_map[(int)::GlobalConfig::getInstance()->select_lan]];\
//})())
//#define AT(...) (LANGUAGE_LAM(__VA_ARGS__))

//第二种实现 obj.get
#include <array>
enum class LAN : int { ZH, EN, JA, COUNT };
static const char* language_list[] = { u8"简体中文", u8"English",u8"日本語" };
template <size_t N>
struct LangText {
    std::array<const char*, N> texts;
    const char* get(LAN lang) const {
        size_t index = static_cast<size_t>(lang);
        if (index < texts.size()) {
            return texts[index];
        }
        static const char* unknown = u8"???";
        return unknown;
    }
    const char* get() const {
        return get(::GlobalConfig::getInstance()->select_lan);
    }
    operator const char* () const {
        return get(::GlobalConfig::getInstance()->select_lan);
    }
};
#define AT(ZH, EN, JA) (static_cast<const char*>(LangText<static_cast<size_t>(LAN::COUNT)>{{ZH, EN, JA}}))

