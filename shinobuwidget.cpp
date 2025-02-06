#include "shinobuwidget.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include <string>
#include <windows.h>
#include <iostream>
#include "compoundgui.h"
#include "translator.h"
#include "llmhome.h"
#include "./cubism_src/CubismLoom.h"
#include "timeconfig.h"
#include "./cubism_src/LAppLive2DManager.hpp"

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
void ShowShinobuLunar()
{
    if (ImGui::CollapsingHeader(TT_254)) {
        char buf[DEFSIZE];
        sprintf_s(buf, sizeof(buf), "%s", ::getCurrentTime().c_str());
        ImGui::Text(buf);
        if (GlobalConfig::getInstance()->select_lan == LAN::CN) {
            ImGui::Text(GlobalTemp::LunarCalendar.c_str());
        }
        else {
            ImGui::Text(TT_255);
        }
    }
}


void ShowShinobuStart()
{
    if (ImGui::CollapsingHeader(TT_256))
    {
        if (Su::UserConfig::getUserVector().size() <= 0) {
            ImGui::Text(TT_257);
            return;
        }
        static std::vector<std::string>column_names_v;
        static int column_names_size = (int)Su::UserConfig::getUserVector().size() + 1;
        static const char* column_names[DEFSIZE] = {  };
        const char* rows_names[] = { TT_258,TT_259 ,TT_260, TT_261, TT_262,TT_263,TT_264 };
        static ImGuiTableFlags table_flags = ImGuiTableFlags_ScrollX | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_Hideable | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_HighlightHoveredColumn;
        static ImGuiTableColumnFlags column_flags = ImGuiTableColumnFlags_AngledHeader | ImGuiTableColumnFlags_WidthFixed;
        static bool bools[DEFSIZE][DEFSIZEK16] = { false };
        static int frozen_cols = 1;
        static int frozen_rows = 2;

        int columns_count = column_names_size;
        const int rows_count = IM_ARRAYSIZE(rows_names);
        float row_height = ImGui::GetTextLineHeightWithSpacing();
        float table_height = row_height * rows_count + 10.0f;

        table_height = max(table_height, 500.f);
        table_height = min(table_height, 1000.f);

        if (GlobalTemp::RefreshTable == true) {
            memset(bools, false, sizeof(bools));
            column_names_size = (int)Su::UserConfig::getUserVector().size() + 1;
            std::cout << u8"变化用户表格实例" << std::endl;
            GlobalTemp::RefreshTable = false;
            column_names_v.clear();
            column_names_v.reserve(column_names_size);
            static char tabname[DEFSIZE];
            memset(tabname, 0, sizeof(tabname));
            column_names[0] = TT_265;
            for (int i = 0; i < column_names_size; ++i) {
                strcpy_s(tabname, sizeof(tabname), TT_33);
                snprintf(tabname, sizeof(tabname), tabname, Su::UserConfig::getUserVector()[i].user_id);
                column_names_v.push_back(std::string(tabname));
                column_names[i + 1] = column_names_v.back().c_str();
            }
            for (int col = 1; col < columns_count; ++col) {
                for (int row = 0; row < rows_count; ++row) {
                    bool& bo = bools[row][col];
                    Su::UserConfig& uc = Su::UserConfig::getUserVector()[col - 1];
                    switch (row) {
                    case 0:bo = uc.enable_widget; break;
                    case 1:bo = uc.enable_cubism; break;
                    case 2:bo = uc.enable_template; break;
                    case 3:bo = uc.enable_tts; break;
                    case 4:bo = uc.enable_mt; break;
                    case 5:bo = uc.enable_original; break;
                    case 6:bo = uc.enable_stt; break;
                    }
                }
            }
        }
        for (int col = 1; col < columns_count; ++col) {
            for (int row = 0; row < rows_count; ++row) {
                const bool& bo = bools[row][col];
                Su::UserConfig& uc = Su::UserConfig::getUserVector()[col - 1];
                switch (row) {
                case 0:uc.enable_widget = bo; break;
                case 1:uc.enable_cubism = bo; break;
                case 2:uc.enable_template = bo; break;
                case 3:uc.enable_tts = bo; break;
                case 4:uc.enable_mt = bo; break;
                case 5:uc.enable_original = bo; break;
                case 6:uc.enable_stt = bo; break;
                }
            }
        }
        if (ImGui::BeginTable("###table_angled_headers___ShowShinobuStart", columns_count, table_flags, ImVec2(0.0f, table_height)))
        {
            ImGui::TableSetupColumn(column_names[0], ImGuiTableColumnFlags_NoHide | ImGuiTableColumnFlags_NoReorder);
            for (int n = 1; n < columns_count; n++)
                ImGui::TableSetupColumn(column_names[n], column_flags);
            ImGui::TableSetupScrollFreeze(frozen_cols, frozen_rows);

            ImGui::TableAngledHeadersRow();
            ImGui::TableHeadersRow();
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
                        ImGui::Checkbox("###Checkbox___ShowShinobuStart", &bools[row][column]);
                        ImGui::PopID();
                    }
                ImGui::PopID();
            }
            ImGui::EndTable();
        }

        if (ImGui::Button(TT_117, ImVec2(150,0))) {
            GlobalTemp::RefreshCubism = true;
            Su::AllConfigSave();
        }
        ImGui::SameLine();
        if (ImGui::Button(TT_266, ImVec2(150, 0))) {
            GlobalTemp::RefreshCubism = true;
            for (int col = 1; col < columns_count; ++col) {
                for (int row = 0; row < rows_count; ++row) {
                    bool& bo = bools[row][col];
                    bo = false;
                    Su::UserConfig& uc = Su::UserConfig::getUserVector()[col - 1];
                    switch (row) {
                        case 0:uc.enable_widget = bo; break;
                        case 1:uc.enable_cubism = bo; break;
                        case 2:uc.enable_template = bo; break;
                        case 3:uc.enable_tts = bo; break;
                        case 4:uc.enable_mt = bo; break;
                        case 5:uc.enable_original = bo; break;
                        case 6:uc.enable_stt = bo; break;
                    }
                }
            }
        }

    }
}

