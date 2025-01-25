#pragma once
#include <iostream>
#include <vector>
#include <map>

#include "filesetting.h"
#include "translator.h"
#include "imgui.h"
#include "sufunction.h"

using std::cout;
using std::endl;
enum class LAN :int;
class GlobalConfig;

namespace Su{
    struct Kasb;
    struct Ap;
    class ConfigBase;
    class LLMConfig;
    class TTSConfig;
    class STTConfig;
    class MTConfig;
    class UserConfig;
    enum LLM { CHATGPT, SPARKDESK, DEEPSEEK };
    enum TTS { VITS_SIMPLE_API, GPT_SOVITS, MYSELF_VITS };
    enum STT { BAIDUZHINENGYUN };
    enum MT { BAIDUFANYI };
    class UserConfig {
    public:
        bool exist  = true;/*存在标识*/
        std::string                  user_name;
        int                          file_id;
        Su::LLM                      select_llm;
        Su::TTS                      select_tts;
        Su::STT                      select_stt;
        Su::MT                       select_mt;
        int                          vits_simple_api_model_id;
        int                          vits_simple_api_speaker_id;
        int                          vits_simple_api_emotion_id;
        int                          gpt_sovits_target_language_id;
        std::string                  myself_vits;
        std::vector<LLMConfig>       llms;
        std::vector<TTSConfig>       ttss;
        std::vector<STTConfig>       stts;
        std::vector<MTConfig>        mts;
    public:
        explicit UserConfig(int useid);
        void operator=(const UserConfig& uc);
        UserConfig(const UserConfig& uc);

        static std::vector<UserConfig>& getUserVector() {
            static std::vector<UserConfig>v;
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
        inline explicit ConfigBase(const char* n) {
            strcpy_s(name, n);
        }
        virtual const char* getLanName() = 0;
    };


    struct Kasb {
        uint32_t needpos = 1;               /* key appid secret baseurl -> 2^0 / 2^1 / 2^2 / 2^3 */
        char key[DEFSIZE] = INITSTR;        //1
        char appid[DEFSIZE] = INITSTR;      //2
        char secret[DEFSIZE] = INITSTR;     //4
        char baseurl[DEFSIZE] = INITSTR;    //8
    };
    struct Ap {
        char address[DEFSIZE] = INITSTR;
        char port[DEFSIZE] = INITSTR;
    };

    class LLMConfig :public ConfigBase {
    public:
        LLM llm;
        Kasb kasb;
        inline explicit LLMConfig(LLM l, const char* n, const uint32_t& np) :
            llm(l), ConfigBase(n) {
            kasb.needpos = np;
        }
        const char* getLanName();
    };
    class TTSConfig:public ConfigBase {
    public:
        TTS tts;
        Ap ap;
        inline explicit TTSConfig(TTS t,const char* n,const char* a, const char* p):
            tts(t),ConfigBase(n){
            strcpy_s(ap.address,a);
            strcpy_s(ap.port, p);
        }
        const char* getLanName();
    };
    class STTConfig:public ConfigBase{
    public:
        STT stt;
        Kasb kasb;
        inline explicit STTConfig(STT s, const char* n, const uint32_t& np) :
            stt(s),ConfigBase(n){
            kasb.needpos = np;
        }
        const char* getLanName();
    };
    class MTConfig :public ConfigBase {

    public:
        MT mt;
        Kasb kasb;
        inline explicit MTConfig(MT m, const char* n, const uint32_t& np):
            mt(m),ConfigBase(n){
            kasb.needpos = np;
        }
        const char* getLanName();
    };

    void AllConfigInit();
    void UserConfigInit(UserConfig* uc);

    void AllConfigSave();
    void UserConfigSave(UserConfig* uc);

