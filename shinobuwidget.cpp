#include "shinobuwidget.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "compoundgui.h"
#include "translator.h"
#include "llmhome.h"
#include "./cubism_src/CubismLoom.h"
#include <string>
#include "timeconfig.h"

static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}
void ShowShinobuHead() {
    ImGui::ProgressBar(-0.5f * (float)ImGui::GetTime(), ImVec2(ImGui::GetContentRegionAvail().x, 7.0f), NULL);

}
void ShowShinobuStart()
{
    return;
    if (ImGui::CollapsingHeader(u8"从这里开始"))
    {
        if (Su::UserConfig::getUserVector().size() <= 0) {
            ImGui::Text(u8"至少创建一个用户配置实例！");

        }
        const char* column_names[] = { u8"用户","2","3"};
        const char* rows_names[]= {  u8"启用设定角色", u8"启用语音输出", u8"启用百度翻译", u8"启用翻译与原文对比", u8"启用语音输入" };
        const int columns_count = IM_ARRAYSIZE(column_names);
        const int rows_count = IM_ARRAYSIZE(rows_names);

        static ImGuiTableFlags table_flags =   ImGuiTableFlags_ScrollX  | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_Hideable | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_HighlightHoveredColumn;
        static ImGuiTableColumnFlags column_flags = ImGuiTableColumnFlags_AngledHeader | ImGuiTableColumnFlags_WidthFixed;
        static bool bools[columns_count * rows_count] = {}; // Dummy storage selection storage
        static int frozen_cols = 1;
        static int frozen_rows = 2;

        if (ImGui::BeginTable("table_angled_headers", columns_count, table_flags, ImVec2(0.0f, 0.0f)))
        {
            ImGui::TableSetupColumn(column_names[0], ImGuiTableColumnFlags_NoHide | ImGuiTableColumnFlags_NoReorder);
            for (int n = 1; n < columns_count; n++)
                ImGui::TableSetupColumn(column_names[n], column_flags);
            ImGui::TableSetupScrollFreeze(frozen_cols, frozen_rows);

            ImGui::TableAngledHeadersRow(); // Draw angled headers for all columns with the ImGuiTableColumnFlags_AngledHeader flag.
            ImGui::TableHeadersRow();       // Draw remaining headers and allow access to context-menu and other functions.
            for (int row = 0; row < rows_count; row++)
            {
                ImGui::PushID(row);
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::AlignTextToFramePadding();
                ImGui::Text(rows_names[row]);
                for (int column = 1; column < columns_count; column++)
                    if (ImGui::TableSetColumnIndex(column))
                    {
                        ImGui::PushID(column);
                        ImGui::Checkbox("", &bools[row * columns_count + column]);
                        ImGui::PopID();
                    }
                ImGui::PopID();
            }
            ImGui::EndTable();
        }
    }

}
void ShowShinobuLanguage() {

    auto fl = [&](const char* text) {
        if (ImGui::BeginCombo(text, language_list[(int)GlobalConfig::getInstance()->select_lan])) {
            for (int n = 0; n < IM_ARRAYSIZE(language_list); n++) {
                const bool is_selected = ((int)GlobalConfig::getInstance()->select_lan == n);
                if (ImGui::Selectable(language_list[n], is_selected)) {
                    GlobalConfig::getInstance()->select_lan = (LAN)n;
                    GlobalConfig::GlobalConfigSave();
                }
                if (is_selected)ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    };
    auto f = [fl](const char* headertitle) {
        if (ImGui::CollapsingHeader(headertitle))fl(TT_4);
    };
    f(TT_5);
}

void ShowShinobuStyle() {
    static bool ShowStyleEditor = false;
    if (ShowStyleEditor) {
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
        ImGui::Begin(TT_39,&ShowStyleEditor);
        ImGui::ShowStyleEditor();
        ImGui::End();
    }
    if (ImGui::CollapsingHeader(TT_40))
    {
        ImGui::Checkbox(TT_41, &ShowStyleEditor);
        ImGui::SameLine();
        ImGui::Spacing();
    }
}

void ShowShinobuGlobal()
{
    if (ImGui::CollapsingHeader(TT_224))
    {
        if (ImGui::TreeNode(TT_231)) {
            ImGui::SeparatorText(TT_232);
            static int ws_t = 0, ws_d = 0, ws_dt = 0;
            ImGui::RadioButton(TT_234, &ws_t, 0); ImGui::SameLine();
            ImGui::RadioButton(TT_235, &ws_t, 1);
            ImGui::SeparatorText(TT_239);
            ImGui::RadioButton(TT_240, &ws_d, 0); ImGui::SameLine();
            ImGui::RadioButton(TT_241, &ws_d, 1);
            ImGui::RadioButton(TT_242, &ws_dt, 0); ImGui::SameLine();
            ImGui::RadioButton(TT_243, &ws_dt, 1);

            ImGui::SeparatorText(TT_233);
            static float fps = 0.0f, fps_min_stop = 0.0f;
            ImGui::SliderFloat(TT_236, &fps, 10.0f, 120.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
            ImGui::SliderFloat(TT_237, &fps_min_stop, 1.0f, 200.0f, "%.2f", ImGuiSliderFlags_Logarithmic);


            if (ImGui::Button(TT_8)){

            }
            ImGuiIO& io = ImGui::GetIO();
            ImGui::Text(TT_42, 1000.0f / io.Framerate, io.Framerate);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode(TT_225)) {
            ImGui::SeparatorText(TT_230);
            static int e = ::GlobalConfig::getInstance()->window_cubism_style_id;
            ImGui::RadioButton(TT_226, &e, 0); ImGui::SameLine();
            ImGui::RadioButton(TT_227, &e, 1); ImGui::SameLine();
            ImGui::RadioButton(TT_228, &e, 2); ImGui::SameLine();
            ImGui::RadioButton(TT_229, &e, 3);
            if (GlobalConfig::getInstance()->window_cubism_style_id != e) {
                CubismLoom::addMessageList(::window_group_mark, ::window_style_mark, std::to_string(e).c_str());
                ::GlobalConfig::GlobalConfigSave();
            }
            ImGui::TreePop();
        }


    }

}

void ShowShinobuUser()
{
    if (ImGui::CollapsingHeader(TT_32))
    {
        if (ImGui::BeginTabBar("##SettingBar001", ImGuiTabBarFlags_None))
        {
            if (ImGui::TabItemButton("+", ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoTooltip))
            {
                int pos = Su::UserConfig::getUserVector().size();
                Su::UserConfig::getUserVector().push_back(Su::UserConfig(pos + 1));
                ::GlobalConfig::getInstance()->user_num = Su::UserConfig::getUserVector().size();
                Su::S_AllConfigSave();
            }
            static char tabname[DEFSIZE];
            int erase_pos = -1;
            for (int i = 0; i < (int)Su::UserConfig::getUserVector().size(); ++i) {
                strcpy_s(tabname, sizeof(tabname), TT_33);
                snprintf(tabname, sizeof(tabname), tabname, Su::UserConfig::getUserVector()[i].file_id);
                if (ImGui::BeginTabItem(tabname, &Su::UserConfig::getUserVector()[i].exist, ImGuiTabItemFlags_None))
                {
                    Su::UserConfig* _uc = &Su::UserConfig::getUserVector()[i];
                    ShowShinobuLLM(_uc);
                    ShowShinibuTTS(_uc);
                    ShowShinobuSTT(_uc);
                    ShowShinobuMT(_uc);
                    ImGui::EndTabItem();
                }
                if (Su::UserConfig::getUserVector()[i].exist == false) {
                    ImGui::OpenPopup(TT_35);
                    ImVec2 parent_pos = ImGui::GetWindowPos();
                    ImVec2 parent_size = ImGui::GetWindowSize();
                    ImVec2 popup_center = ImVec2(
                        parent_pos.x + parent_size.x * 0.5f,
                        parent_pos.y + parent_size.y * 0.5f
                    );
                    ImGui::SetNextWindowPos(popup_center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
                    if (ImGui::BeginPopupModal(TT_35, NULL, ImGuiWindowFlags_AlwaysAutoResize))
                    {
                        ImGui::Text(TT_34);
                        ImGui::Separator();
                        if (ImGui::Button(TT_36, ImVec2(120, 0))) {
                            erase_pos = i;
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::SetItemDefaultFocus();
                        ImGui::SameLine();
                        if (ImGui::Button(TT_37, ImVec2(120, 0))) {
                            Su::UserConfig::getUserVector()[i].exist = true;
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::EndPopup();
                    }
                }
            }
            if (erase_pos != -1) {
                Su::UserConfig::getUserVector().erase(Su::UserConfig::getUserVector().begin() + erase_pos);
                for (int i = 0; i < (int)Su::UserConfig::getUserVector().size(); ++i) {
                    Su::UserConfig::getUserVector()[i].exist = true;
                    Su::UserConfig::getUserVector()[i].file_id = i + 1;
                }
                erase_pos = -1;
                ::GlobalConfig::getInstance()->user_num = Su::UserConfig::getUserVector().size();
                Su::S_AllConfigSave();
            }
            ImGui::EndTabBar();
        }
    }
}

void ShowShinobuLLM(Su::UserConfig* _uc) {
    auto fb = [_uc](const char* butname, Su::LLMConfig* llmc) {
        if (ImGui::Button(butname))
        {
            std::cout << butname << std::endl;
            S_SaveKasb(_uc, llmc);
        }
    };
    auto ft = [fb](const char* nodename, Su::LLMConfig* llmc, const char* butname) {
        static char button[DEFSIZE];
        Su::S_GetGuiMark(button, sizeof(button), butname, llmc->name);
        KASB_FUNCTION(llmc)
        fb(button, llmc);
    };
    auto fc = [_uc](const char* comtitle) {
        CM_FUNCTION(comtitle, _uc->llms, _uc->select_llm, Su::LLM)
    };
    auto f = [_uc, ft, fb, fc](const char* headertitle, const char* butname) {
        if (ImGui::TreeNode(headertitle)) {
            fc(TT_6);
            for (auto& val : _uc->llms) {
                if(val.llm==_uc->select_llm)
                    ft(val.getLanName(), &val, butname);
            }
            ImGui::TreePop();
        }
    };
    f(TT_7, TT_8);
}

void ShowShinibuTTS(Su::UserConfig* _uc) {
    auto fb = [_uc](const char* butname, Su::TTSConfig* ttsc) {
        if (ImGui::Button(butname))
        {
            std::cout << butname << std::endl;
            S_UserConfigSave(_uc);

        }
    };
    auto ft = [_uc,fb](const char* nodename, Su::TTSConfig* ttsc, const char* butname) {

        static char button[DEFSIZE];
        static char input[DEFSIZE];
        static char input_value[DEFSIZE];
        if (ttsc->tts == Su::TTS::VITS_SIMPLE_API) {
            AP_FUNCTION(ttsc);
            Su::S_MemsetStr(input,sizeof(input));
            strcpy_s(input, sizeof(input),TT_27);
            {
                if (ImGui::BeginCombo(input, ::vits_simple_api_model_list[_uc->vits_simple_api_model_id])) {
                    for (int n = 0; n < IM_ARRAYSIZE(::vits_simple_api_model_list); n++) {
                        const bool is_selected = ((int)_uc->vits_simple_api_model_id == n);
                        if (ImGui::Selectable(::vits_simple_api_model_list[n], is_selected)) {
                            _uc->vits_simple_api_model_id = n;
                            S_UserConfigSave(_uc);
                        }
                        if (is_selected)ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
            }
            strcpy_s(input, sizeof(input), TT_26);
            sprintf_s(input_value, "%d", _uc->vits_simple_api_speaker_id);
            ImGui::InputText(input, input_value, IM_ARRAYSIZE(input_value));
            if (strlen(input_value) >= 1)
                _uc->vits_simple_api_speaker_id = std::atoi(input_value);
            strcpy_s(input, sizeof(input), TT_28);
            sprintf_s(input_value, "%d", _uc->vits_simple_api_emotion_id);
            ImGui::InputText(input, input_value, IM_ARRAYSIZE(input_value));
            if (strlen(input_value) >= 1)
                _uc->vits_simple_api_emotion_id = std::atoi(input_value);

        }
        else if(ttsc->tts== Su::TTS::GPT_SOVITS) {
            
            AP_FUNCTION(ttsc);
            Su::S_MemsetStr(input, sizeof(input));
            strcpy_s(input, sizeof(input), TT_27);
            {
                if (ImGui::BeginCombo(input, ::gpt_sovits_target_language_list[_uc->gpt_sovits_target_language_id])) {
                    for (int n = 0; n < IM_ARRAYSIZE(::gpt_sovits_target_language_list); n++) {
                        const bool is_selected = ((int)_uc->gpt_sovits_target_language_id == n);
                        if (ImGui::Selectable(::gpt_sovits_target_language_list[n], is_selected)) {
                            _uc->gpt_sovits_target_language_id = n;
                            S_UserConfigSave(_uc);
                        }
                        if (is_selected)ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
            }
        }
        else if(ttsc->tts== Su::TTS::MYSELF_VITS){
            static char url[DEFSIZE];
            static char input_url[DEFSIZE16]; 
            strcpy_s(input_url, sizeof(input_url), _uc->myself_vits.c_str());
            Su::S_GetGuiMark(url, sizeof(url), TT_31, ttsc->name);
            ImGui::InputText(url, input_url, IM_ARRAYSIZE(input_url));
            _uc->myself_vits.assign(input_url);

        }

        Su::S_GetGuiMark(button, sizeof(button), butname, ttsc->name);
        fb(button, ttsc);

    };
    auto fc = [_uc](const char* comtitle) {
        CM_FUNCTION(comtitle, _uc->ttss, _uc->select_tts, Su::TTS)
    };
    auto f = [_uc, ft, fb, fc](const char* headertitle, const char* butname) {
        if (ImGui::TreeNode(headertitle)) {
            fc(TT_9);
            for (auto& val : _uc->ttss) {
                if(val.tts==_uc->select_tts)
                    ft(val.getLanName(), &val, butname);
            }
            ImGui::TreePop();
        }
    };
    f(TT_10, TT_8);
}

void ShowShinobuSTT(Su::UserConfig* _uc) {
    auto fb = [_uc](const char* butname, Su::STTConfig* sttc) {
        if (ImGui::Button(butname))
        {
            std::cout << butname << std::endl;
            S_SaveKasb(_uc, sttc);
        }
    };
    auto ft = [fb](const char* nodename, Su::STTConfig* sttc, const char* butname) {

        static char button[DEFSIZE];
        Su::S_GetGuiMark(button, sizeof(button), butname, sttc->name);
        KASB_FUNCTION(sttc)
        fb(button, sttc);
    };
    auto fc = [_uc](const char* comtitle) {
        CM_FUNCTION(comtitle, _uc->stts, _uc->select_stt, Su::STT)
    };
    auto f = [_uc, ft, fb, fc](const char* headertitle, const char* butname) {
        if (ImGui::TreeNode(headertitle)) {
            fc(TT_11);
            for (auto& val : _uc->stts)
            {
                if(val.stt==_uc->select_stt)
                    ft(val.getLanName(), &val, butname);
            }
            ImGui::TreePop();

        }
    };
    f(TT_12, TT_8);

}
void ShowShinobuMT(Su::UserConfig* _uc) {
    auto fb = [_uc](const char* butname, Su::MTConfig* mtc) {
        if (ImGui::Button(butname))
        {
            std::cout << butname << std::endl;
            S_SaveKasb(_uc, mtc);
        }
    };
    auto ft = [fb](const char* nodename, Su::MTConfig* mtc, const char* butname) {

            static char button[DEFSIZE];
            Su::S_GetGuiMark(button, sizeof(button), butname, mtc->name);
            KASB_FUNCTION(mtc)
            fb(button, mtc);
    };
    auto fc = [_uc](const char* comtitle) {
        CM_FUNCTION(comtitle, _uc->mts, _uc->select_mt, Su::MT)
    };
    auto f = [_uc, ft, fb, fc](const char* headertitle, const char* butname) {
        if (ImGui::TreeNode(headertitle)) {
            fc(TT_13);
            for (auto& val : _uc->mts) {
                if (val.mt == _uc->select_mt)
                    ft(val.getLanName(), &val, butname);
            }
            ImGui::TreePop();
        }
    };
    f(TT_14, TT_8);
}
void ShowShinobuHelp() {
    auto f = [](const char* headertitle, const char* title, const char* text) {
        if (ImGui::CollapsingHeader(headertitle))
        {
            ImGui::SeparatorText(title);
            ImGui::BulletText(text);
        }
    };
    f(TT_15, TT_16, TT_17);
}
void ShowShinobuAbout() {

    auto f = [](const char* headertitle, const char* title, const char* text, const char* url) {
        if (ImGui::CollapsingHeader(headertitle))
        {
            ImGui::SeparatorText(title);
            ImGui::BulletText(text);
            ImGui::TextLinkOpenURL(url);
            float samples[100];
            for (int n = 0; n < 100; n++)
                samples[n] = sinf((float)n * 0.2f + (float)ImGui::GetTime() * 1.5f);
            ImGui::PlotLines("##S_SamplesPlotLines001", samples, 100, NULL, NULL, -1.0f, 1.0f, ImVec2(ImGui::GetContentRegionAvail().x, 0.0));
            ImGui::ProgressBar(sinf((float)ImGui::GetTime()) * 0.5f + 0.5f, ImVec2(ImGui::GetContentRegionAvail().x, 0.0));

        }
    };
    f(TT_0, TT_1, TT_2, TT_3);

}

void ShowShinobuWindow(bool* p_open) {
    if (*p_open == false) { return; }
    char buf[128];
    sprintf_s(buf, sizeof(buf), TT_38, "|/-\\"[(int)(ImGui::GetTime() / 0.25f) & 3], ::getCurrentTime().c_str());
    ImGui::SetNextWindowPos(ImVec2(0,0), ImGuiCond_FirstUseEver);
    ImGui::Begin(buf, p_open);

    ShowShinobuHead();
    ShowShinobuStart();
    //{
    //    static char text[DEFSIZEK16] =
    //        "/*\n"
    //        " The Pentium F00F bug, shorthand for F0 0F C7 C8,\n"
    //        " the hexadecimal encoding of one offending instruction,\n"
    //        " more formally, the invalid operand with locked CMPXCHG8B\n"
    //        " instruction bug, is a design flaw in the majority of\n"
    //        " Intel Pentium, Pentium MMX, and Pentium OverDrive\n"
    //        " processors (all in the P5 microarchitecture).\n"
    //        "*/\n\n"
    //        "label:\n"
    //        "\tlock cmpxchg8b eax\n";
    //    static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
    //    static char text2[DEFSIZEK16];
    //    static ImGuiInputTextFlags flags2 = ImGuiInputTextFlags_AllowTabInput;
    //    //HelpMarker("You can use the ImGuiInputTextFlags_CallbackResize facility if you need to wire InputTextMultiline() to a dynamic string type. See misc/cpp/imgui_stdlib.h for an example. (This is not demonstrated in imgui_demo.cpp because we don't want to include <string> in here)");
    //    //ImGui::CheckboxFlags("ImGuiInputTextFlags_ReadOnly", &flags, ImGuiInputTextFlags_ReadOnly);
    //    //ImGui::CheckboxFlags("ImGuiInputTextFlags_AllowTabInput", &flags, ImGuiInputTextFlags_AllowTabInput);
    //    //ImGui::SameLine(); HelpMarker("When _AllowTabInput is set, passing through the widget with Tabbing doesn't automatically activate it, in order to also cycling through subsequent widgets.");
    //    //ImGui::CheckboxFlags("ImGuiInputTextFlags_CtrlEnterForNewLine", &flags, ImGuiInputTextFlags_CtrlEnterForNewLine);
    //    ImGui::InputTextMultiline("##source", text, IM_ARRAYSIZE(text), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 5), flags);
    //    ImGui::InputTextMultiline("##source2", text2, IM_ARRAYSIZE(text2), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 5), flags2);
    //    if (ImGui::Button("Send Button")) {
    //        Su::S_SparkDesk(&Su::UserConfig::getUserVector()[0].getLLMConfig(Su::LLM::SPARKDESK)->kasb, text, sizeof(text), text2);
    //    }

    //    //static char user[DEFSIZEK16];
    //    //ImGui::InputText("user input", user, IM_ARRAYSIZE(user));

    //}


    ShowShinobuLanguage();
    ShowShinobuStyle();
    ShowShinobuGlobal();
    ShowShinobuUser();
    ShowShinobuHelp();
    ShowShinobuAbout();

    ImGui::End();
}