void ShowShinobuCubism()
{
    if (GlobalTemp::CubismQuit == true)return;
    if (LAppLive2DManager::GetInstance()->GetModelDir().GetSize() <=0)return;

    if (!GlobalTemp::RefreshCubism)return;
    GlobalTemp::RefreshCubism = false;
    auto& cm=LAppLive2DManager::GetInstance()->UserCubismMap;
    cm.clear();
    for (auto& val : Su::UserConfig::getUserVector())
    {
        if (val.enable_cubism) {
            if (val.cubism_config.model_dir.size() <= 0) {
                val.cubism_config.model_dir = LAppLive2DManager::GetInstance()->GetModelDir()[0].GetRawString();
            }
            int pos = cm.size();
            cm[val.user_id] = std::pair<int, std::string>(pos, val.cubism_config.model_dir);
        }
    }
    std::cout <<u8"Cubism Size：" << cm.size() << u8" ----------------------->" << std::endl;
    for (auto it = cm.begin(); it != cm.end();++it) {
        std::cout << u8"用户ID：" << it->first << " " << it->second.first << " " << it->second.second.c_str() << std::endl;
    }
    GlobalTemp::RefreshCubismScene = true;
    HWND hwnd = FindWindow(NULL, GlobalTemp::CubismWindowClassName);
    if (!hwnd) {
        MessageBox(NULL, u8"Cubism窗口未找到", u8"错误", MB_OK);
    }
    if (hwnd != NULL) {
        POINT pt;
        if (GetCursorPos(&pt)) {
            std::cout << u8"鼠标当前位置: (" << pt.x << ", " << pt.y << ")\n";
        }

        LPARAM lParam = MAKELPARAM(pt.x, pt.y); // 传递客户区坐标
        WPARAM wParam = MK_LBUTTON;

        // 发送鼠标按下事件
        PostMessage(hwnd, WM_LBUTTONDOWN, wParam, lParam);
        // 发送鼠标松开事件
        PostMessage(hwnd, WM_LBUTTONUP, 0, lParam);

    }



}
void ShowShinobuCSM(Su::UserConfig* _uc)
{
    ImGui::SeparatorText(TT_267);

    Csm::csmVector<Csm::csmString>& csmModelDir = LAppLive2DManager::GetInstance()->GetModelDir();
    if (csmModelDir.GetSize() <= 0) {
        if (ImGui::BeginCombo(TT_268, TT_269)) {
            ImGui::EndCombo();
        };
        ImGui::TreePop();
        return;
    }
    static const char* modelDirs[DEFSIZEK16] = { 0 };
    static int modelDirsSize = -1;
    int select = ([_uc,csmModelDir]()->int {
        int pos = 0;
        for (int i = 0; i < csmModelDir.GetSize(); ++i) {
            if (strcmp(_uc->cubism_config.model_dir.c_str(),csmModelDir[i].GetRawString()) == 0) {
                pos = i;
                break;
            }
        }
        return pos;
    }());
    if (modelDirsSize != csmModelDir.GetSize()) {
        modelDirsSize = csmModelDir.GetSize();
        for (int i = 0; i < modelDirsSize; ++i) {
            modelDirs[i] = csmModelDir[i].GetRawString();
        }
    }
    if (ImGui::BeginCombo(TT_268, modelDirs[select])) {
        for (int n = 0; n < modelDirsSize; n++) {
            const bool is_selected = (select == n);
            if (ImGui::Selectable(modelDirs[n], is_selected)) {
                select = n;
                _uc->cubism_config.model_dir = std::string(modelDirs[n] == NULL ? "": modelDirs[n]);
                cout << _uc->user_id << " " << _uc->cubism_config.model_dir.c_str() << endl;
                Su::UserConfigSave(_uc);
            }
            if (is_selected)ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
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


void ShowShinobuGlobal()
{

    if (ImGui::CollapsingHeader(TT_224))
    {
        float halfWidth = ImGui::GetWindowSize().x / 2.0f;         // 计算一半宽度

        if (ImGui::TreeNode(TT_231)) {

            ImGuiIO& io = ImGui::GetIO(); (void)io;

            ImGui::SeparatorText(TT_40);
            ImGui::Checkbox(TT_41, &GlobalTemp::ShowStyleEditor);

            ImGui::SeparatorText(TT_232);
            static int ws_t = ::GlobalConfig::getInstance()->window_main_style_id,
                ws_d = ::GlobalConfig::getInstance()->window_main_dock_id,
                ws_dt = ::GlobalConfig::getInstance()->window_main_transparent_id,
                ws_f = ::GlobalConfig::getInstance()->window_main_fast_id;
            ImGui::RadioButton(TT_234, &ws_t, 0); ImGui::SameLine(); ImGui::SetCursorPosX(halfWidth);
            ImGui::RadioButton(TT_235, &ws_t, 1);

            ImGui::SeparatorText(TT_239);
            ImGui::RadioButton(TT_240, &ws_d, 0); ImGui::SameLine(); ImGui::SetCursorPosX(halfWidth);
            ImGui::RadioButton(TT_241, &ws_d, 1);
            ImGui::RadioButton(TT_242, &ws_dt, 0); ImGui::SameLine(); ImGui::SetCursorPosX(halfWidth);
            ImGui::RadioButton(TT_243, &ws_dt, 1);

            if (::GlobalConfig::getInstance()->window_main_style_id != ws_t){
                ::GlobalConfig::getInstance()->window_main_style_id = ws_t;
                if (ws_t == 0)
                    ::SetWindowPos(GlobalTemp::WindowMainHandle, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
                else if(ws_t ==1)
                    ::SetWindowPos(GlobalTemp::WindowMainHandle, HWND_TOPMOST , 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
                ::GlobalConfig::GlobalConfigSave();
            }
            if (::GlobalConfig::getInstance()->window_main_dock_id != ws_d) {
                ::GlobalConfig::getInstance()->window_main_dock_id = ws_d;
                if (ws_d == 0)  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
                else if(ws_d==1) io.ConfigFlags &= (~ImGuiConfigFlags_DockingEnable); 
                ::GlobalConfig::GlobalConfigSave();
            }
            if (::GlobalConfig::getInstance()->window_main_transparent_id != ws_dt) {
                ::GlobalConfig::getInstance()->window_main_transparent_id = ws_dt;
                if (ws_dt == 0)  io.ConfigDockingTransparentPayload = true;
                else if (ws_dt == 1) io.ConfigDockingTransparentPayload = false;;
                ::GlobalConfig::GlobalConfigSave();
            }
            ImGui::SeparatorText(TT_276);
            ImGui::RadioButton(TT_274, &ws_f, 0);
            ImGui::SameLine(); ImGui::SetCursorPosX(halfWidth);
            ImGui::RadioButton(TT_275, &ws_f, 1);
            if (::GlobalConfig::getInstance()->window_main_fast_id != ws_f) {
                ::GlobalConfig::getInstance()->window_main_fast_id = ws_f;
                ::GlobalConfig::GlobalConfigSave();
            }

            ImGui::SeparatorText(TT_233);
            ImGui::SliderFloat(TT_236, &GlobalConfig::getInstance()->window_main_forecastfps, 10.0f, 120.0f, "%.2f fps", ImGuiSliderFlags_None);
            ImGui::SliderInt(TT_237, &GlobalConfig::getInstance()->window_main_addtimefps, 0, 100, TT_247, ImGuiSliderFlags_None);
            ImGui::SameLine(); HelpMarker(TT_246);
            ImGui::Text(TT_42, 1000.0f / io.Framerate, io.Framerate);

            ImGui::TreePop();
        }
        if (ImGui::TreeNode(TT_225)) {
            ImGui::SeparatorText(TT_230);
            static int e = ::GlobalConfig::getInstance()->window_cubism_style_id;
            ImGui::RadioButton(TT_226, &e, 0); ImGui::SameLine(); ImGui::SetCursorPosX(halfWidth * 0.5);
            ImGui::RadioButton(TT_227, &e, 1); ImGui::SameLine(); ImGui::SetCursorPosX(halfWidth);
            ImGui::RadioButton(TT_228, &e, 2); ImGui::SameLine(); ImGui::SetCursorPosX(halfWidth * 1.5);
            ImGui::RadioButton(TT_229, &e, 3);
            if (GlobalConfig::getInstance()->window_cubism_style_id != e) {
                CubismLoom::addMessageList(::window_group_mark, ::window_style_mark, std::to_string(e).c_str());
                ::GlobalConfig::GlobalConfigSave();
            }

            ImGui::SeparatorText(TT_248);
            ImGui::Text(TT_250);

            ImGui::SliderInt(TT_237, &GlobalConfig::getInstance()->window_cubism_addtimefps, 0, 100, TT_247, ImGuiSliderFlags_None);
            ImGui::SameLine(); HelpMarker(TT_246);

            ImGui::Text(TT_249, GlobalTemp::CubismFrameCount);
            ImGui::TreePop();
        }
    }

}

void ShowShinobuUser()
{
    if (ImGui::CollapsingHeader(TT_32))
    {
        ImGui::BeginGroup();//G1
        // Left
        static int selected = -1;
        ImGui::BeginChild("###BeginChild___ShowShinobuUser_A", ImVec2(150,400), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);//C1
        for (int i = 0; i < Su::UserConfig::getUserVector().size(); i++)
        {
            char label[128];
            sprintf_s(label, sizeof(label), "MyObject %d", Su::UserConfig::getUserVector()[i].user_id);
            if (ImGui::Selectable(label, selected == i))
                selected = i;
        }
        ImGui::EndChild();//C1


        ImGui::SameLine();
        // Right
        ImGui::BeginChild("###BeginChild___ShowShinobuUser_B", ImVec2(0, 400), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);//C2
        ImGui::BeginGroup();//G2
        if (ImGui::Button("ADD", ImVec2(150,0))) {
            int pos = Su::UserConfig::getUserVector().size();
            cout << u8"添加新用户 ID:" << pos + 1 << endl;
            Su::UserConfig::getUserVector().push_back(Su::UserConfig(pos + 1));

            ::GlobalConfig::getInstance()->user_num = Su::UserConfig::getUserVector().size();
            Su::AllConfigSave();
        }
        ImGui::SameLine();
        if (ImGui::Button("DELETE", ImVec2(150, 0))) {
            if (selected != -1) {
                Su::UserConfig::getUserVector().erase(Su::UserConfig::getUserVector().begin() + selected);
                for (int i = 0; i < (int)Su::UserConfig::getUserVector().size(); ++i) {
                    Su::UserConfig::getUserVector()[i].exist = true;
                    Su::UserConfig::getUserVector()[i].user_id = i + 1;
                }
                selected = -1;
                ::GlobalConfig::getInstance()->user_num = Su::UserConfig::getUserVector().size();
                Su::AllConfigSave();
            }
        }
        if (selected == -1) {
            ImGui::EndGroup();//G2
            ImGui::EndChild();//C2
            ImGui::EndGroup();//G1
            return;
        }
        ImGui::BeginChild("item view", ImVec2(0, 0));//C3
        Su::UserConfig* _uc = &Su::UserConfig::getUserVector()[selected];
        ImGui::Text("UserID: %d", _uc->user_id);
        ImGui::Separator();
        if (ImGui::BeginTabBar("###BeginTabBar___ShowShinobuUser", ImGuiTabBarFlags_None)) {
            if (ImGui::BeginTabItem(TT_267)) { ShowShinobuCSM(_uc); ImGui::EndTabItem(); }
            if (ImGui::BeginTabItem(TT_7)) { ShowShinobuLLM(_uc); ImGui::EndTabItem(); }
            if (ImGui::BeginTabItem(TT_10)) { ShowShinibuTTS(_uc); ImGui::EndTabItem(); }
            if (ImGui::BeginTabItem(TT_12)) { ShowShinobuSTT(_uc); ImGui::EndTabItem(); }
            if (ImGui::BeginTabItem(TT_14)) { ShowShinobuMT(_uc); ImGui::EndTabItem(); }
            ImGui::EndTabBar();
        }
        ImGui::EndChild();//C3
        ImGui::EndGroup();//G2
        ImGui::EndChild();//C2
        ImGui::EndGroup();//G1


    }

    return;
    if (ImGui::CollapsingHeader(TT_32))
    {
        if (ImGui::BeginTabBar("###SettingBar", ImGuiTabBarFlags_None))
        {
            if (ImGui::TabItemButton("+", ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoTooltip))
            {
                int pos = Su::UserConfig::getUserVector().size();
                cout << u8"添加新用户 ID:" << pos + 1 << endl;
                Su::UserConfig::getUserVector().push_back(Su::UserConfig(pos + 1));

                ::GlobalConfig::getInstance()->user_num = Su::UserConfig::getUserVector().size();
                Su::AllConfigSave();
            }
            static char tabname[DEFSIZE];
            memset(tabname, 0, sizeof(tabname));
            int erase_pos = -1;
            for (int i = 0; i < (int)Su::UserConfig::getUserVector().size(); ++i) {
                strcpy_s(tabname, sizeof(tabname), TT_33);
                snprintf(tabname, sizeof(tabname), tabname, Su::UserConfig::getUserVector()[i].user_id);
                sprintf_s(tabname,sizeof(tabname), "%s###%d", tabname, Su::UserConfig::getUserVector()[i].user_id);

                if (ImGui::BeginTabItem(tabname, &Su::UserConfig::getUserVector()[i].exist, ImGuiTabItemFlags_None))
                {
                    Su::UserConfig* _uc = &Su::UserConfig::getUserVector()[i];
                    ShowShinobuCSM(_uc);
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
                    Su::UserConfig::getUserVector()[i].user_id = i + 1;
                }
                erase_pos = -1;
                ::GlobalConfig::getInstance()->user_num = Su::UserConfig::getUserVector().size();
                Su::AllConfigSave();
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
            SaveKasb(_uc, llmc);
        }
    };
    auto ft = [fb](const char* nodename, Su::LLMConfig* llmc, const char* butname) {
        static char button[DEFSIZE];
        Su::GetGuiMark(button, sizeof(button), butname, llmc->name);
        KASB_FUNCTION(llmc)
        fb(button, llmc);
    };
    auto fc = [_uc](const char* comtitle) {
        CM_FUNCTION(comtitle, _uc->llms, _uc->select_llm, Su::LLM)
    };
    auto f = [_uc, ft, fb, fc](const char* headertitle, const char* butname) {
        ImGui::SeparatorText(headertitle);
        fc(TT_6);
        for (auto& val : _uc->llms) {
            if(val.llm==_uc->select_llm)
                ft(val.getLanName(), &val, butname);
        }

    };
    f(TT_7, TT_8);
}

void ShowShinibuTTS(Su::UserConfig* _uc) {
    auto fb = [_uc](const char* butname, Su::TTSConfig* ttsc) {
        if (ImGui::Button(butname))
        {
            std::cout << butname << std::endl;
            UserConfigSave(_uc);

        }
    };
    auto ft = [_uc,fb](const char* nodename, Su::TTSConfig* ttsc, const char* butname) {

        static char button[DEFSIZE];
        static char input[DEFSIZE];
        static char input_value[DEFSIZE];
        if (ttsc->tts == Su::TTS::VITS_SIMPLE_API) {
            AP_FUNCTION(ttsc);
            Su::MemsetStr(input,sizeof(input));
            strcpy_s(input, sizeof(input),TT_27);
            {
                if (ImGui::BeginCombo(input, ::vits_simple_api_model_list[_uc->vits_simple_api_model_id])) {
                    for (int n = 0; n < IM_ARRAYSIZE(::vits_simple_api_model_list); n++) {
                        const bool is_selected = ((int)_uc->vits_simple_api_model_id == n);
                        if (ImGui::Selectable(::vits_simple_api_model_list[n], is_selected)) {
                            _uc->vits_simple_api_model_id = n;
                            UserConfigSave(_uc);
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
            Su::MemsetStr(input, sizeof(input));
            strcpy_s(input, sizeof(input), TT_27);
            {
                if (ImGui::BeginCombo(input, ::gpt_sovits_target_language_list[_uc->gpt_sovits_target_language_id])) {
                    for (int n = 0; n < IM_ARRAYSIZE(::gpt_sovits_target_language_list); n++) {
                        const bool is_selected = ((int)_uc->gpt_sovits_target_language_id == n);
                        if (ImGui::Selectable(::gpt_sovits_target_language_list[n], is_selected)) {
                            _uc->gpt_sovits_target_language_id = n;
                            UserConfigSave(_uc);
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
            Su::GetGuiMark(url, sizeof(url), TT_31, ttsc->name);
            ImGui::InputText(url, input_url, IM_ARRAYSIZE(input_url));
            _uc->myself_vits.assign(input_url);

        }

        Su::GetGuiMark(button, sizeof(button), butname, ttsc->name);
        fb(button, ttsc);

    };
    auto fc = [_uc](const char* comtitle) {
        CM_FUNCTION(comtitle, _uc->ttss, _uc->select_tts, Su::TTS)
    };
    auto f = [_uc, ft, fb, fc](const char* headertitle, const char* butname) {
        ImGui::SeparatorText(headertitle);
        fc(TT_9);
        for (auto& val : _uc->ttss) {
            if(val.tts==_uc->select_tts)
                ft(val.getLanName(), &val, butname);
        }

    };
    f(TT_10, TT_8);
}

void ShowShinobuSTT(Su::UserConfig* _uc) {
    auto fb = [_uc](const char* butname, Su::STTConfig* sttc) {
        if (ImGui::Button(butname))
        {
            std::cout << butname << std::endl;
            SaveKasb(_uc, sttc);
        }
    };
    auto ft = [fb](const char* nodename, Su::STTConfig* sttc, const char* butname) {

        static char button[DEFSIZE];
        Su::GetGuiMark(button, sizeof(button), butname, sttc->name);
        KASB_FUNCTION(sttc)
        fb(button, sttc);
    };
    auto fc = [_uc](const char* comtitle) {
        CM_FUNCTION(comtitle, _uc->stts, _uc->select_stt, Su::STT)
    };
    auto f = [_uc, ft, fb, fc](const char* headertitle, const char* butname) {
        ImGui::SeparatorText(headertitle);
        fc(TT_11);
        for (auto& val : _uc->stts)
        {
            if(val.stt==_uc->select_stt)
                ft(val.getLanName(), &val, butname);
        }
    };
    f(TT_12, TT_8);

}
void ShowShinobuMT(Su::UserConfig* _uc) {
    auto fb = [_uc](const char* butname, Su::MTConfig* mtc) {
        if (ImGui::Button(butname))
        {
            std::cout << butname << std::endl;
            SaveKasb(_uc, mtc);
        }
    };
    auto ft = [fb](const char* nodename, Su::MTConfig* mtc, const char* butname) {

            static char button[DEFSIZE];
            Su::GetGuiMark(button, sizeof(button), butname, mtc->name);
            KASB_FUNCTION(mtc)
            fb(button, mtc);
    };
    auto fc = [_uc](const char* comtitle) {
        CM_FUNCTION(comtitle, _uc->mts, _uc->select_mt, Su::MT)
    };
    auto f = [_uc, ft, fb, fc](const char* headertitle, const char* butname) {
        ImGui::SeparatorText(headertitle);
        fc(TT_13);
        for (auto& val : _uc->mts) {
            if (val.mt == _uc->select_mt)
                ft(val.getLanName(), &val, butname);
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
            ImGui::PlotLines("###PlotLines___ShowShinobuAbout", samples, 100, NULL, NULL, -1.0f, 1.0f, ImVec2(ImGui::GetContentRegionAvail().x, 0.0));
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
    ShowShinobuLunar();
    ShowShinobuLanguage();
    ShowShinobuGlobal();
    ShowShinobuStart();
    ShowShinobuCubism();
 

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
    //        Su::SparkDesk(&Su::UserConfig::getUserVector()[0].getLLMConfig(Su::LLM::SPARKDESK)->kasb, text, sizeof(text), text2);
    //    }

    //    //static char user[DEFSIZEK16];
    //    //ImGui::InputText("user input", user, IM_ARRAYSIZE(user));

    //}


    ShowShinobuUser();
    ShowShinobuHelp();
    ShowShinobuAbout();

    ImGui::End();

    //


    
}
