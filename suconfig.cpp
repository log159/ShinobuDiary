#include"suconfig.h"
#include "translator.h"
#include "filesetting.h"

namespace Su {
    UserConfig::UserConfig(int useid) :
        //user normal
        user_id(useid),
        select_llm(LLM::CHATGPT),
        select_tts(TTS::VITS_SIMPLE_API),
        select_stt(STT::BAIDUZHINENGYUN),
        select_mt(MT::BAIDUFANYI),
        vits_simple_api_model_id(0),
        vits_simple_api_speaker_id(0),
        vits_simple_api_emotion_id(0),
        gpt_sovits_target_language_id(0),
        myself_vits(INITSTR),
        //Table
        enable_widget(false),
        enable_cubism(false),
        enable_template(false),
        enable_tts(false),
        enable_mt(false),
        enable_original(false),
        enable_stt(false)
    {
        this->llms = std::vector<LLMConfig>({
            LLMConfig(LLM::CHATGPT,u8"ChatGPT",9),
            LLMConfig(LLM::SPARKDESK,u8"SparkDesk",7),
            LLMConfig(LLM::DEEPSEEK,u8"DeepSeek",1)
            });
        this->ttss = std::vector<TTSConfig>({
            TTSConfig(TTS::VITS_SIMPLE_API,u8"vits-simple-api",u8"127.0.0.1",u8"23456"),
            TTSConfig(TTS::GPT_SOVITS,u8"GPT-SoVits",u8"127.0.0.1",u8"9880"),
            TTSConfig(TTS::MYSELF_VITS,u8"MySelf-Vits",INITSTR,INITSTR)
            });
        this->stts = std::vector<STTConfig>({
            STTConfig(STT::BAIDUZHINENGYUN,u8"BaiDuZhiNengYun",5)
            });
        this->mts = std::vector<MTConfig>({
            MTConfig(MT::BAIDUFANYI,u8"BaiDufanYi",3)
            });
        //Cubism
        cubism_config.model_dir = "";
        cubism_config.cubism_cg.cubism_ts_s = 0.0;
        cubism_config.cubism_cg.cubism_tx_t = 0.0;
        cubism_config.cubism_cg.cubism_ty_t = 0.0;
        cubism_config.cubism_cg.cubism_ts_x = 0.0;
        cubism_config.cubism_cg.cubism_ts_y = 0.0;
        cubism_config.cubism_cg.cubism_tx_s = 0.0;
        cubism_config.cubism_cg.cubism_ty_s = 0.0;
        cubism_config.cubism_cg.cubism_tx_p = 0.0;
        cubism_config.cubism_cg.cubism_ty_p = 0.0;
    }
    void UserConfig::operator=(const UserConfig& uc) {
        this->user_id = uc.user_id;
        this->select_llm = uc.select_llm;
        this->select_tts = uc.select_tts;
        this->select_stt = uc.select_stt;
        this->select_mt = uc.select_mt;
        this->vits_simple_api_model_id = uc.vits_simple_api_model_id;
        this->vits_simple_api_speaker_id = uc.vits_simple_api_speaker_id;
        this->vits_simple_api_emotion_id = uc.vits_simple_api_emotion_id;
        this->gpt_sovits_target_language_id = uc.gpt_sovits_target_language_id;
        this->myself_vits = uc.myself_vits;
        this->llms = uc.llms;
        this->ttss = uc.ttss;
        this->stts = uc.stts;
        this->mts = uc.mts;
        this->enable_widget = uc.enable_widget;         
        this->enable_cubism = uc.enable_cubism;         
        this->enable_template = uc.enable_template;     
        this->enable_tts = uc.enable_tts;               
        this->enable_mt = uc.enable_mt;                 
        this->enable_original = uc.enable_original;     
        this->enable_stt = uc.enable_stt;               
        //Cubism
        this->cubism_config.model_dir = uc.cubism_config.model_dir;
        this->cubism_config.cubism_cg = uc.cubism_config.cubism_cg;

    }
    UserConfig::UserConfig(const UserConfig& uc) {
        this->operator=(uc);
    }

    const char* LLMConfig::getLanName() {
        if (llm == LLM::CHATGPT)return TT_18;
        else if (llm == LLM::SPARKDESK)return TT_19;
        else if (llm == LLM::DEEPSEEK)return TT_20;
        else return INITSTR;
    }
    const char* TTSConfig::getLanName() {
        if (tts == TTS::VITS_SIMPLE_API)return TT_21;
        else if (tts == TTS::GPT_SOVITS)return TT_22;
        else if (tts == TTS::MYSELF_VITS)return TT_23;
        else return INITSTR;
    }
    const char* STTConfig::getLanName() {
        if (stt == STT::BAIDUZHINENGYUN)return TT_24;
        else return INITSTR;
    }

