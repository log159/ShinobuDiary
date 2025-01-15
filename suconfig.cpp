#include"suconfig.h"

namespace Su {
    UserConfig::UserConfig(int useid)
    {
        this->file_id = useid;
        this->select_llm = LLM::CHATGPT;
        this->select_tts = TTS::VITS_SIMPLE_API;
        this->select_stt = STT::BAIDUZHINENGYUN;
        this->select_mt = MT::BAIDUFANYI;
        this->vits_simple_api_model_id = 0;
        this->vits_simple_api_speaker_id = 0;
        this->vits_simple_api_emotion_id = 0;
        this->gpt_sovits_target_language_id = 0;
        this->myself_vits = INITSTR;
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
    }
    void UserConfig::operator=(const UserConfig& uc) {
        this->file_id = uc.file_id;
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
        for (auto& val : llms) {
            if (val.llm == llm) {
                return &val;
            }
        }
        return nullptr;
    }
    Su::TTSConfig* Su::UserConfig::getTTSConfig(Su::TTS tts) {
        for (auto& val : ttss) {
            if (val.tts == tts) {
                return &val;
            }
        }
        return nullptr;
    }
    Su::STTConfig* Su::UserConfig::getSTTConfig(Su::STT stt) {
        for (auto& val : stts) {
            if (val.stt == stt) {
                return &val;
            }
        }
        return nullptr;
    }
    Su::MTConfig* Su::UserConfig::getMTConfig(Su::MT mt) {
        for (auto& val : mts) {
            if (val.mt == mt) {
                return &val;
            }
        }
        return nullptr;
    }


    void S_MemsetStr(char* str, size_t size) {
        memset(str, 0, size);
    }

    void S_GetGuiMark(char* markbuf, size_t size, const char* name, const char* id) {
        S_MemsetStr(markbuf, size);
        strcpy_s(markbuf, size, name);
        strcat_s(markbuf, size, "##");
        strcat_s(markbuf, size, id);
    }

    void S_AllConfigInit()
    {
        std::cout << u8"Su配置初始化" << std::endl;
        UserConfig::getUserVector().clear();
        for (int i = 1; i <= ::GlobalConfig::getInstance()->user_num; ++i) {
            UserConfig::getUserVector().push_back(UserConfig(i));
        }
        for (auto& val : UserConfig::getUserVector())
            S_UserConfigInit(&val);
    }
    void S_AllConfigSave()
    {
        std::cout << u8"Su配置保存" << std::endl;
        GlobalConfig::GlobalConfigSave();
        for (auto& val : UserConfig::getUserVector())
            S_UserConfigSave(&val);
    }

    void S_UserConfigInit(UserConfig* uc)
    {
        std::cout << uc->file_id <<u8" 用户配置初始化" << std::endl;

        for (auto& val : uc->llms) S_InitKasb(uc, &val);
        for (auto& val : uc->ttss) S_InitAp(uc, &val);
        for (auto& val : uc->stts) S_InitKasb(uc, &val);
        for (auto& val : uc->mts) S_InitKasb(uc, &val);

        uc->select_llm = (LLM)FileSetting::S_GetLongValue(uc->file_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::LLMSEL_ID], INITINT);
        uc->select_tts = (TTS)FileSetting::S_GetLongValue(uc->file_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::TTSSEL_ID], INITINT);
        uc->select_stt = (STT)FileSetting::S_GetLongValue(uc->file_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::STTSEL_ID], INITINT);
        uc->select_mt = (MT)FileSetting::S_GetLongValue(uc->file_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::MTSEL_ID], INITINT);
        uc->vits_simple_api_model_id = FileSetting::S_GetLongValue(uc->file_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::VITS_SIMPLE_API_MODEL_ID], INITINT);
        uc->vits_simple_api_speaker_id = FileSetting::S_GetLongValue(uc->file_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::VITS_SIMPLE_API_SPEAKER_ID], INITINT);
        uc->vits_simple_api_emotion_id = FileSetting::S_GetLongValue(uc->file_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::VITS_SIMPLE_API_EMOTION_ID], INITINT);
        uc->gpt_sovits_target_language_id = FileSetting::S_GetLongValue(uc->file_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::GPT_SOVITS_TARGET_LANGUAGE_ID], INITINT);
        uc->myself_vits = FileSetting::S_GetValue(uc->file_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::MYSELF_VITS_URL], INITSTR);

    }
    void S_UserConfigSave(UserConfig* uc) {
        std::cout << uc->file_id <<u8" 用户配置保存" << std::endl;

        for (auto& val : uc->llms) S_SaveKasb(uc, &val);
        for (auto& val : uc->ttss) S_SaveAp(uc, &val);
        for (auto& val : uc->stts) S_SaveKasb(uc, &val);
        for (auto& val : uc->mts) S_SaveKasb(uc, &val);

        FileSetting::S_SetLongValue(uc->file_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::LLMSEL_ID], uc->select_llm);
        FileSetting::S_SetLongValue(uc->file_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::TTSSEL_ID], uc->select_tts);
        FileSetting::S_SetLongValue(uc->file_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::STTSEL_ID], uc->select_stt);
        FileSetting::S_SetLongValue(uc->file_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::MTSEL_ID], uc->select_mt);
        FileSetting::S_SetLongValue(uc->file_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::VITS_SIMPLE_API_MODEL_ID], uc->vits_simple_api_model_id);
        FileSetting::S_SetLongValue(uc->file_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::VITS_SIMPLE_API_SPEAKER_ID], uc->vits_simple_api_speaker_id);
        FileSetting::S_SetLongValue(uc->file_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::VITS_SIMPLE_API_EMOTION_ID], uc->vits_simple_api_emotion_id);
        FileSetting::S_SetLongValue(uc->file_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::GPT_SOVITS_TARGET_LANGUAGE_ID], uc->gpt_sovits_target_language_id);
        FileSetting::S_SetValue(uc->file_id, INIGROUPMARKSTR, inifreemark_map[FREEMARK::MYSELF_VITS_URL], uc->myself_vits.c_str());

    }

}