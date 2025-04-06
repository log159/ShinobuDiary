#include"suconfig.h"
#include "./translator.h"
#include "./filesetting.h"
#include "./somemacros.h"
#include "sufunction.h"

namespace Su {

    std::vector<LLMConfig>       Su::llms = 
    std::vector<LLMConfig>({
        LLMConfig(LLM::CHATGPT,u8"ChatGPT",9),
        LLMConfig(LLM::SPARKDESK,u8"SparkDesk",7),
        LLMConfig(LLM::DEEPSEEK,u8"DeepSeek",1)
    });
    std::vector<TTSConfig>       Su::ttss =
    std::vector<TTSConfig>({
        TTSConfig(TTS::VITS_SIMPLE_API,u8"vits-simple-api",u8"127.0.0.1",u8"23456"),
        TTSConfig(TTS::GPT_SOVITS,u8"GPT-SoVits",u8"127.0.0.1",u8"9880"),
        TTSConfig(TTS::MYSELF_VITS,u8"MySelf-Vits",INITSTR,INITSTR)
    });
    std::vector<STTConfig>       Su::stts =
    std::vector<STTConfig>({
        STTConfig(STT::BAIDUZHINENGYUN,u8"BaiDuZhiNengYun",5)
    });
    std::vector<MTConfig>        Su::mts =
    std::vector<MTConfig>({
        MTConfig(MT::BAIDUFANYI,u8"BaiDufanYi",3)
    });

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
        enable_stt(false),
        cubism_model_dir("")
    {

    }
    UserConfig::UserConfig(const UserConfig& uc) {
        this->operator=(uc);
    }
    void UserConfig::operator=(const UserConfig& uc)
    {
        user_id = uc.user_id;
        select_llm = uc.select_llm;
        select_tts = uc.select_tts;
        select_stt = uc.select_stt;
        select_mt = uc.select_mt;
        vits_simple_api_model_id = uc.vits_simple_api_model_id;
        vits_simple_api_speaker_id = uc.vits_simple_api_speaker_id;
        vits_simple_api_emotion_id = uc.vits_simple_api_emotion_id;
        gpt_sovits_target_language_id = uc.gpt_sovits_target_language_id;
        myself_vits = uc.myself_vits;
        enable_widget = uc.enable_widget;
        enable_cubism = uc.enable_cubism;
        enable_template = uc.enable_template;
        enable_tts = uc.enable_tts;
        enable_mt = uc.enable_mt;
        enable_original = uc.enable_original;
        enable_stt = uc.enable_stt;
        cubism_model_dir = uc.cubism_model_dir;
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
        for (auto& val : Su::llms) if (val.llm == llm)return &val;
        return nullptr;
    }
    Su::TTSConfig* Su::UserConfig::getTTSConfig(Su::TTS tts) {
        for (auto& val : Su::ttss) if (val.tts == tts) return &val;
        return nullptr;
    }
    Su::STTConfig* Su::UserConfig::getSTTConfig(Su::STT stt) {
        for (auto& val : Su::stts) if (val.stt == stt) return &val;
        return nullptr;
    }
    Su::MTConfig* Su::UserConfig::getMTConfig(Su::MT mt) {
        for (auto& val : Su::mts) if (val.mt == mt)return &val;
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

        //uc->select_llm = (LLM)FileSetting::GetLongValue(uc->user_id, INIGROUPMARKSTR, ::inimark_map[::INIMARK::LLMSEL_ID], INITINT);
        //uc->select_tts = (TTS)FileSetting::GetLongValue(uc->user_id, INIGROUPMARKSTR, ::inimark_map[::INIMARK::TTSSEL_ID], INITINT);
        //uc->select_stt = (STT)FileSetting::GetLongValue(uc->user_id, INIGROUPMARKSTR, ::inimark_map[::INIMARK::STTSEL_ID], INITINT);
        //uc->select_mt = (MT)FileSetting::GetLongValue(uc->user_id, INIGROUPMARKSTR, ::inimark_map[::INIMARK::MTSEL_ID], INITINT);
        //uc->vits_simple_api_model_id = FileSetting::GetLongValue(uc->user_id, INIGROUPMARKSTR, ::inimark_map[::INIMARK::VITS_SIMPLE_API_MODEL_ID], INITINT);
        //uc->vits_simple_api_speaker_id = FileSetting::GetLongValue(uc->user_id, INIGROUPMARKSTR, ::inimark_map[::INIMARK::VITS_SIMPLE_API_SPEAKER_ID], INITINT);
        //uc->vits_simple_api_emotion_id = FileSetting::GetLongValue(uc->user_id, INIGROUPMARKSTR, ::inimark_map[::INIMARK::VITS_SIMPLE_API_EMOTION_ID], INITINT);
        //uc->gpt_sovits_target_language_id = FileSetting::GetLongValue(uc->user_id, INIGROUPMARKSTR, ::inimark_map[::INIMARK::GPT_SOVITS_TARGET_LANGUAGE_ID], INITINT);
        //uc->myself_vits = FileSetting::GetValue(uc->user_id, INIGROUPMARKSTR, ::inimark_map[::INIMARK::MYSELF_VITS_URL], INITSTR);

        uc->enable_widget = FileSetting::GetBoolValue(uc->user_id, INIGROUPMARKSTR, ::inimark_map[::INIMARK::ENABLE_WIDGET], INITBOOL);
        uc->enable_cubism = FileSetting::GetBoolValue(uc->user_id, INIGROUPMARKSTR, ::inimark_map[::INIMARK::ENABLE_CUBISM], INITBOOL);
        //uc->enable_template = FileSetting::GetBoolValue(uc->user_id, INIGROUPMARKSTR, ::inimark_map[::INIMARK::ENABLE_TEMPLATE], INITBOOL);
        //uc->enable_tts = FileSetting::GetBoolValue(uc->user_id, INIGROUPMARKSTR, ::inimark_map[::INIMARK::ENABLE_TTS], INITBOOL);
        //uc->enable_mt = FileSetting::GetBoolValue(uc->user_id, INIGROUPMARKSTR, ::inimark_map[::INIMARK::ENABLE_MT], INITBOOL);
        //uc->enable_original = FileSetting::GetBoolValue(uc->user_id, INIGROUPMARKSTR, ::inimark_map[::INIMARK::ENABLE_ORIGINAL], INITBOOL);
        //uc->enable_stt = FileSetting::GetBoolValue(uc->user_id, INIGROUPMARKSTR, ::inimark_map[::INIMARK::ENABLE_STT], INITBOOL);
        uc->cubism_model_dir = FileSetting::GetValue(uc->user_id, INIGROUPMARKSTR, ::inimark_map[::INIMARK::CUBISM_MODEL_DIR], INITSTR);
    }
    void UserConfigSave(UserConfig* uc) {
        std::cout << uc->user_id <<u8" 用户配置保存" << std::endl;
        CSimpleIniA ini;
        FileSetting::BeginSave(ini);
        //for (auto& val : uc->llms) SaveKasb(uc, &val);
        //for (auto& val : uc->ttss) SaveAp(uc, &val);
        //for (auto& val : uc->stts) SaveKasb(uc, &val);
        //for (auto& val : uc->mts) SaveKasb(uc, &val);

        //FileSetting::SetLongValue(uc->user_id, INIGROUPMARKSTR, ::inimark_map[::INIMARK::LLMSEL_ID], uc->select_llm);
        //FileSetting::SetLongValue(uc->user_id, INIGROUPMARKSTR, ::inimark_map[::INIMARK::TTSSEL_ID], uc->select_tts);
        //FileSetting::SetLongValue(uc->user_id, INIGROUPMARKSTR, ::inimark_map[::INIMARK::STTSEL_ID], uc->select_stt);
        //FileSetting::SetLongValue(uc->user_id, INIGROUPMARKSTR, ::inimark_map[::INIMARK::MTSEL_ID], uc->select_mt);
        //FileSetting::SetLongValue(uc->user_id, INIGROUPMARKSTR, ::inimark_map[::INIMARK::VITS_SIMPLE_API_MODEL_ID], uc->vits_simple_api_model_id);
        //FileSetting::SetLongValue(uc->user_id, INIGROUPMARKSTR, ::inimark_map[::INIMARK::VITS_SIMPLE_API_SPEAKER_ID], uc->vits_simple_api_speaker_id);
        //FileSetting::SetLongValue(uc->user_id, INIGROUPMARKSTR, ::inimark_map[::INIMARK::VITS_SIMPLE_API_EMOTION_ID], uc->vits_simple_api_emotion_id);
        //FileSetting::SetLongValue(uc->user_id, INIGROUPMARKSTR, ::inimark_map[::INIMARK::GPT_SOVITS_TARGET_LANGUAGE_ID], uc->gpt_sovits_target_language_id);
        //FileSetting::SetValue(uc->user_id, INIGROUPMARKSTR, ::inimark_map[::INIMARK::MYSELF_VITS_URL], uc->myself_vits.c_str());

        FileSetting::SetBoolValue(uc->user_id, INIGROUPMARKSTR, ::inimark_map[::INIMARK::ENABLE_WIDGET], uc->enable_widget);
        FileSetting::SetBoolValue(uc->user_id, INIGROUPMARKSTR, ::inimark_map[::INIMARK::ENABLE_CUBISM], uc->enable_cubism );
        //FileSetting::SetBoolValue(uc->user_id, INIGROUPMARKSTR, ::inimark_map[::INIMARK::ENABLE_TEMPLATE], uc->enable_template);
        //FileSetting::SetBoolValue(uc->user_id, INIGROUPMARKSTR, ::inimark_map[::INIMARK::ENABLE_TTS], uc->enable_tts);
        //FileSetting::SetBoolValue(uc->user_id, INIGROUPMARKSTR, ::inimark_map[::INIMARK::ENABLE_MT], uc->enable_mt);
        //FileSetting::SetBoolValue(uc->user_id, INIGROUPMARKSTR, ::inimark_map[::INIMARK::ENABLE_ORIGINAL], uc->enable_original);
        //FileSetting::SetBoolValue(uc->user_id, INIGROUPMARKSTR, ::inimark_map[::INIMARK::ENABLE_STT], uc->enable_stt);
        FileSetting::SetValue(uc->user_id, INIGROUPMARKSTR, ::inimark_map[::INIMARK::CUBISM_MODEL_DIR], uc->cubism_model_dir.c_str());
        FileSetting::EndSave(ini);
    }
}