    const char* MTConfig::getLanName() {
        if (mt == MT::BAIDUFANYI)return TT_25;
        else return INITSTR;
    }

    Su::LLMConfig* Su::UserConfig::getLLMConfig(Su::LLM llm) {
        for (auto& val : llms) if (val.llm == llm)return &val;
        return nullptr;
    }
    Su::TTSConfig* Su::UserConfig::getTTSConfig(Su::TTS tts) {
        for (auto& val : ttss) if (val.tts == tts) return &val;
        return nullptr;
    }
    Su::STTConfig* Su::UserConfig::getSTTConfig(Su::STT stt) {
        for (auto& val : stts) if (val.stt == stt) return &val;
        return nullptr;
    }
    Su::MTConfig* Su::UserConfig::getMTConfig(Su::MT mt) {
        for (auto& val : mts) if (val.mt == mt)return &val;
        return nullptr;
    }

    void AllConfigInit()
    {
        std::cout << u8"Su配置全部初始化" << std::endl;
        UserConfig::getUserVector().clear();
        for (int i = 1; i <= ::GlobalConfig::getInstance()->user_num; ++i) 
            UserConfig::getUserVector().push_back(UserConfig(i));
        for (auto& val : UserConfig::getUserVector())
            UserConfigInit(&val);
    }
    void AllConfigSave()
    {
        FileSetting::ClearFile();
        std::cout << u8"Su配置全部保存" << std::endl;
        GlobalConfig::GlobalConfigSave();
        for (auto& val : UserConfig::getUserVector())
            UserConfigSave(&val);
        GlobalTemp::RefreshTable = true;

    }

