#pragma once
#include <vector>
#include "./global.h"
#include "./sizedef.h"
#include "../cubism_src/LAppModel.hpp"
enum class LAN :int;
class GlobalConfig;

namespace Su{
    struct  Kasb; struct  Ap;
    class   ConfigBase;
    class   LLMConfig; class   TTSConfig; class   STTConfig; class   MTConfig;
    class   UserConfig; class   CubismConfig;
    struct  Su::ShinobuExList;
    enum LLM { CHATGPT, SPARKDESK, DEEPSEEK };
    enum TTS { VITS_SIMPLE_API, GPT_SOVITS, MYSELF_VITS };
    enum STT { BAIDUZHINENGYUN };
    enum MT { BAIDUFANYI };
    extern std::vector<LLMConfig>       llms;
    extern std::vector<TTSConfig>       ttss;
    extern std::vector<STTConfig>       stts;
    extern std::vector<MTConfig>        mts;

    class UserConfig {
    public:
        //temp 直接初始化参数以区分
        bool                         need_init_cubism = false;
        //normal 构造中初始化
        int                          user_id;           //用户标识
        bool                         enable_widget;     //交互面板  [0]
        bool                         enable_cubism;     //虚拟形象  [1]
        bool                         enable_template;   //设定角色  [2]
        bool                         enable_tts;        //TTS       [3]
        bool                         enable_mt;         //MT        [4]
        bool                         enable_original;   //原文本对比[5]
        bool                         enable_stt;        //STT       [6]
        Su::LLM                      select_llm;
        Su::TTS                      select_tts;
        Su::STT                      select_stt;
        Su::MT                       select_mt;
        std::string                  myself_vits;
        int                          vits_simple_api_model_id;
        int                          vits_simple_api_speaker_id;
        int                          vits_simple_api_emotion_id;
        int                          gpt_sovits_target_language_id;
        std::string                  cubism_model_dir;

    public:
        explicit UserConfig(int useid);
        UserConfig(const UserConfig& uc);
        void operator=(const UserConfig& uc);
        static std::vector<Su::UserConfig>& getUserVector() {
            static std::vector<Su::UserConfig> v;
            return v;
        }
        Su::LLMConfig* getLLMConfig(Su::LLM llm);
        Su::TTSConfig* getTTSConfig(Su::TTS tts);
        Su::STTConfig* getSTTConfig(Su::STT stt);
        Su::MTConfig* getMTConfig(Su::MT mt);
    };

    class ConfigBase {
    public:
        char name[DEFSIZE] = INITSTR;
        inline explicit ConfigBase(const char* n) { strcpy_s(name, n); }
        virtual const char* getLanName() = 0;
    };
    struct Kasb {
        /* key appid secret baseurl -> 2^0 / 2^1 / 2^2 / 2^3 */
        uint32_t needpos        = 1;
        char key[DEFSIZE]       = INITSTR;  //1
        char appid[DEFSIZE]     = INITSTR;  //2
        char secret[DEFSIZE]    = INITSTR;  //4
        char baseurl[DEFSIZE]   = INITSTR;  //8
    };
    struct Ap {
        char address[DEFSIZE]   = INITSTR;
        char port[DEFSIZE]      = INITSTR;
    };

    class LLMConfig :public ConfigBase {
    public:
        LLM llm;
        Kasb kasb;
        inline explicit LLMConfig(LLM l, const char* n, const uint32_t& np) :llm(l), ConfigBase(n) {
            kasb.needpos = np;
        }
        const char* getLanName();
    };
    class TTSConfig:public ConfigBase {
    public:
        TTS tts;
        Ap ap;
        inline explicit TTSConfig(TTS t,const char* n,const char* a, const char* p):tts(t),ConfigBase(n){
            strcpy_s(ap.address,a);
            strcpy_s(ap.port, p);
        }
        const char* getLanName();
    };
    class STTConfig:public ConfigBase{
    public:
        STT stt;
        Kasb kasb;
        inline explicit STTConfig(STT s, const char* n, const uint32_t& np) :stt(s),ConfigBase(n){
            kasb.needpos = np;
        }
        const char* getLanName();
    };
    class MTConfig :public ConfigBase {

    public:
        MT mt;
        Kasb kasb;
        inline explicit MTConfig(MT m, const char* n, const uint32_t& np):mt(m),ConfigBase(n){
            kasb.needpos = np;
        }
        const char* getLanName();
    };

    void AllConfigInit();
    void UserConfigInit(UserConfig* uc);
    void AllConfigSave();
    void UserConfigSave(UserConfig* uc);

}