    //配置信息保存到文件
    template<typename FT, typename T>
    void SaveKasb(FT* uc, T* tc) {
        static char key[DEFSIZE2];
        static char appid[DEFSIZE2];
        static char secret[DEFSIZE2];
        static char baseurl[DEFSIZE2];
        GetGuiMark(key, sizeof(key), inimark_map[INIMARK::KEY], tc->name);
        GetGuiMark(appid, sizeof(appid), inimark_map[INIMARK::APPID], tc->name);
        GetGuiMark(secret, sizeof(secret), inimark_map[INIMARK::SECRET], tc->name);
        GetGuiMark(baseurl, sizeof(baseurl), inimark_map[INIMARK::BASEURL], tc->name);
        if (POS_0 & tc->kasb.needpos)FileSetting::SetValue(uc->file_id, INIGROUPMARKSTR, key, tc->kasb.key);
        if (POS_1 & tc->kasb.needpos)FileSetting::SetValue(uc->file_id, INIGROUPMARKSTR, appid, tc->kasb.appid);
        if (POS_2 & tc->kasb.needpos)FileSetting::SetValue(uc->file_id, INIGROUPMARKSTR, secret, tc->kasb.secret);
        if (POS_3 & tc->kasb.needpos)FileSetting::SetValue(uc->file_id, INIGROUPMARKSTR, baseurl, tc->kasb.baseurl);
    }
    template<typename FT, typename T>
    void SaveAp(FT* uc, T* tc) {
        static char address[DEFSIZE2]; 
        static char port[DEFSIZE2];
        GetGuiMark(address, sizeof(address), inimark_map[INIMARK::ADDRESS], tc->name);
        GetGuiMark(port, sizeof(port), inimark_map[INIMARK::PORT], tc->name);
        FileSetting::SetValue(uc->file_id, INIGROUPMARKSTR, address,tc->ap.address);
        FileSetting::SetValue(uc->file_id, INIGROUPMARKSTR, port, tc->ap.port);
    }

    template<typename FT,typename T>
    void InitKasb(FT* uc, T* tc) {
        static char key[DEFSIZE2];
        static char appid[DEFSIZE2];
        static char secret[DEFSIZE2];
        static char baseurl[DEFSIZE2];
        GetGuiMark(key, sizeof(key), inimark_map[INIMARK::KEY], tc->name);
        GetGuiMark(appid, sizeof(appid), inimark_map[INIMARK::APPID], tc->name);
        GetGuiMark(secret, sizeof(secret), inimark_map[INIMARK::SECRET], tc->name);
        GetGuiMark(baseurl, sizeof(baseurl), inimark_map[INIMARK::BASEURL], tc->name);
        if (POS_0 & tc->kasb.needpos)strcpy_s(tc->kasb.key,sizeof(tc->kasb.key), FileSetting::GetValue(uc->file_id, INIGROUPMARKSTR, key, INITSTR).c_str());
        if (POS_1 & tc->kasb.needpos)strcpy_s(tc->kasb.appid, sizeof(tc->kasb.appid),FileSetting::GetValue(uc->file_id, INIGROUPMARKSTR, appid, INITSTR).c_str());
        if (POS_2 & tc->kasb.needpos)strcpy_s(tc->kasb.secret, sizeof(tc->kasb.secret), FileSetting::GetValue(uc->file_id, INIGROUPMARKSTR, secret, INITSTR).c_str());
        if (POS_3 & tc->kasb.needpos)strcpy_s(tc->kasb.baseurl, sizeof(tc->kasb.baseurl), FileSetting::GetValue(uc->file_id, INIGROUPMARKSTR, baseurl, INITSTR).c_str());
    }
    template<typename FT, typename T>
    void InitAp(FT* uc, T* tc) {
        static char address[DEFSIZE2];
        static char port[DEFSIZE2];
        GetGuiMark(address, sizeof(address), inimark_map[INIMARK::ADDRESS], tc->name);
        GetGuiMark(port, sizeof(port), inimark_map[INIMARK::PORT], tc->name);
        strcpy_s(tc->ap.address,sizeof(tc->ap.address), FileSetting::GetValue(uc->file_id, INIGROUPMARKSTR, address, INITSTR).c_str());
        strcpy_s(tc->ap.port, sizeof(tc->ap.port),FileSetting::GetValue(uc->file_id, INIGROUPMARKSTR, port, INITSTR).c_str());
    }

}