    void UserConfigInit(UserConfig* uc)
    {
        std::cout << uc->user_id <<u8" 用户配置初始化" << std::endl;

        //for (auto& val : uc->llms) InitKasb(uc, &val);
        //for (auto& val : uc->ttss) InitAp(uc, &val);
        //for (auto& val : uc->stts) InitKasb(uc, &val);
        //for (auto& val : uc->mts) InitKasb(uc, &val);

        //uc->select_llm = (LLM)FileSetting::GetLongValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::LLMSEL_ID], INITINT);
        //uc->select_tts = (TTS)FileSetting::GetLongValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::TTSSEL_ID], INITINT);
        //uc->select_stt = (STT)FileSetting::GetLongValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::STTSEL_ID], INITINT);
        //uc->select_mt = (MT)FileSetting::GetLongValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::MTSEL_ID], INITINT);
        //uc->vits_simple_api_model_id = FileSetting::GetLongValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::VITS_SIMPLE_API_MODEL_ID], INITINT);
        //uc->vits_simple_api_speaker_id = FileSetting::GetLongValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::VITS_SIMPLE_API_SPEAKER_ID], INITINT);
        //uc->vits_simple_api_emotion_id = FileSetting::GetLongValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::VITS_SIMPLE_API_EMOTION_ID], INITINT);
        //uc->gpt_sovits_target_language_id = FileSetting::GetLongValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::GPT_SOVITS_TARGET_LANGUAGE_ID], INITINT);
        //uc->myself_vits = FileSetting::GetValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::MYSELF_VITS_URL], INITSTR);

        uc->enable_widget = FileSetting::GetBoolValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::ENABLE_WIDGET], INITBOOL);
        uc->enable_cubism = FileSetting::GetBoolValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::ENABLE_CUBISM], INITBOOL);
        //uc->enable_template = FileSetting::GetBoolValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::ENABLE_TEMPLATE], INITBOOL);
        //uc->enable_tts = FileSetting::GetBoolValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::ENABLE_TTS], INITBOOL);
        //uc->enable_mt = FileSetting::GetBoolValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::ENABLE_MT], INITBOOL);
        //uc->enable_original = FileSetting::GetBoolValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::ENABLE_ORIGINAL], INITBOOL);
        //uc->enable_stt = FileSetting::GetBoolValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::ENABLE_STT], INITBOOL);

        //Cubism
        uc->cubism_config.model_dir = FileSetting::GetValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::CUBISM_CONFIG_MODEL_DIR], INITSTR);
        uc->cubism_config.cubism_cg.cubism_ts_s = (float)FileSetting::GetDoubleValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::CUBISM_CG_CUBISM_TS_S], INITDOUBLE);
        uc->cubism_config.cubism_cg.cubism_tx_t = (float)FileSetting::GetDoubleValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::CUBISM_CG_CUBISM_TX_T], INITDOUBLE);
        uc->cubism_config.cubism_cg.cubism_ty_t = (float)FileSetting::GetDoubleValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::CUBISM_CG_CUBISM_TY_T], INITDOUBLE);
        uc->cubism_config.cubism_cg.cubism_ts_x = (float)FileSetting::GetDoubleValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::CUBISM_CG_CUBISM_TS_X], INITDOUBLE);
        uc->cubism_config.cubism_cg.cubism_ts_y = (float)FileSetting::GetDoubleValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::CUBISM_CG_CUBISM_TS_Y], INITDOUBLE);
        uc->cubism_config.cubism_cg.cubism_tx_s = (float)FileSetting::GetDoubleValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::CUBISM_CG_CUBISM_TX_S], INITDOUBLE);
        uc->cubism_config.cubism_cg.cubism_ty_s = (float)FileSetting::GetDoubleValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::CUBISM_CG_CUBISM_TY_S], INITDOUBLE);
        uc->cubism_config.cubism_cg.cubism_tx_p = (float)FileSetting::GetDoubleValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::CUBISM_CG_CUBISM_TX_P], INITDOUBLE);
        uc->cubism_config.cubism_cg.cubism_ty_p = (float)FileSetting::GetDoubleValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::CUBISM_CG_CUBISM_TY_P], INITDOUBLE);

    }
    void UserConfigSave(UserConfig* uc) {
        std::cout << uc->user_id <<u8" 用户配置保存" << std::endl;
        CSimpleIniA ini;
        FileSetting::BeginSave(ini);
        //for (auto& val : uc->llms) SaveKasb(uc, &val);
        //for (auto& val : uc->ttss) SaveAp(uc, &val);
        //for (auto& val : uc->stts) SaveKasb(uc, &val);
        //for (auto& val : uc->mts) SaveKasb(uc, &val);

        //FileSetting::SetLongValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::LLMSEL_ID], uc->select_llm);
        //FileSetting::SetLongValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::TTSSEL_ID], uc->select_tts);
        //FileSetting::SetLongValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::STTSEL_ID], uc->select_stt);
        //FileSetting::SetLongValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::MTSEL_ID], uc->select_mt);
        //FileSetting::SetLongValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::VITS_SIMPLE_API_MODEL_ID], uc->vits_simple_api_model_id);
        //FileSetting::SetLongValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::VITS_SIMPLE_API_SPEAKER_ID], uc->vits_simple_api_speaker_id);
        //FileSetting::SetLongValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::VITS_SIMPLE_API_EMOTION_ID], uc->vits_simple_api_emotion_id);
        //FileSetting::SetLongValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::GPT_SOVITS_TARGET_LANGUAGE_ID], uc->gpt_sovits_target_language_id);
        //FileSetting::SetValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::MYSELF_VITS_URL], uc->myself_vits.c_str());

        FileSetting::SetBoolValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::ENABLE_WIDGET], uc->enable_widget);
        FileSetting::SetBoolValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::ENABLE_CUBISM], uc->enable_cubism );
        //FileSetting::SetBoolValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::ENABLE_TEMPLATE], uc->enable_template);
        //FileSetting::SetBoolValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::ENABLE_TTS], uc->enable_tts);
        //FileSetting::SetBoolValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::ENABLE_MT], uc->enable_mt);
        //FileSetting::SetBoolValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::ENABLE_ORIGINAL], uc->enable_original);
        //FileSetting::SetBoolValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::ENABLE_STT], uc->enable_stt);
        
        //Cubism
        FileSetting::SetValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::CUBISM_CONFIG_MODEL_DIR], uc->cubism_config.model_dir.c_str());
        FileSetting::SetDoubleValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::CUBISM_CG_CUBISM_TS_S], uc->cubism_config.cubism_cg.cubism_ts_s);
        FileSetting::SetDoubleValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::CUBISM_CG_CUBISM_TX_T], uc->cubism_config.cubism_cg.cubism_tx_t);
        FileSetting::SetDoubleValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::CUBISM_CG_CUBISM_TY_T], uc->cubism_config.cubism_cg.cubism_ty_t);
        FileSetting::SetDoubleValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::CUBISM_CG_CUBISM_TS_X], uc->cubism_config.cubism_cg.cubism_ts_x);
        FileSetting::SetDoubleValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::CUBISM_CG_CUBISM_TS_Y], uc->cubism_config.cubism_cg.cubism_ts_y);
        FileSetting::SetDoubleValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::CUBISM_CG_CUBISM_TX_S], uc->cubism_config.cubism_cg.cubism_tx_s);
        FileSetting::SetDoubleValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::CUBISM_CG_CUBISM_TY_S], uc->cubism_config.cubism_cg.cubism_ty_s);
        FileSetting::SetDoubleValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::CUBISM_CG_CUBISM_TX_P], uc->cubism_config.cubism_cg.cubism_tx_p);
        FileSetting::SetDoubleValue(uc->user_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::CUBISM_CG_CUBISM_TY_P], uc->cubism_config.cubism_cg.cubism_ty_p);

        FileSetting::EndSave(ini);
    }

    template<typename FT, typename T>
    void SaveKasb(FT* uc, T* tc) {
        static char key[DEFSIZE2];
        static char appid[DEFSIZE2];
        static char secret[DEFSIZE2];
        static char baseurl[DEFSIZE2];
        JoinGuiMark(key, sizeof(key), inimark_map[INIMARK::KEY], tc->name);
        JoinGuiMark(appid, sizeof(appid), inimark_map[INIMARK::APPID], tc->name);
        JoinGuiMark(secret, sizeof(secret), inimark_map[INIMARK::SECRET], tc->name);
        JoinGuiMark(baseurl, sizeof(baseurl), inimark_map[INIMARK::BASEURL], tc->name);
        if (POS_0 & tc->kasb.needpos)FileSetting::SetValue(uc->user_id, INIGROUPMARKSTR, key, tc->kasb.key);
        if (POS_1 & tc->kasb.needpos)FileSetting::SetValue(uc->user_id, INIGROUPMARKSTR, appid, tc->kasb.appid);
        if (POS_2 & tc->kasb.needpos)FileSetting::SetValue(uc->user_id, INIGROUPMARKSTR, secret, tc->kasb.secret);
        if (POS_3 & tc->kasb.needpos)FileSetting::SetValue(uc->user_id, INIGROUPMARKSTR, baseurl, tc->kasb.baseurl);
    }
    template<typename FT, typename T>
    void SaveAp(FT* uc, T* tc) {
        static char address[DEFSIZE2];
        static char port[DEFSIZE2];
        JoinGuiMark(address, sizeof(address), inimark_map[INIMARK::ADDRESS], tc->name);
        JoinGuiMark(port, sizeof(port), inimark_map[INIMARK::PORT], tc->name);
        FileSetting::SetValue(uc->user_id, INIGROUPMARKSTR, address, tc->ap.address);
        FileSetting::SetValue(uc->user_id, INIGROUPMARKSTR, port, tc->ap.port);
    }

    template<typename FT, typename T>
    void InitKasb(FT* uc, T* tc) {
        static char key[DEFSIZE2];
        static char appid[DEFSIZE2];
        static char secret[DEFSIZE2];
        static char baseurl[DEFSIZE2];
        JoinGuiMark(key, sizeof(key), inimark_map[INIMARK::KEY], tc->name);
        JoinGuiMark(appid, sizeof(appid), inimark_map[INIMARK::APPID], tc->name);
        JoinGuiMark(secret, sizeof(secret), inimark_map[INIMARK::SECRET], tc->name);
        JoinGuiMark(baseurl, sizeof(baseurl), inimark_map[INIMARK::BASEURL], tc->name);
        if (POS_0 & tc->kasb.needpos)strcpy_s(tc->kasb.key, sizeof(tc->kasb.key), FileSetting::GetValue(uc->user_id, INIGROUPMARKSTR, key, INITSTR).c_str());
        if (POS_1 & tc->kasb.needpos)strcpy_s(tc->kasb.appid, sizeof(tc->kasb.appid), FileSetting::GetValue(uc->user_id, INIGROUPMARKSTR, appid, INITSTR).c_str());
        if (POS_2 & tc->kasb.needpos)strcpy_s(tc->kasb.secret, sizeof(tc->kasb.secret), FileSetting::GetValue(uc->user_id, INIGROUPMARKSTR, secret, INITSTR).c_str());
        if (POS_3 & tc->kasb.needpos)strcpy_s(tc->kasb.baseurl, sizeof(tc->kasb.baseurl), FileSetting::GetValue(uc->user_id, INIGROUPMARKSTR, baseurl, INITSTR).c_str());
    }
    template<typename FT, typename T>
    void InitAp(FT* uc, T* tc) {
        static char address[DEFSIZE2];
        static char port[DEFSIZE2];
        JoinGuiMark(address, sizeof(address), inimark_map[INIMARK::ADDRESS], tc->name);
        JoinGuiMark(port, sizeof(port), inimark_map[INIMARK::PORT], tc->name);
        strcpy_s(tc->ap.address, sizeof(tc->ap.address), FileSetting::GetValue(uc->user_id, INIGROUPMARKSTR, address, INITSTR).c_str());
        strcpy_s(tc->ap.port, sizeof(tc->ap.port), FileSetting::GetValue(uc->user_id, INIGROUPMARKSTR, port, INITSTR).c_str());
    }

}