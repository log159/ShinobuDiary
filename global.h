#pragma once

#include <map>
#include "filesetting.h"
#include "imgui.h"
#define FONTSWAY "./Fonts/*.ttf"
#define FONTSTR "./Fonts/%s"
#define STYLEWAY "./Saves/style.data"
#define POS_0 uint32_t(0x1)
#define POS_1 uint32_t(0x2)
#define POS_2 uint32_t(0x4)
#define POS_3 uint32_t(0x8)

class FileSetting;

enum FREEMARK {
    LANGUAGESEL_ID,
    USER_NUM,
    THEMESEL_ID,
    FONTSEL_NAME,
    WINDOW_MAIN_STYLE_ID,
    WINDOW_CUBISM_STYLE_ID,
    LLMSEL_ID,
    TTSSEL_ID,
    STTSEL_ID,
    MTSEL_ID,
    VITS_SIMPLE_API_MODEL_ID,
    VITS_SIMPLE_API_SPEAKER_ID,
    VITS_SIMPLE_API_EMOTION_ID,
    GPT_SOVITS_TARGET_LANGUAGE_ID,
    MYSELF_VITS_URL
};
static std::map<FREEMARK, const char*>inifreemark_map = {
    {FREEMARK::LANGUAGESEL_ID,                      "languagesel_id"},
    {FREEMARK::USER_NUM,                            "user_num"},
    {FREEMARK::THEMESEL_ID,                         "themesel_id"},
    {FREEMARK::FONTSEL_NAME,                        "fontsel_id"},
    {FREEMARK::WINDOW_MAIN_STYLE_ID,                "window_main_style_id"},
    {FREEMARK::WINDOW_CUBISM_STYLE_ID,              "window_cubism_style_id"},
    {FREEMARK::LLMSEL_ID,                           "llmsel_id"},
    {FREEMARK::TTSSEL_ID,                           "ttssel_id"},
    {FREEMARK::STTSEL_ID,                           "sttsel_id"},
    {FREEMARK::MTSEL_ID,                            "mtsel_id"},
    {FREEMARK::VITS_SIMPLE_API_MODEL_ID,            "vits_simple_api_model_id"},
    {FREEMARK::VITS_SIMPLE_API_SPEAKER_ID,          "vits_simple_api_speaker_id"},
    {FREEMARK::VITS_SIMPLE_API_EMOTION_ID,          "vits_simple_api_emotion_id"},
    {FREEMARK::GPT_SOVITS_TARGET_LANGUAGE_ID,       "gpt_sovits_target_language_id"},
    {FREEMARK::MYSELF_VITS_URL,                     "myself_vits_url"}
};
enum INIMARK { KEY, APPID, SECRET, BASEURL, ADDRESS, PORT };
static std::map<INIMARK, const char*>inimark_map = {
    {INIMARK::KEY,          "key"       },
    {INIMARK::APPID,        "appid"     },
    {INIMARK::SECRET,       "secret"    },
    {INIMARK::BASEURL,      "baseurl"   },
    {INIMARK::ADDRESS,      "address"   },
    {INIMARK::PORT,         "port"      }
};

static const char* INIGROUPMARKSTR = "INIMARKGROUP%d";
static const char* vits_simple_api_model_list[] = { "vits","bert-vits2","w2v2-vits" };
static const char* gpt_sovits_target_language_list[] = { "zh","ja","en" };

#define LANGUAGE_ENUM_DEF(E,...) enum class LAN:int {E, ##__VA_ARGS__};
LANGUAGE_ENUM_DEF(CN, EN /*other*/)
class GlobalConfig {
public:
    ::LAN select_lan;
    int user_num                    = 0;
    int select_theme                = 0;/*{ 0,1,2 }*/
    int window_main_style           = 0;/*{ 0,1 }*/
    //float window_main_fea         = 0;/*{ 0,1 }*/
    int window_cubism_style_id      = 0;/*{ 0,1,2,3 }*/
    std::string select_font         = "";
    const char** fonts_list         = NULL;
    const char** fonts_name_list    = NULL;
    int fonts_size                  = 0;
    ImGuiStyle defaultStyle;
public:
    static GlobalConfig* getInstance() {
        static GlobalConfig instance;
        return &instance;
    }
    static void GlobalConfigInit(GlobalConfig* gc);
    static void GlobalConfigSave(GlobalConfig* gc = GlobalConfig::getInstance());

private:
    GlobalConfig() {
        GlobalConfigInit(this);
    }
    GlobalConfig(const GlobalConfig&);
    GlobalConfig& operator=(const GlobalConfig&);
    ~GlobalConfig();
};
#define PAIR(key,value) {key, value}
#define MAP_TYPE_DEF(KEY_TYPE,VALUE_TYPE) static std::map<KEY_TYPE, VALUE_TYPE>
#define LANGUAGE_MAP(KEY_TYPE,VALUE_TYPE,...) \
MAP_TYPE_DEF(KEY_TYPE,VALUE_TYPE)language_map = {__VA_ARGS__};
static const char* language_list[] = { u8"简体中文", u8"English"/*other*/ };
LANGUAGE_MAP(int, LAN, PAIR(0, LAN::CN), PAIR(1, LAN::EN)/*other*/)
#define LANGUAGE_ADD(T,...) \
static const char* ARR[] = {T,##__VA_ARGS__};
#define LANGUAGE_LAM(...) (([]()->const char* {\
        LANGUAGE_ADD(__VA_ARGS__)\
        return ARR[(int)::language_map[(int)::GlobalConfig::getInstance()->select_lan]];\
})())
#define AT(...) (LANGUAGE_LAM(__VA_ARGS__))

