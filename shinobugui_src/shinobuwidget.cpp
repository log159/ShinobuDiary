#include "shinobuwidget.h"
#include <iostream>
#include <string>
#include <sstream>
#include <windows.h>
#include <CubismFramework.hpp>
#include <./Id/CubismIdManager.hpp>
#include "../imgui_src/imgui.h"
#include "../imgui_impl_win32.h"
#include "../imgui_impl_dx11.h"
#include "../implot_src/implot.h"
#include "../implot_src/implot_internal.h"
#include "../translator.h"
#include "../shinobugui_src/sufunction.h"
#include "../cubism_src/LAppLive2DManager.hpp"
#include "../cubism_src/LAppModel.hpp"
#include "../cubism_src/LAppDefine.hpp"
#include "../cubism_src/LAppDelegate.hpp"
#include "./compoundgui.h"
//#include "llmhome.h"

void ShowShinobuHead() {
    //FPS测试条
    ImGui::ProgressBar(-0.5f * (float)ImGui::GetTime(), ImVec2(ImGui::GetContentRegionAvail().x, 7.0f), NULL);
}

#define RTOL(L,R) if(L!=R){L=R;};
#define ERTOL_A \
case 0:RTOL(bo, uc.enable_widget) break;\
case 1:RTOL(bo, uc.enable_cubism) break;\
case 2:RTOL(bo, uc.enable_template) break;\
case 3:RTOL(bo, uc.enable_tts) break;\
case 4:RTOL(bo, uc.enable_mt) break;\
case 5:RTOL(bo, uc.enable_original) break;\
case 6:RTOL(bo, uc.enable_stt) break;

#define ERTOL_B \
case 0:RTOL(uc.enable_widget, bo) break;\
case 1:RTOL(uc.enable_cubism, bo) break;\
case 2:RTOL(uc.enable_template, bo) break;\
case 3:RTOL(uc.enable_tts, bo) break;\
case 4:RTOL(uc.enable_mt, bo) break;\
case 5:RTOL(uc.enable_original, bo) break;\
case 6:RTOL(uc.enable_stt, bo) break;

void ShowShinobuStart()
{
    static std::vector<std::string>column_names_v;
    static int column_names_size = (int)Su::UserConfig::getUserVector().size() + 1;
    static const char* column_names[DEFSIZE];
    const char* rows_names[] = { TT_258,TT_259/* ,TT_260, TT_261, TT_262,TT_263,TT_264*/ };
    static ImGuiTableFlags table_flags = ImGuiTableFlags_ScrollX | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_Hideable | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_HighlightHoveredColumn;
    static ImGuiTableColumnFlags column_flags = ImGuiTableColumnFlags_AngledHeader | ImGuiTableColumnFlags_WidthFixed;
    static int frozen_cols = 1;
    static int frozen_rows = 2;
    int columns_count = column_names_size;
    const int rows_count = IM_ARRAYSIZE(rows_names);
    float row_height = ImGui::GetTextLineHeightWithSpacing();
    float table_height = row_height * rows_count + 10.0f;
    table_height = max(table_height, 300.f);
    table_height = min(table_height, 1000.f);

    if (GlobalTemp::RefreshTable == true) {
        GlobalTemp::RefreshTable = false;
        memset(GlobalTemp::TableBools, false, sizeof(GlobalTemp::TableBools));
        column_names_size = (int)Su::UserConfig::getUserVector().size() + 1;
        std::cout << u8"变化用户表格实例" << std::endl;
        column_names_v.clear();
        column_names_v.reserve(column_names_size);
        static char tabname[DEFSIZEK16];
        memset(tabname, 0, sizeof(tabname));
        column_names[0] = TT_265;
        for (int i = 0; i < column_names_size-1; ++i) {
            strcpy_s(tabname, sizeof(tabname), TT_33);
            snprintf(tabname, sizeof(tabname), tabname, Su::UserConfig::getUserVector()[i].user_id);
            column_names_v.push_back(std::string(tabname));
            column_names[i + 1] = column_names_v.back().c_str();
        }
        for (int col = 1; col < columns_count; ++col) {
            for (int row = 0; row < rows_count; ++row) {
                bool& bo = GlobalTemp::TableBools[row][col];
                Su::UserConfig& uc = Su::UserConfig::getUserVector()[col - 1];
                switch (row) {
                    ERTOL_A
                }
            }
        }
    }
    for (int col = 1; col < columns_count; ++col) {
        for (int row = 0; row < rows_count; ++row) {
            const bool& bo = GlobalTemp::TableBools[row][col];
            Su::UserConfig& uc = Su::UserConfig::getUserVector()[col - 1];
            if (row == 1) {
                if (uc.enable_cubism != bo) {
                    if (bo) {
                        std::cout << u8"用户：" << uc.user_id << " Cubism F -> T" << std::endl;
                        GlobalTemp::CubismModelMessage.push(std::make_pair(uc.user_id, uc.cubism_config.model_dir));
                    }
                    else {
                        std::cout << u8"用户：" << uc.user_id << " Cubism T -> F" << std::endl;
                        GlobalTemp::CubismModelMessage.push(std::make_pair(uc.user_id, ""));
                    }
                    GlobalTemp::RefreshCubism = true;
                }
            }
            switch (row) {
                ERTOL_B
            }
        }
    }

    if (ImGui::CollapsingHeader(GETGUIID(TT_256)))
    {
        if (Su::UserConfig::getUserVector().size() <= 0) {
            ImGui::Text(TT_257);
            return;
        }
        
        if (ImGui::BeginTable("###BeginTable___ShowShinobuStart", columns_count, table_flags, ImVec2(0.0f, table_height)))
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
                        ImGui::Checkbox("###Checkbox___ShowShinobuStart", &GlobalTemp::TableBools[row][column]);
                        ImGui::PopID();
                    }
                ImGui::PopID();
            }
            ImGui::EndTable();
        }

        if (ImGui::Button(TT_8, ImVec2(GlobalTemp::GuiButtonWidth,0))) {
            GlobalTemp::RefreshCubism = true;
            Su::AllConfigSave();
        }
        ImGui::SameLine();
        if (ImGui::Button(TT_294, ImVec2(GlobalTemp::GuiButtonWidth, 0))) {
            std::vector<Su::UserConfig>& vu = Su::UserConfig::getUserVector();
            for (int i = 0; i < int(vu.size()); ++i)
                if (vu[i].enable_cubism)
                    GlobalTemp::CubismModelMessage.push(std::make_pair(vu[i].user_id, ""));
            GlobalTemp::RefreshCubism = true;
            for (int col = 1; col < columns_count; ++col) {
                for (int row = 0; row < rows_count; ++row) {
                    bool& bo = GlobalTemp::TableBools[row][col];
                    bo = false;
                    Su::UserConfig& uc = Su::UserConfig::getUserVector()[col - 1];
                    switch (row) {
                        ERTOL_B
                    }
                }
            }
        }
    }
}
void ShowShinobuStyleEditor()
{
    if (GlobalTemp::ShowStyleEditor) {
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
        ImGui::Begin(GETGUIID(TT_39), &GlobalTemp::ShowStyleEditor);
        ImGui::ShowStyleEditor();
        ImGui::End();
    }
}
void ShowShinobuInteractively()
{
    //Cubism 相关
    if (GlobalTemp::CubismIsRunning == false)return;
    static char winname[DEFSIZE];
    std::unordered_map<int, LAppModel*>& models= LAppLive2DManager::GetInstance()->GetModel();
    std::vector<Su::UserConfig>& uv = Su::UserConfig::getUserVector();
    for (int iuv = 0; iuv < int(uv.size()); ++iuv) {
        if (models.find(uv[iuv].user_id) == models.end())
            continue;
        Su::UserConfig& uc = uv[iuv];
        LAppModel* lam= models[uc.user_id];
        Csm::ICubismModelSetting* lcms = lam->GetModelSetting();
        if (lam == NULL) {
            std::cout << "LAppModel pointer is nullptr!" << std::endl;
            continue;
        }
        if (lam->GetModelMatrix() == nullptr || lam->GetModel() == nullptr || lam->GetModelSetting() ==nullptr) {
            continue;
        }
        if (uc.enable_widget) {
            Su::MemsetStr(winname,sizeof(winname));
            strcpy_s(winname, sizeof(winname), GETGUIID(TT_279));
            strcat_s(winname, sizeof(winname), "_%d");
            snprintf(winname, sizeof(winname), winname, uc.user_id, uc.user_id);
            ImGui::Begin(winname, &GlobalTemp::TableBools[0][uc.user_id]);
            const char* tab_buf[DEFSIZE] = { TT_366,TT_367,TT_368,TT_369,TT_370,TT_371,TT_372,TT_376 };
            const int tab_size = 8;
            int& select_id = lam->function_selected;
            if (ImGui::BeginCombo(TT_365, tab_buf[select_id])) {
                for (int n = 0; n < tab_size; n++) {
                    const bool is_selected = (select_id == n);
                    if (ImGui::Selectable(tab_buf[n], is_selected))
                        select_id = n;
                    if (is_selected)ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            switch (select_id) {
                case 0:ShowShinobuCsmGeometry(uc, lam); break;
                case 1:ShowShinobuCsmLook(uc, lam); break;
                case 2:ShowShinobuCsmBreath(uc, lam); break;
                case 3:ShowShinobuCsmBlink(uc, lam); break;
                case 4:ShowShinobuCsmAnimation(uc, lam); break;
                case 5:ShowShinobuCsmHitareas(uc, lam); break;
                case 6:ShowShinobuCsmRendering(uc, lam); break;
                case 7:ShowShinobuCsmObserve(uc, lam); break;
                default:break;
            }
            ImGui::End();
            ShowShinobuCsmObserveSeparate(uc, lam);
        }
    }
}
void ShowShinobuCubism()
{
    if (GlobalTemp::CubismIsRunning == false)return;
    if (!GlobalTemp::RefreshCubism)return;GlobalTemp::RefreshCubism = false;
    //发送鼠标点击事件
    HWND hwnd = LAppDelegate::GetInstance()->GetWindowHandle();
    if (!hwnd)return;
    if (hwnd != NULL) {
        POINT pt;
        if (GetCursorPos(&pt)) {
            std::cout << u8"鼠标当前位置: (" << pt.x << ", " << pt.y << ")" << std::endl;
        }
        LPARAM lParam = MAKELPARAM(pt.x, pt.y);
        WPARAM wParam = MK_LBUTTON;
        PostMessage(hwnd, WM_LBUTTONDOWN, wParam, lParam);
        PostMessage(hwnd, WM_LBUTTONUP, 0, lParam);
    }
}
void ShowShinobuCSM(Su::UserConfig* _uc)
{
    ImGui::SeparatorText(GETGUIID(TT_267));
    Csm::csmVector<Csm::csmString>& csmModelDir = LAppLive2DManager::GetInstance()->GetModelDir();
    if (csmModelDir.GetSize() <= 0) {
        if (ImGui::BeginCombo(TT_268, TT_269)) {
            ImGui::EndCombo();
        };
        return;
    }
    static const char* modelDirs[DEFSIZEK16];
    static int modelDirsSize = -1;
    int select = ([_uc,csmModelDir]()->int {
        int pos = 0;
        for (int i = 0; i < int(csmModelDir.GetSize()); ++i) {
            if (strcmp(_uc->cubism_config.model_dir.c_str(),csmModelDir[i].GetRawString()) == 0) {
                pos = i;break;
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
                if (strcmp(_uc->cubism_config.model_dir.c_str(), (modelDirs[n] == nullptr ? "" : modelDirs[n])) != 0) {
                    _uc->cubism_config.model_dir = std::string(modelDirs[n] == nullptr ? "" : modelDirs[n]);
                        std::cout << _uc->user_id << " " << _uc->cubism_config.model_dir.c_str() << std::endl;
                        std::cout << u8"用户：" << _uc->user_id << " Cubism T -> T" << std::endl;
                        _uc->need_init_cubism = true;
                    Su::UserConfigSave(_uc);
                }
                else {
                    std::cout << u8"重复选择,无事发生" << std::endl;
                }
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
    f(GETGUIID(TT_5));
}
void ShowShinobuStyle()
{
    if(ImGui::CollapsingHeader(GETGUIID(TT_40))) {
        ImGui::Checkbox(TT_41, &GlobalTemp::ShowStyleEditor);
    }
}
void ShowShinobuGlobal()
{
    if (ImGui::CollapsingHeader(GETGUIID(TT_224)))
    {
        ImGuiStyle& sty = ImGui::GetStyle();
        float halfWidth = ImGui::GetWindowSize().x / 2.0f;

        if (ImGui::TreeNode(GETGUIID(TT_231))) {

            ImGuiIO& io = ImGui::GetIO(); (void)io;
            static int 
                ws_c = ::GlobalConfig::getInstance()->window_main_close,
                ws_t = ::GlobalConfig::getInstance()->window_main_style_id,
                ws_d = ::GlobalConfig::getInstance()->window_main_dock_id,
                ws_dt = ::GlobalConfig::getInstance()->window_main_transparent_id,
                ws_f = ::GlobalConfig::getInstance()->window_main_fast_id;
            ImGui::SeparatorText(TT_431);
            ImGui::RadioButton(TT_432, &ws_c, 0); ImGui::SameLine(); ImGui::SetCursorPosX(halfWidth + sty.IndentSpacing);
            ImGui::RadioButton(TT_433, &ws_c, 1);
            if (GlobalConfig::getInstance()->window_main_close != ws_c) {
                GlobalConfig::getInstance()->window_main_close = ws_c;
            }
            ImGui::SeparatorText(TT_232);
            ImGui::RadioButton(TT_234, &ws_t, 0); ImGui::SameLine(); ImGui::SetCursorPosX(halfWidth + sty.IndentSpacing);
            ImGui::RadioButton(TT_235, &ws_t, 1);
            ImGui::SeparatorText(TT_239);
            ImGui::RadioButton(TT_240, &ws_d, 0); ImGui::SameLine(); ImGui::SetCursorPosX(halfWidth + sty.IndentSpacing);
            ImGui::RadioButton(TT_241, &ws_d, 1);
            ImGui::RadioButton(TT_242, &ws_dt, 0); ImGui::SameLine(); ImGui::SetCursorPosX(halfWidth + sty.IndentSpacing);
            ImGui::RadioButton(TT_243, &ws_dt, 1);

            if (::GlobalConfig::getInstance()->window_main_style_id != ws_t){
                ::GlobalConfig::getInstance()->window_main_style_id = ws_t;
                if (ws_t == 0)
                    ::SetWindowPos(GlobalTemp::WindowMainHandle, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
                else if(ws_t ==1)
                    ::SetWindowPos(GlobalTemp::WindowMainHandle, HWND_TOPMOST , 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
            }
            if (::GlobalConfig::getInstance()->window_main_dock_id != ws_d) {
                ::GlobalConfig::getInstance()->window_main_dock_id = ws_d;
                if (ws_d == 0)  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
                else if(ws_d==1) io.ConfigFlags &= (~ImGuiConfigFlags_DockingEnable); 
            }
            if (::GlobalConfig::getInstance()->window_main_transparent_id != ws_dt) {
                ::GlobalConfig::getInstance()->window_main_transparent_id = ws_dt;
                if (ws_dt == 0)  io.ConfigDockingTransparentPayload = true;
                else if (ws_dt == 1) io.ConfigDockingTransparentPayload = false;;
            }
            ImGui::SeparatorText(TT_276);
            ImGui::RadioButton(TT_274, &ws_f, 0);
            ImGui::SameLine(); ImGui::SetCursorPosX(halfWidth + sty.IndentSpacing);
            ImGui::RadioButton(TT_275, &ws_f, 1);
            if (::GlobalConfig::getInstance()->window_main_fast_id != ws_f) {
                ::GlobalConfig::getInstance()->window_main_fast_id = ws_f;
            }
            ImGui::SeparatorText(TT_434);
            ImGui::SliderFloat(TT_113, &GlobalConfig::getInstance()->window_main_alpha, 30.0f, 100.0f, "%.2f", ImGuiSliderFlags_None);

            ImGui::SeparatorText(TT_233);
            ImGui::SliderFloat(TT_236, &GlobalConfig::getInstance()->window_main_forecastfps, 10.0f, 120.0f, "%.2f fps", ImGuiSliderFlags_None);
            ImGui::SliderInt(TT_237, &GlobalConfig::getInstance()->window_main_addtimefps, 0, 100, TT_247, ImGuiSliderFlags_None);
            ImGui::SameLine(); Su::HelpMarker(TT_246);
            ImGui::Text(TT_42, 1000.0f / io.Framerate, io.Framerate);

            ImGui::TreePop();
        }
        if (ImGui::TreeNode(GETGUIID(TT_225))) {
            ImGui::SeparatorText(TT_230);
            static int e = ::GlobalConfig::getInstance()->window_cubism_style_id;
            ImGui::RadioButton(TT_226, &e, 0); ImGui::SameLine(); ImGui::SetCursorPosX(halfWidth * 0.5f+ sty.IndentSpacing);
            ImGui::RadioButton(TT_227, &e, 1); ImGui::SameLine(); ImGui::SetCursorPosX(halfWidth+ sty.IndentSpacing);
            ImGui::RadioButton(TT_228, &e, 2); ImGui::SameLine(); ImGui::SetCursorPosX(halfWidth * 1.5f+ sty.IndentSpacing);
            ImGui::RadioButton(TT_229, &e, 3);
            if (GlobalConfig::getInstance()->window_cubism_style_id != e) {
                GlobalConfig::getInstance()->window_cubism_style_id = e;
            }
            ImGui::SeparatorText(TT_248);
            ImGui::Text(TT_250);
            ImGui::SliderInt(TT_237, &GlobalConfig::getInstance()->window_cubism_addtimefps, 0, 100, TT_247, ImGuiSliderFlags_None);
            ImGui::SameLine(); Su::HelpMarker(TT_246);
            ImGui::Text(TT_249, GlobalTemp::CubismFrameCount);
            ImGui::TreePop();
        }
    }
}
void ShowShinobuUser()
{
    if (ImGui::CollapsingHeader(GETGUIID(TT_32)))
    {
        ImGui::BeginGroup();//G1
        // Left
        static int selected = -1;
        ImGui::BeginChild("###BeginChild___ShowShinobuUser_L", ImVec2(150,400), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);//C1
        for (int i = 0; i < int(Su::UserConfig::getUserVector().size()); i++)
        {
            static char label[DEFSIZE];
            sprintf_s(label, sizeof(label), TT_33, Su::UserConfig::getUserVector()[i].user_id);
            if (ImGui::Selectable(label, selected == i))
                selected = i;
        }
        ImGui::EndChild();//C1
        ImGui::SameLine();
        // Right
        ImGui::BeginChild("###BeginChild___ShowShinobuUser_R", ImVec2(0, 400), ImGuiChildFlags_Borders);//C2
        ImGui::BeginGroup();//G2
        if (ImGui::Button(TT_277, ImVec2(GlobalTemp::GuiButtonWidth,0))) {
            int pos = Su::UserConfig::getUserVector().size();
            std::cout << u8"添加新用户 ID:" << pos + 1 << std::endl;
            Su::UserConfig::getUserVector().push_back(Su::UserConfig(pos + 1));

            ::GlobalConfig::getInstance()->user_num = Su::UserConfig::getUserVector().size();
            Su::AllConfigSave();
        }
        ImGui::SameLine();
        if (ImGui::Button(TT_278, ImVec2(GlobalTemp::GuiButtonWidth, 0))) {
            if (selected != -1) {
                std::vector<Su::UserConfig>& vu = Su::UserConfig::getUserVector();
                int euid = vu[selected].user_id;
                vu.erase(vu.begin() + selected);
                for (int i = 0; i < (int)vu.size(); ++i) {
                    vu[i].user_id = i + 1;
                }
                ::GlobalConfig::getInstance()->user_num = vu.size();
                Su::AllConfigSave();
                selected = -1;
                RemoveUserHandle(euid);
            }
        }
        if (selected == -1) {
            ImGui::EndGroup();//G2
            ImGui::EndChild();//C2
            ImGui::EndGroup();//G1
            return;
        }
        ImGui::BeginChild("###BeginChild___ShowShinobuUser", ImVec2(0, 0));//C3
        Su::UserConfig* _uc = &Su::UserConfig::getUserVector()[selected];
        ImGui::Text(TT_33, _uc->user_id);
        ImGui::Separator();
        if (ImGui::BeginTabBar("###BeginTabBar___ShowShinobuUser", ImGuiTabBarFlags_None)) {
            if (ImGui::BeginTabItem(GETGUIID(TT_267))) { ShowShinobuCSM(_uc); ImGui::EndTabItem(); }
            //if (ImGui::BeginTabItem(TT_7)) { ShowShinobuLLM(_uc); ImGui::EndTabItem(); }
            //if (ImGui::BeginTabItem(TT_10)) { ShowShinibuTTS(_uc); ImGui::EndTabItem(); }
            //if (ImGui::BeginTabItem(TT_12)) { ShowShinobuSTT(_uc); ImGui::EndTabItem(); }
            //if (ImGui::BeginTabItem(TT_14)) { ShowShinobuMT(_uc); ImGui::EndTabItem(); }
            ImGui::EndTabBar();
        }
        ImGui::EndChild();//C3
        ImGui::EndGroup();//G2
        ImGui::EndChild();//C2
        ImGui::EndGroup();//G1
    }
}
void ShowShinobuErrorWindow()
{
    if (GlobalTemp::CubismIsRunning == false)
        ShowShinobuError(SUERROR::CUBISM_NOT_RUNNING);//Error Window
}
void ShowShinobuLLM(Su::UserConfig* _uc) {
    auto fb = [_uc](const char* butname) {
        if (ImGui::Button(butname, ImVec2(GlobalTemp::GuiButtonWidth, 0)))
        {
            std::cout << butname << std::endl;
            Su::UserConfigSave(_uc);
        }
    };
    auto ft = [fb](const char* nodename, Su::LLMConfig* llmc, const char* butname) {
        static char button[DEFSIZE];
        Su::JoinGuiMark(button, sizeof(button), butname, llmc->name);
        KASB_FUNCTION(llmc)
        fb(button);
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
    f(GETGUIID(TT_7), TT_8);
}

void ShowShinibuTTS(Su::UserConfig* _uc) {
    auto fb = [_uc](const char* butname) {
        if (ImGui::Button(butname, ImVec2(GlobalTemp::GuiButtonWidth, 0)))
        {
            std::cout << butname << std::endl;
            Su::UserConfigSave(_uc);
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
            static char input_url[DEFSIZEK16];
            strcpy_s(input_url, sizeof(input_url), _uc->myself_vits.c_str());
            Su::JoinGuiMark(url, sizeof(url), TT_31, ttsc->name);
            ImGui::InputText(url, input_url, IM_ARRAYSIZE(input_url));
            _uc->myself_vits.assign(input_url);

        }

        Su::JoinGuiMark(button, sizeof(button), butname, ttsc->name);
        fb(button);

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
    f(GETGUIID(TT_10), TT_8);
}

void ShowShinobuSTT(Su::UserConfig* _uc) {
    auto fb = [_uc](const char* butname) {
        if (ImGui::Button(butname, ImVec2(GlobalTemp::GuiButtonWidth, 0)))
        {
            std::cout << butname << std::endl;
            Su::UserConfigSave(_uc);
        }
    };
    auto ft = [fb](const char* nodename, Su::STTConfig* sttc, const char* butname) {

        static char button[DEFSIZE];
        Su::JoinGuiMark(button, sizeof(button), butname, sttc->name);
        KASB_FUNCTION(sttc)
        fb(button);
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
    f(GETGUIID(TT_12), TT_8);

}
void ShowShinobuMT(Su::UserConfig* _uc) {
    auto fb = [_uc](const char* butname) {
        if (ImGui::Button(butname, ImVec2(GlobalTemp::GuiButtonWidth, 0)))
        {
            std::cout << butname << std::endl;
            Su::UserConfigSave(_uc);
        }
    };
    auto ft = [fb](const char* nodename, Su::MTConfig* mtc, const char* butname) {

            static char button[DEFSIZE];
            Su::JoinGuiMark(button, sizeof(button), butname, mtc->name);
            KASB_FUNCTION(mtc)
            fb(button);
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
    f(GETGUIID(TT_14), TT_8);
}
void ShowShinobuHelp() {
    if (ImGui::CollapsingHeader(GETGUIID(TT_15)))
    {
        ImGui::SeparatorText(TT_16);
        ImGui::BulletText(TT_17);
    }
}
void ShowShinobuAbout() {

    if (ImGui::CollapsingHeader(GETGUIID(TT_0)))
    {
        static char name_buf[DEFSIZE];
        ImGui::SeparatorText(TT_1);
        ImGui::BulletText(TT_2);
        ImGui::BulletText(TT_421);
        ImGui::SameLine();
        ImGui::TextLinkOpenURL(TT_3);
        static Su::ShinobuScrollingBuffer sdata1, sdata2;
        static Su::ShinobuRollingBuffer   rdata1, rdata2;
        ImVec2 mouse = ImGui::GetMousePos();
        static float t = 0;
        t += ImGui::GetIO().DeltaTime;
        sdata1.AddPoint(t, mouse.x * 0.0005f);
        rdata1.AddPoint(t, mouse.x * 0.0005f);
        sdata2.AddPoint(t, mouse.y * 0.0005f);
        rdata2.AddPoint(t, mouse.y * 0.0005f);
        static float history = 10.0f;
        rdata1.Span = history;
        rdata2.Span = history;
        static ImPlotAxisFlags flags = ImPlotAxisFlags_NoTickLabels;
        if (ImPlot::BeginPlot("##BeginPlot___ShowShinobuAbout", ImVec2(-1, 150))) {
            ImPlot::SetupAxes(nullptr, nullptr, flags, flags);
            ImPlot::SetupAxisLimits(ImAxis_X1, 0, history, ImGuiCond_Always);
            ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 1);
            Su::JoinGuiMark(name_buf, sizeof(name_buf), TT_390,"##PlotLine___X");
            ImPlot::PlotLine(name_buf, &rdata1.Data[0].x, &rdata1.Data[0].y, rdata1.Data.size(), 0, 0, 2 * sizeof(float));
            Su::JoinGuiMark(name_buf, sizeof(name_buf), TT_391, "##PlotLine___Y");
            ImPlot::PlotLine(name_buf, &rdata2.Data[0].x, &rdata2.Data[0].y, rdata2.Data.size(), 0, 0, 2 * sizeof(float));
            ImPlot::EndPlot();
        }
    }
}
void ShowShinobuError(SUERROR se)
{
    ImGui::Begin(GETGUIID(TT_346));
    switch (se) {
        case SUERROR::CUBISM_NOT_RUNNING:ImGui::Text(TT_345);break;
        default:break;
    }
    ImGui::End();
}

void ShowShinobuDebugWindow()
{
    ImGui::Begin(GETGUIID(TT_364));
    std::vector<std::string> dv;
    Su::StringSplit(GlobalTemp::DebugOss.str(), '\n', dv);
    for (int i = dv.size() - 1; i >= 0;i--) {
        ImGui::Text(dv[i].c_str());
    }
    ImGui::End();
}
void ShowShinobuCsmGeometry(Su::UserConfig& uc, LAppModel* lam)
{
    static char name_buf[DEFSIZE];
    ImGui::SeparatorText(TT_291);
    float& ts_s = uc.cubism_config.cubism_cg.cubism_ts_s;
    float& tx_t = uc.cubism_config.cubism_cg.cubism_tx_t;
    float& ty_t = uc.cubism_config.cubism_cg.cubism_ty_t;
    float& ts_x = uc.cubism_config.cubism_cg.cubism_ts_x;
    float& ts_y = uc.cubism_config.cubism_cg.cubism_ts_y;
    float& tx_s = uc.cubism_config.cubism_cg.cubism_tx_s;
    float& ty_s = uc.cubism_config.cubism_cg.cubism_ty_s;
    float& tx_p = uc.cubism_config.cubism_cg.cubism_tx_p;
    float& ty_p = uc.cubism_config.cubism_cg.cubism_ty_p;

    lam->GetModelMatrix()->GetArray()[12] = tx_t;
    lam->GetModelMatrix()->GetArray()[13] = ty_t;
    lam->GetModelMatrix()->GetArray()[0] = ts_x * ts_s;
    lam->GetModelMatrix()->GetArray()[5] = ts_y * ts_s;
    lam->GetModelMatrix()->GetArray()[4] = tx_s;
    lam->GetModelMatrix()->GetArray()[1] = ty_s;
    lam->GetModelMatrix()->GetArray()[7] = tx_p;
    lam->GetModelMatrix()->GetArray()[3] = ty_p;

    ImGui::DragFloat("###DragFloat___ShowShinobuInteractively_ts_s", &ts_s, 0.005f, -100.f, 100.f, "%.2f", ImGuiSliderFlags_None);//大小缩放
    ImGui::SameLine(); Su::JoinGuiMark(name_buf, sizeof(name_buf), TT_293, "ts_s");
    if (ImGui::Button(name_buf))ts_s = lam->def_cubism_cg.cubism_ts_s;
    ImGui::SameLine(); ImGui::Text(TT_280);

    ImGui::DragFloat("###DragFloat___ShowShinobuInteractively_tx_t", &tx_t, 0.005f, -100.f, 100.f, "%.2f", ImGuiSliderFlags_None);//水平坐标
    ImGui::SameLine(); Su::JoinGuiMark(name_buf, sizeof(name_buf), TT_293, "tx_t");
    if (ImGui::Button(name_buf))tx_t = lam->def_cubism_cg.cubism_tx_t;
    ImGui::SameLine(); ImGui::Text(TT_283);

    ImGui::DragFloat("###DragFloat___ShowShinobuInteractively_ty_t", &ty_t, 0.005f, -100.f, 100.f, "%.2f", ImGuiSliderFlags_None);//垂直坐标
    ImGui::SameLine(); Su::JoinGuiMark(name_buf, sizeof(name_buf), TT_293, "ty_t");
    if (ImGui::Button(name_buf))ty_t = lam->def_cubism_cg.cubism_ty_t;
    ImGui::SameLine(); ImGui::Text(TT_284);

    ImGui::DragFloat("###DragFloat___ShowShinobuInteractively_ts_x", &ts_x, 0.005f, -100.f, 100.f, "%.2f", ImGuiSliderFlags_None);//横轴缩放
    ImGui::SameLine(); Su::JoinGuiMark(name_buf, sizeof(name_buf), TT_293, "ts_x");
    if (ImGui::Button(name_buf))ts_x = lam->def_cubism_cg.cubism_ts_x;
    ImGui::SameLine(); ImGui::Text(TT_281);

    ImGui::DragFloat("###DragFloat___ShowShinobuInteractively_ts_y", &ts_y, 0.005f, -100.f, 100.f, "%.2f", ImGuiSliderFlags_None);//纵轴缩放
    ImGui::SameLine(); Su::JoinGuiMark(name_buf, sizeof(name_buf), TT_293, "ts_y");
    if (ImGui::Button(name_buf))ts_y = lam->def_cubism_cg.cubism_ts_y;
    ImGui::SameLine(); ImGui::Text(TT_282);

    ImGui::DragFloat("###DragFloat___ShowShinobuInteractively_tx_s", &tx_s, 0.005f, -100.f, 100.f, "%.2f", ImGuiSliderFlags_None);//水平拉伸
    ImGui::SameLine(); Su::JoinGuiMark(name_buf, sizeof(name_buf), TT_293, "tx_s");
    if (ImGui::Button(name_buf))tx_s = lam->def_cubism_cg.cubism_tx_s;
    ImGui::SameLine(); ImGui::Text(TT_285);

    ImGui::DragFloat("###DragFloat___ShowShinobuInteractively_ty_s", &ty_s, 0.005f, -100.f, 100.f, "%.2f", ImGuiSliderFlags_None);//垂直拉伸
    ImGui::SameLine(); Su::JoinGuiMark(name_buf, sizeof(name_buf), TT_293, "ty_s");
    if (ImGui::Button(name_buf))ty_s = lam->def_cubism_cg.cubism_ty_s;
    ImGui::SameLine(); ImGui::Text(TT_286);

    ImGui::DragFloat("###DragFloat___ShowShinobuInteractively_tx_p", &tx_p, 0.005f, -100.f, 100.f, "%.2f", ImGuiSliderFlags_None);//水平透视
    ImGui::SameLine(); Su::JoinGuiMark(name_buf, sizeof(name_buf), TT_293, "tx_p");
    if (ImGui::Button(name_buf))tx_p = lam->def_cubism_cg.cubism_tx_p;
    ImGui::SameLine(); ImGui::Text(TT_287);

    ImGui::DragFloat("###DragFloat___ShowShinobuInteractively_ty_p", &ty_p, 0.005f, -100.f, 100.f, "%.2f", ImGuiSliderFlags_None);//垂直透视
    ImGui::SameLine(); Su::JoinGuiMark(name_buf, sizeof(name_buf), TT_293, "ty_p");
    if (ImGui::Button(name_buf))ty_p = lam->def_cubism_cg.cubism_ty_p;
    ImGui::SameLine(); ImGui::Text(TT_288);

    Su::JoinGuiMark(name_buf, sizeof(name_buf), TT_8, "___GeometrySaveButton");
    if (ImGui::Button(name_buf, ImVec2(GlobalTemp::GuiButtonWidth, 0))) {
        //Su::UserConfigSave(&uc);
    }
    ImGui::SameLine();
    Su::JoinGuiMark(name_buf, sizeof(name_buf), TT_294, "___GeometrySetNullButton");
    if (ImGui::Button(name_buf, ImVec2(GlobalTemp::GuiButtonWidth, 0))) {
        uc.cubism_config.cubism_cg = lam->def_cubism_cg;
    }
}
void ShowShinobuCsmLook(Su::UserConfig& uc, LAppModel* lam)
{
    static char name_buf[DEFSIZE];
    static char input_buf[DEFSIZE16];
    ImGui::SeparatorText(TT_295);
    ImGui::Checkbox(TT_296, &uc.cubism_config.enable_look_mouse);
    lam->canLookMouse = uc.cubism_config.enable_look_mouse;
    ImGui::Separator();
    ImGui::SliderFloat(TT_297, &uc.cubism_config.damping, 0.01f, 2.0f, "%.2f", ImGuiSliderFlags_None);
    lam->GetLookTargetDamping() = uc.cubism_config.damping;
    ImGui::SameLine(); Su::HelpMarker(TT_298);
    if (ImGui::TreeNode(GETGUIID(TT_299))) {
        ImGui::InputText(TT_422, lam->filter_param_buf, sizeof(lam->filter_param_buf), ImGuiInputTextFlags_None);
        static ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoSavedSettings;
        if (ImGui::BeginTable("###Table___ShowShinobuInteractively_LookingMouse", 5, flags, ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 5)))
        {
            ImGui::TableSetupColumn(TT_300, ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn(TT_380, ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn(TT_301, ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn(TT_302, ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn(TT_303, ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();
            for (int paramid = 0; paramid < lam->GetModel()->GetParameterCount(); paramid++) {
                LookParam& cubism_lp = lam->GetLookTargetParams()[paramid];
                LookParam& user_lp = uc.cubism_config.look_target_params[paramid];
                strcpy_s(input_buf, sizeof(input_buf), lam->GetModel()->GetParameterId(paramid)->GetString().GetRawString());
                if (!Su::IsSubstring(input_buf, lam->filter_param_buf))continue;
                ImGui::PushID(paramid);
                ImGui::TableNextColumn();
                ImGui::PushItemWidth(-1);
                ImGui::InputText("###InputText___ShowShinobuCsmLook_Id", input_buf, sizeof(input_buf), ImGuiInputTextFlags_ReadOnly);
                ImGui::TableNextColumn();
                ImGui::PushItemWidth(-1);
                if (lam->cdi_exist)
                    strcpy_s(input_buf, sizeof(input_buf), lam->cdi_json->GetParametersName(paramid));
                else 
                    strcpy_s(input_buf, sizeof(input_buf), TT_322);
                ImGui::InputText("###InputText___ShowShinobuCsmLook_Exp", input_buf, sizeof(input_buf), ImGuiInputTextFlags_ReadOnly);
                ImGui::TableNextColumn();
                ImGui::Checkbox(TT_304, &user_lp.enable);
                cubism_lp.enable = user_lp.enable;
                ImGui::TableNextColumn();
                ImGui::DragFloat(TT_305, &user_lp.param, 0.005f, 0.f, 100.f, "%.2f", ImGuiSliderFlags_None);
                cubism_lp.param = user_lp.param;
                ImGui::TableNextColumn();
                if (ImGui::BeginCombo(TT_306, XYZAXIS[user_lp.xyzpos])) {
                    for (int n = 0; n < IM_ARRAYSIZE(XYZAXIS); n++) {
                        const bool is_selected = (user_lp.xyzpos == n);
                        if (ImGui::Selectable(XYZAXIS[n], is_selected)) {
                            user_lp.xyzpos = n;
                            cubism_lp.xyzpos = user_lp.xyzpos;
                            std::cout << user_lp.cid->GetString().GetRawString() << u8" 更改为 " << XYZAXIS[n] << endl;
                        }
                        if (is_selected)ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                ImGui::PopID();
            }
            ImGui::EndTable();
        }
        ImGui::TreePop();
    }

    Su::JoinGuiMark(name_buf, sizeof(name_buf), TT_8, "___LookingMouseSaveButton");
    if (ImGui::Button(name_buf, ImVec2(GlobalTemp::GuiButtonWidth, 0))) {
        //Su::UserConfigSave(&uc);
    }
    ImGui::SameLine();
    Su::JoinGuiMark(name_buf, sizeof(name_buf), TT_294, "___LookingMouseSetNullButton");
    if (ImGui::Button(name_buf, ImVec2(GlobalTemp::GuiButtonWidth, 0))) {
        uc.cubism_config.enable_look_mouse = true;
        uc.cubism_config.damping = 0.15f;
        uc.cubism_config.look_target_params = lam->def_look_target_params;
        lam->canLookMouse = uc.cubism_config.enable_look_mouse;
        lam->GetLookTargetDamping() = uc.cubism_config.damping;
        lam->GetLookTargetParams() = uc.cubism_config.look_target_params;
    }
}
void ShowShinobuCsmBreath(Su::UserConfig& uc, LAppModel* lam)
{
    static char name_buf[DEFSIZE];
    static char input_buf[DEFSIZE16];
    ImGui::SeparatorText(TT_307);
    ImGui::Checkbox(TT_308, &uc.cubism_config.enable_breath);
    lam->GetCanBreath() = uc.cubism_config.enable_breath;
    ImGui::Separator();
    if (ImGui::TreeNode(GETGUIID(TT_309))) {
        ImGui::InputText(TT_422, lam->filter_param_buf, sizeof(lam->filter_param_buf), ImGuiInputTextFlags_None);
        static ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoSavedSettings;
        if (ImGui::BeginTable("###Table___ShowShinobuInteractively_Breath", 7, flags, ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 5)))
        {
            ImGui::TableSetupColumn(TT_300, ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn(TT_380, ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn(TT_301, ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn(TT_310, ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn(TT_311, ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn(TT_312, ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn(TT_313, ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();
            for (int paramid = 0; paramid < lam->GetModel()->GetParameterCount(); paramid++) {
                Csm::CubismBreath::BreathParameterData& cubism_cb = lam->GetBreathParameters()[paramid];
                Csm::CubismBreath::BreathParameterData& user_cb = uc.cubism_config.breath_params[paramid];
                strcpy_s(input_buf, sizeof(input_buf), lam->GetModel()->GetParameterId(paramid)->GetString().GetRawString());
                if (!Su::IsSubstring(input_buf, lam->filter_param_buf))continue;
                ImGui::PushID(paramid);
                ImGui::TableNextColumn();
                ImGui::PushItemWidth(-1);
                ImGui::InputText("###InputText___ShowShinobuCsmBreath_Id", input_buf, sizeof(input_buf), ImGuiInputTextFlags_ReadOnly);
                ImGui::TableNextColumn();
                ImGui::PushItemWidth(-1);
                if (lam->cdi_exist)
                    strcpy_s(input_buf, sizeof(input_buf), lam->cdi_json->GetParametersName(paramid));
                else
                    strcpy_s(input_buf, sizeof(input_buf), TT_322);
                ImGui::InputText("###InputText___ShowShinobuCsmBreath_Exp", input_buf, sizeof(input_buf), ImGuiInputTextFlags_ReadOnly);
                ImGui::TableNextColumn();
                ImGui::Checkbox(TT_304, &user_cb.Enable);
                cubism_cb.Enable = user_cb.Enable;
                ImGui::TableNextColumn();
                ImGui::DragFloat(TT_314, &user_cb.Offset, 0.005f, 0.f, 100.f, "%.2f", ImGuiSliderFlags_None);
                cubism_cb.Offset = user_cb.Offset;
                ImGui::TableNextColumn();
                ImGui::DragFloat(TT_315, &user_cb.Peak, 0.005f, 0.f, 100.f, "%.2f", ImGuiSliderFlags_None);
                cubism_cb.Peak = user_cb.Peak;
                ImGui::TableNextColumn();
                ImGui::DragFloat(TT_316, &user_cb.Cycle, 0.005f, 0.f, 100.f, "%.2f", ImGuiSliderFlags_None);
                cubism_cb.Cycle = user_cb.Cycle;
                ImGui::TableNextColumn();
                ImGui::DragFloat(TT_317, &user_cb.Weight, 0.005f, 0.f, 100.f, "%.2f", ImGuiSliderFlags_None);
                cubism_cb.Weight = user_cb.Weight;
                ImGui::PopID();
            }
            ImGui::EndTable();
        }
        ImGui::TreePop();
    }
    Su::JoinGuiMark(name_buf, sizeof(name_buf), TT_8, "___BreathSaveButton");
    if (ImGui::Button(name_buf, ImVec2(GlobalTemp::GuiButtonWidth, 0))) {
        //Su::UserConfigSave(&uc);
    }
    ImGui::SameLine();
    Su::JoinGuiMark(name_buf, sizeof(name_buf), TT_294, "___BreathSetNullButton");
    if (ImGui::Button(name_buf, ImVec2(GlobalTemp::GuiButtonWidth, 0))) {
        uc.cubism_config.enable_breath = true;
        uc.cubism_config.breath_params = lam->def_breath_params;
        lam->GetCanBreath() = uc.cubism_config.enable_breath;
        lam->GetBreathParameters().Clear();
        for (const Csm::CubismBreath::BreathParameterData& bpd : lam->def_breath_params)
            lam->GetBreathParameters().PushBack(bpd);
    }
}
void ShowShinobuCsmBlink(Su::UserConfig& uc, LAppModel* lam)
{
    static char name_buf[DEFSIZE];
    static char input_buf[DEFSIZE16];
    ImGui::SeparatorText(TT_319);
    ImGui::Checkbox(TT_320, &uc.cubism_config.enable_blink);
    lam->canEyeBlink = uc.cubism_config.enable_blink;
    ImGui::SameLine();

    static std::string blinkIdsHelp;
    blinkIdsHelp.clear();
    blinkIdsHelp = TT_321;
    for (int ieb = 0; ieb < int(lam->GetEyeBlinkIds().GetSize()); ++ieb) {
        blinkIdsHelp += lam->GetEyeBlinkIds()[ieb]->GetString().GetRawString() + std::string("\n");
    }
    if (lam->GetEyeBlinkIds().GetSize() == 0)
        blinkIdsHelp += TT_322;
    Su::HelpMarker(blinkIdsHelp.c_str());
    ImGui::Separator();
    ImGui::SliderFloat(TT_323, &uc.cubism_config.blinking_interval_seconds, 0.01f, 10.0f, "%.2f", ImGuiSliderFlags_None);   //眨眼的时间间隔
    lam->GetBlinkingIntervalSeconds() = uc.cubism_config.blinking_interval_seconds;
    ImGui::SliderFloat(TT_324, &uc.cubism_config.closing_seconds, 0.01f, 2.0f, "%.2f", ImGuiSliderFlags_None);              //闭眼所需的时间
    lam->GetClosingSeconds() = uc.cubism_config.closing_seconds;
    ImGui::SliderFloat(TT_325, &uc.cubism_config.closed_seconds, 0.01f, 2.0f, "%.2f", ImGuiSliderFlags_None);               //完全闭合状态的持续时间
    lam->GetClosedSeconds() = uc.cubism_config.closed_seconds;
    ImGui::SliderFloat(TT_326, &uc.cubism_config.opening_seconds, 0.01f, 2.0f, "%.2f", ImGuiSliderFlags_None);              //闭合到完全睁开的过渡时间
    lam->GetOpeningSeconds() = uc.cubism_config.opening_seconds;
    static const char* blinklist[DEFSIZEK16];
    for (int paramid = 0; paramid < lam->GetModel()->GetParameterCount(); paramid++) {
        blinklist[paramid] = lam->GetModel()->GetParameterId(paramid)->GetString().GetRawString();
    }
    if (ImGui::TreeNode(GETGUIID(TT_327)))
    {
        Su::ShinobuExList& sel = uc.cubism_config.blink_sel_list;
        const char** ex_name = blinklist;
        if (sel.Show(ex_name)) {
            Csm::csmVector<Csm::CubismIdHandle>& leb = lam->GetEyeBlinkIds();
            leb.Clear();
            for (auto& ex_id : sel.Items[1]) {
                leb.PushBack(lam->GetModel()->GetParameterId(ex_id));
            }
        }
        ImGui::TreePop();
    }
    Su::JoinGuiMark(name_buf, sizeof(name_buf), TT_8, "___BlinkSaveButton");
    if (ImGui::Button(name_buf, ImVec2(GlobalTemp::GuiButtonWidth, 0))) {
        //Su::UserConfigSave(&uc);
    }
    ImGui::SameLine();
    Su::JoinGuiMark(name_buf, sizeof(name_buf), TT_294, "___BlinkSetNullButton");
    if (ImGui::Button(name_buf, ImVec2(GlobalTemp::GuiButtonWidth, 0))) {
        uc.cubism_config.enable_blink = true;
        uc.cubism_config.blinking_interval_seconds = 4.0f;
        uc.cubism_config.closing_seconds = 0.10f;
        uc.cubism_config.closed_seconds = 0.05f;
        uc.cubism_config.opening_seconds = 0.15f;
        uc.cubism_config.blink_sel_list.Items[0].clear();
        uc.cubism_config.blink_sel_list.Items[1].clear();
        for (const ImGuiID& pid :lam->def_blink_list_ids[1]) 
            uc.cubism_config.blink_sel_list.Items[1].push_back(pid);
        for (const ImGuiID& pid : lam->def_blink_list_ids[0])
            uc.cubism_config.blink_sel_list.Items[0].push_back(pid);
        lam->canEyeBlink = uc.cubism_config.enable_blink;
        lam->GetBlinkingIntervalSeconds() = uc.cubism_config.blinking_interval_seconds;
        lam->GetClosingSeconds() = uc.cubism_config.closing_seconds;
        lam->GetClosedSeconds() = uc.cubism_config.closed_seconds;
        lam->GetOpeningSeconds() = uc.cubism_config.opening_seconds;
        lam->GetEyeBlinkIds().Clear();
        for (const ImGuiID& pid : lam->def_blink_list_ids[1]) {
            std::string param_str = lam->GetModel()->GetParameterId(pid)->GetString().GetRawString();
            lam->GetEyeBlinkIds().PushBack(Csm::CubismFramework::GetIdManager()->GetId(param_str.c_str()));
        }
    }
}
void ShowShinobuCsmAnimation(Su::UserConfig& uc, LAppModel* lam)
{
    static char name_buf[DEFSIZE];
    static char input_buf[DEFSIZE16];
    Csm::ICubismModelSetting* lcms = lam->GetModelSetting();
    ImGui::SeparatorText(TT_290);
    ImGui::Checkbox(TT_359, &uc.cubism_config.enable_anim_autoplay);
    lam->animationAutoPlay = uc.cubism_config.enable_anim_autoplay;
    Su::JoinGuiMark(name_buf, sizeof(name_buf), TT_330, "___RandomMotionButton");
    if (ImGui::Button(name_buf)) {
        int  mg_count = lam->GetModelSetting()->GetMotionGroupCount();
        if (mg_count > 0) {
            int gno = rand() % mg_count;
            int m_count = lam->GetModelSetting()->GetMotionCount(lam->GetModelSetting()->GetMotionGroupName(gno));
            if (m_count != 0) {
                int no = rand() % m_count;
                AddMotionStart(uc.user_id, gno, no);
            }
        }
    }
    ImGui::SameLine();
    Su::JoinGuiMark(name_buf, sizeof(name_buf), TT_331, "___RandomExpressionButton");
    if (ImGui::Button(name_buf)) {
        if (lam->GetModelSetting()->GetExpressionCount() > 0) {
            int no = rand() % lam->GetModelSetting()->GetExpressionCount();
            AddExpressionStart(uc.user_id, no);
        }
    }
    ImGui::Separator();
    strcpy_s(input_buf, sizeof(input_buf), TT_374);
    strcat_s(input_buf, sizeof(input_buf), lam->current_motion_name.c_str());
    ImGui::Text(input_buf);
    strcpy_s(input_buf, sizeof(input_buf), TT_375);
    strcat_s(input_buf, sizeof(input_buf), lam->current_expression_name.c_str());
    ImGui::Text(input_buf);
    if (ImGui::TreeNode(GETGUIID(TT_328))) {
        for (int i = 0; i < lam->GetModelSetting()->GetMotionGroupCount(); ++i) {
            strcpy_s(input_buf, sizeof(input_buf), TT_292);
            strcat_s(input_buf, sizeof(input_buf), lam->GetModelSetting()->GetMotionGroupName(i));
            ImGui::PushID(i);
            ImGui::Text(input_buf);
            for (int j = 0; j < lcms->GetMotionCount(lam->GetModelSetting()->GetMotionGroupName(i)); ++j) {
                ImGui::PushID(j);
                strcpy_s(input_buf, sizeof(input_buf), lcms->GetMotionFileName(lcms->GetMotionGroupName(i), j));
                ImGui::InputText("", input_buf, sizeof(input_buf), ImGuiInputTextFlags_ReadOnly);
                ImGui::SameLine();
                if (ImGui::Button(TT_289)) {
                    AddMotionStart(uc.user_id, i, j);
                }
                ImGui::PopID();
            }
            ImGui::PopID();
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNode(GETGUIID(TT_329))) {
        for (int i = 0; i < lam->GetModelSetting()->GetExpressionCount(); ++i) {
            ImGui::PushID(i);
            strcpy_s(input_buf, sizeof(input_buf), lcms->GetExpressionName(i));
            ImGui::InputText("", input_buf, sizeof(input_buf), ImGuiInputTextFlags_ReadOnly);
            ImGui::SameLine();
            if (ImGui::Button(TT_289)) {
                AddExpressionStart(uc.user_id, i);
            }
            ImGui::PopID();
        }
        ImGui::TreePop();
    }
    Su::JoinGuiMark(name_buf, sizeof(name_buf), TT_8, "___AnimationSaveButton");
    if (ImGui::Button(name_buf, ImVec2(GlobalTemp::GuiButtonWidth, 0))) {
        //Su::UserConfigSave(&uc);
    }
    ImGui::SameLine();
    Su::JoinGuiMark(name_buf, sizeof(name_buf), TT_294, "___AnimationSetNullButton");
    if (ImGui::Button(name_buf, ImVec2(GlobalTemp::GuiButtonWidth, 0))) {
        uc.cubism_config.enable_anim_autoplay = false;
        lam->animationAutoPlay = uc.cubism_config.enable_anim_autoplay;
    }
    
}
void ShowShinobuCsmHitareas(Su::UserConfig& uc, LAppModel* lam)
{
    static char name_buf[DEFSIZE];
    static char input_buf[DEFSIZE16];
    Csm::ICubismModelSetting* lcms = lam->GetModelSetting();
    ImGui::SeparatorText(TT_349);
    ImGui::Checkbox(TT_373, &uc.cubism_config.enable_hitareas);
    lam->canHitareas = uc.cubism_config.enable_hitareas;
    ImGui::SameLine();
    ImGui::Checkbox(TT_350, &uc.cubism_config.enable_preview_hitareas);
    lam->previewHitareas = uc.cubism_config.enable_preview_hitareas;
    ImGui::Separator();
    strcpy_s(input_buf, sizeof(input_buf), TT_351);
    strcat_s(input_buf, sizeof(input_buf), lam->hit_id.c_str());
    ImGui::Text(input_buf);
    strcpy_s(input_buf, sizeof(input_buf), TT_352);
    strcat_s(input_buf, sizeof(input_buf), lam->hit_name.c_str());
    ImGui::Text(input_buf);
    strcpy_s(input_buf, sizeof(input_buf), TT_353);
    strcat_s(input_buf, sizeof(input_buf), std::to_string(lam->hit_num).c_str());
    ImGui::Text(input_buf);
    strcpy_s(input_buf, sizeof(input_buf), TT_354);
    strcat_s(input_buf, sizeof(input_buf), lam->hit_motion_name.c_str());
    ImGui::Text(input_buf);
    strcpy_s(input_buf, sizeof(input_buf), TT_355);
    strcat_s(input_buf, sizeof(input_buf), lam->hit_expression_name.c_str());
    ImGui::Text(input_buf);
    ImGui::Separator();
    Su::JoinGuiMark(name_buf, sizeof(name_buf), TT_8, "___HitareasSaveButton");
    if (ImGui::Button(name_buf, ImVec2(GlobalTemp::GuiButtonWidth, 0))) {
        //Su::UserConfigSave(&uc);
    }
    ImGui::SameLine();
    Su::JoinGuiMark(name_buf, sizeof(name_buf), TT_294, "___HitareasSetNullButton");
    if (ImGui::Button(name_buf, ImVec2(GlobalTemp::GuiButtonWidth, 0))) {
        uc.cubism_config.enable_hitareas = true;
        uc.cubism_config.enable_preview_hitareas = false;
        lam->canHitareas = uc.cubism_config.enable_hitareas;
        lam->previewHitareas = uc.cubism_config.enable_preview_hitareas;

    }
    ImGui::BeginGroup(); // G1
    int& selected = lam->hit_ui_selected;
    ImGui::BeginChild("###BeginChild___ShowShinobuInteractively_L", ImVec2(150, 400), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX); // C1
    for (int i = 0; i < lcms->GetHitAreasCount(); i++)
    {
        Csm::CubismIdHandle had = lcms->GetHitAreaId(i);
        sprintf_s(input_buf, sizeof(input_buf), had->GetString().GetRawString());
        if (ImGui::Selectable(input_buf, selected == i))
            selected = i;
    }
    ImGui::EndChild(); // C1
    ImGui::SameLine();
    ImGui::BeginChild("###BeginChild__ShowShinobuInteractively_R", ImVec2(0, 400), ImGuiChildFlags_Borders); // C2
    if (selected != -1) {

        Csm::CubismIdHandle had = lcms->GetHitAreaId(selected);
        const Csm::csmChar* han = lcms->GetHitAreaName(selected);
        strcpy_s(input_buf, sizeof(input_buf), TT_356);
        strcat_s(input_buf, sizeof(input_buf), han);
        ImGui::Text(input_buf);
        ImGui::Separator();
        static const char* anim_list[DEFSIZEK16];
        const char** ex_name = anim_list;
        if (ImGui::BeginTabBar("###BeginTabBar___ShowShinobuInteractively_Anim", ImGuiTabBarFlags_None)) {
            if (ImGui::BeginTabItem(TT_357)) {
                int m_pos = 0;
                for (int i = 0; i < lam->GetModelSetting()->GetMotionGroupCount(); ++i) {
                    for (int j = 0; j < lcms->GetMotionCount(lam->GetModelSetting()->GetMotionGroupName(i)); ++j) {
                        anim_list[m_pos] = lcms->GetMotionFileName(lcms->GetMotionGroupName(i), j);
                        m_pos++;
                    }
                }
                Su::ShinobuExList& sel = uc.cubism_config.hit_areas_motion_map[lcms->GetHitAreaId(selected)->GetString().GetRawString()];
                if (sel.Show(ex_name)) {
                    lam->hit_areas_motion_map[lcms->GetHitAreaId(selected)->GetString().GetRawString()] = sel;
                }
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem(TT_358)) {

                for (int i = 0; i < lam->GetModelSetting()->GetExpressionCount(); ++i) {
                    anim_list[i] = lcms->GetExpressionName(i);
                }
                Su::ShinobuExList& sel = uc.cubism_config.hit_areas_expression_map[lcms->GetHitAreaId(selected)->GetString().GetRawString()];
                if (sel.Show(ex_name)) {
                    lam->hit_areas_expression_map[lcms->GetHitAreaId(selected)->GetString().GetRawString()] = sel;
                }
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
    }
    ImGui::EndChild(); // C2
    ImGui::EndGroup(); // G1
}
void ShowShinobuCsmRendering(Su::UserConfig& uc, LAppModel* lam)
{
    static char name_buf[DEFSIZE];
    static char edit_name[DEFSIZE];
    ImGui::SeparatorText(TT_332);
    ImGui::Checkbox(TT_333, (bool*)(&lam->GetModel()->GetOverwriteFlagForModelMultiplyColorsRef()));// 正片叠底色覆盖标志
    ImGui::SameLine();
    ImGui::Checkbox(TT_334, (bool*)(&lam->GetModel()->GetOverwriteFlagForModelScreenColorsRef()));  // 屏幕色覆盖标志
    ImGui::SameLine();
    ImGui::Checkbox(TT_360, (bool*)(&lam->GetModel()->GetOverwriteFlagForModelCullingsRef()));      // 剔除规则
    ImGui::Separator();
    Su::JoinGuiMark(name_buf, sizeof(name_buf), TT_8, "___ColorsSaveButton");
    if (ImGui::Button(name_buf, ImVec2(GlobalTemp::GuiButtonWidth, 0))) {
        //Su::UserConfigSave(&uc);
    }
    ImGui::SameLine();
    Su::JoinGuiMark(name_buf, sizeof(name_buf), TT_294, "___ColorsSetNullButton");
    if (ImGui::Button(name_buf, ImVec2(GlobalTemp::GuiButtonWidth, 0))) {
        lam->GetModel()->GetOverwriteFlagForModelMultiplyColorsRef() = false;
        lam->GetModel()->GetOverwriteFlagForModelScreenColorsRef() = false;
        lam->GetModel()->GetOverwriteFlagForModelCullingsRef() = false;
        lam->InitMultiplyColor();
        lam->InitScreenColor();
        lam->InitPartMultiplyColor();
        lam->InitPartScreenColor();
        lam->InitPartOpacity();
        for (int i = 0; i < lam->GetModel()->GetPartCount(); i++) {
            lam->GetModel()->GetPartMultiplyColorsRef()[i].IsOverwritten = false;
            lam->GetModel()->GetPartScreenColorsRef()[i].IsOverwritten = false;
        }
    }
    if (ImGui::BeginTabBar("###BeginTabBar___ShowShinobuInteractively_Rendering", ImGuiTabBarFlags_None)) {
        //------------------------------------------------正片叠底
        if (ImGui::BeginTabItem(GETGUIID(TT_335))) {
            ImGui::SeparatorText(TT_336);
            if (ImGui::Button(TT_50, ImVec2(GlobalTemp::GuiButtonWidth, 0))) {
                lam->InitMultiplyColor();
            }
            Su::JoinGuiMark(edit_name, sizeof(edit_name), TT_362, u8"___Multiply_Synchronize_ColorEdit4");
            ImGui::ColorEdit4(edit_name, (float*)&(lam->multiply_color), ImGuiColorEditFlags_AlphaBar);
            ImGui::SameLine();
            ImGui::Checkbox(TT_363, &lam->set_all_m_mark);
            if (lam->set_all_m_mark) {
                for (int i = 0; i < lam->GetModel()->GetDrawableCount(); i++)
                {
                    lam->drawable_multiply_color[i][0] = lam->multiply_color[0];
                    lam->drawable_multiply_color[i][1] = lam->multiply_color[1];
                    lam->drawable_multiply_color[i][2] = lam->multiply_color[2];
                    lam->drawable_multiply_color[i][3] = lam->multiply_color[3];
                }
            }
            ImGui::Separator();
            ImGui::InputText(TT_423, lam->filter_drawable_buf, sizeof(lam->filter_drawable_buf), ImGuiInputTextFlags_None);
            for (int i = 0; i < lam->GetModel()->GetDrawableCount(); i++)
            {
                if (!Su::IsSubstring(lam->GetModel()->GetDrawableId(i)->GetString().GetRawString(), lam->filter_drawable_buf))continue;
                ImGui::PushID(i);
                if (ImGui::Button("?"))
                    lam->StartFlashColor(0, i);
                ImGui::SetItemTooltip(TT_116);
                ImGui::SameLine();
                Su::JoinGuiMark(edit_name, sizeof(edit_name), lam->GetModel()->GetDrawableId(i)->GetString().GetRawString(), std::to_string(i).c_str());
                ImGui::ColorEdit4(edit_name, (float*)&(lam->drawable_multiply_color[i]), ImGuiColorEditFlags_AlphaBar);
                ImGui::PopID();
            }
            ImGui::EndTabItem();
        }
        //------------------------------------------------屏幕色
        if (ImGui::BeginTabItem(GETGUIID(TT_337))) {
            ImGui::SeparatorText(TT_338);
            if (ImGui::Button(TT_50, ImVec2(GlobalTemp::GuiButtonWidth, 0))) {
                lam->InitScreenColor();
            }
            Su::JoinGuiMark(edit_name, sizeof(edit_name), TT_362, u8"___Screen_Synchronize_ColorEdit4");
            ImGui::ColorEdit4(edit_name, (float*)&(lam->screen_color), ImGuiColorEditFlags_AlphaBar);
            ImGui::SameLine();
            ImGui::Checkbox(TT_363, &lam->set_all_s_mark);
            if (lam->set_all_s_mark) {
                for (int i = 0; i < lam->GetModel()->GetDrawableCount(); i++)
                {
                    lam->drawable_screen_color[i][0] = lam->screen_color[0];
                    lam->drawable_screen_color[i][1] = lam->screen_color[1];
                    lam->drawable_screen_color[i][2] = lam->screen_color[2];
                    lam->drawable_screen_color[i][3] = lam->screen_color[3];
                }
            }
            ImGui::Separator();
            ImGui::InputText(TT_423, lam->filter_drawable_buf, sizeof(lam->filter_drawable_buf), ImGuiInputTextFlags_None);
            for (int i = 0; i < lam->GetModel()->GetDrawableCount(); i++)
            {
                if (!Su::IsSubstring(lam->GetModel()->GetDrawableId(i)->GetString().GetRawString(), lam->filter_drawable_buf))continue;
                ImGui::PushID(i);
                if (ImGui::Button("?"))
                    lam->StartFlashColor(1, i);
                ImGui::SetItemTooltip(TT_116);
                ImGui::SameLine();
                Su::JoinGuiMark(edit_name, sizeof(edit_name), lam->GetModel()->GetDrawableId(i)->GetString().GetRawString(), std::to_string(i).c_str());
                ImGui::ColorEdit4(edit_name, (float*)&(lam->drawable_screen_color[i]), ImGuiColorEditFlags_AlphaBar);
                ImGui::PopID();
            }
            ImGui::EndTabItem();
        }
        //------------------------------------------------正片叠底组
        if (ImGui::BeginTabItem(GETGUIID(TT_339))) {
            ImGui::SeparatorText(TT_340);
            if (ImGui::Button(TT_341, ImVec2(GlobalTemp::GuiButtonWidth, 0))) {
                for (int i = 0; i < lam->GetModel()->GetPartCount(); i++) {
                    lam->GetModel()->GetPartMultiplyColorsRef()[i].IsOverwritten = true;
                }
            }
            ImGui::SameLine();
            if (ImGui::Button(TT_361, ImVec2(GlobalTemp::GuiButtonWidth, 0))) {
                for (int i = 0; i < lam->GetModel()->GetPartCount(); i++) {
                    lam->GetModel()->GetPartMultiplyColorsRef()[i].IsOverwritten = false;
                }
            }
            ImGui::SameLine();
            if (ImGui::Button(TT_50, ImVec2(GlobalTemp::GuiButtonWidth, 0))) {
                lam->InitPartMultiplyColor();
            }
            Su::JoinGuiMark(edit_name, sizeof(edit_name), TT_362, u8"___MultiplyPart_Synchronize_ColorEdit4");
            ImGui::ColorEdit4(edit_name, (float*)&(lam->multiply_group_color), ImGuiColorEditFlags_AlphaBar);
            ImGui::SameLine();
            ImGui::Checkbox(TT_363, &lam->set_all_mp_mark);
            if (lam->set_all_mp_mark) {
                for (int i = 0; i < lam->GetModel()->GetPartCount(); i++)
                {
                    lam->drawable_part_multiply_color[i][0] = lam->multiply_group_color[0];
                    lam->drawable_part_multiply_color[i][1] = lam->multiply_group_color[1];
                    lam->drawable_part_multiply_color[i][2] = lam->multiply_group_color[2];
                    lam->drawable_part_multiply_color[i][3] = lam->multiply_group_color[3];
                }
            }
            ImGui::Separator();
            ImGui::InputText(TT_424, lam->filter_part_buf, sizeof(lam->filter_part_buf), ImGuiInputTextFlags_None);
            for (int i = 0; i < lam->GetModel()->GetPartCount(); i++)
            {
                if (!Su::IsSubstring(lam->GetModel()->GetPartId(i)->GetString().GetRawString(), lam->filter_part_buf))continue;
                ImGui::PushID(i);
                if (ImGui::Button("?"))
                    lam->StartFlashColor(2, i);
                ImGui::SetItemTooltip(TT_116);
                ImGui::SameLine();
                ImGui::Checkbox(TT_342, &lam->GetModel()->GetPartMultiplyColorsRef()[i].IsOverwritten);
                ImGui::SameLine();
                Su::JoinGuiMark(edit_name, sizeof(edit_name), lam->GetModel()->GetPartId(i)->GetString().GetRawString(), std::to_string(i).c_str());
                ImGui::ColorEdit4(edit_name, (float*)&(lam->drawable_part_multiply_color[i]), ImGuiColorEditFlags_AlphaBar);
                if (lam->cdi_exist) {
                    ImGui::SameLine();
                    sprintf_s(edit_name, sizeof(edit_name), "%s", lam->cdi_json->GetPartsName(i));
                    Su::HelpMarker(edit_name);
                }
                ImGui::PopID();
            }
            ImGui::EndTabItem();
        }
        //------------------------------------------------屏幕色组
        if (ImGui::BeginTabItem(GETGUIID(TT_343))) {
            ImGui::SeparatorText(TT_344);
            if (ImGui::Button(TT_341, ImVec2(GlobalTemp::GuiButtonWidth, 0))) {
                for (int i = 0; i < lam->GetModel()->GetPartCount(); i++) {
                    lam->GetModel()->GetPartScreenColorsRef()[i].IsOverwritten = true;
                }
            }
            ImGui::SameLine();
            if (ImGui::Button(TT_361, ImVec2(GlobalTemp::GuiButtonWidth, 0))) {
                for (int i = 0; i < lam->GetModel()->GetPartCount(); i++) {
                    lam->GetModel()->GetPartScreenColorsRef()[i].IsOverwritten = false;
                }
            }
            ImGui::SameLine();
            if (ImGui::Button(TT_50, ImVec2(GlobalTemp::GuiButtonWidth, 0))) {
                lam->InitPartScreenColor();
            }
            Su::JoinGuiMark(edit_name, sizeof(edit_name), TT_362, u8"___ScreenPart_Synchronize_ColorEdit4");
            ImGui::ColorEdit4(edit_name, (float*)&(lam->screen_group_color), ImGuiColorEditFlags_AlphaBar);
            ImGui::SameLine();
            ImGui::Checkbox(TT_363, &lam->set_all_sp_mark);
            if (lam->set_all_sp_mark) {
                for (int i = 0; i < lam->GetModel()->GetPartCount(); i++)
                {
                    lam->drawable_part_screen_color[i][0] = lam->screen_group_color[0];
                    lam->drawable_part_screen_color[i][1] = lam->screen_group_color[1];
                    lam->drawable_part_screen_color[i][2] = lam->screen_group_color[2];
                    lam->drawable_part_screen_color[i][3] = lam->screen_group_color[3];
                }
            }
            ImGui::Separator();
            ImGui::InputText(TT_424, lam->filter_part_buf, sizeof(lam->filter_part_buf), ImGuiInputTextFlags_None);
            for (int i = 0; i < lam->GetModel()->GetPartCount(); i++)
            {
                if (!Su::IsSubstring(lam->GetModel()->GetPartId(i)->GetString().GetRawString(), lam->filter_part_buf))continue;
                ImGui::PushID(i);
                if (ImGui::Button("?"))
                    lam->StartFlashColor(3, i);
                ImGui::SetItemTooltip(TT_116);
                ImGui::SameLine();
                ImGui::Checkbox(TT_342, &lam->GetModel()->GetPartScreenColorsRef()[i].IsOverwritten);
                ImGui::SameLine();
                Su::JoinGuiMark(edit_name, sizeof(edit_name), lam->GetModel()->GetPartId(i)->GetString().GetRawString(), std::to_string(i).c_str());
                ImGui::ColorEdit4(edit_name, (float*)&(lam->drawable_part_screen_color[i]), ImGuiColorEditFlags_AlphaBar);
                if (lam->cdi_exist) {
                    ImGui::SameLine();
                    sprintf_s(edit_name, sizeof(edit_name), "%s", lam->cdi_json->GetPartsName(i));
                    Su::HelpMarker(edit_name);
                }
                ImGui::PopID();
            }
            ImGui::EndTabItem();
        }
        //控件渲染
        if (ImGui::BeginTabItem(GETGUIID(TT_381))) {
            ImGui::SeparatorText(TT_382);
            if (ImGui::Button(TT_50, ImVec2(GlobalTemp::GuiButtonWidth, 0))) {
                lam->InitPartOpacity();
            }
            Su::JoinGuiMark(edit_name, sizeof(edit_name),TT_383, u8"___PartOpacity_Synchronize_ColorEdit4");
            if (ImGui::DragFloat(edit_name, &lam->group_opacity, 0.005f, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_None)) {
                lam->group_opacity = Su::Clamp(lam->group_opacity, 0.0f, 1.0f);
            }
            ImGui::SameLine();
            ImGui::Checkbox(TT_363, &lam->set_all_po_mark);
            if (lam->set_all_po_mark) {
                for (int i = 0; i < lam->GetModel()->GetPartCount(); i++)
                    lam->drawable_part_opacity[i] = lam->group_opacity;
            }
            ImGui::Separator();
            ImGui::InputText(TT_424, lam->filter_part_buf, sizeof(lam->filter_part_buf), ImGuiInputTextFlags_None);
            for (int i = 0; i < lam->GetModel()->GetPartCount(); i++)
            {
                if (!Su::IsSubstring(lam->GetModel()->GetPartId(i)->GetString().GetRawString(), lam->filter_part_buf))continue;
                ImGui::PushID(i);
                if (ImGui::Button("?")) 
                    lam->StartFlashOpacity(i);
                ImGui::SetItemTooltip(TT_116);
                ImGui::SameLine();
                const Csm::CubismId* part_id = Csm::CubismFramework::GetIdManager()->GetId(lam->GetModel()->GetPartId(i)->GetString().GetRawString());
                if (ImGui::DragFloat("", &lam->drawable_part_opacity[i], 0.005f, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_None)) {
                    lam->drawable_part_opacity[i] = Su::Clamp(lam->drawable_part_opacity[i], 0.0f, 1.0f);
                }
                ImGui::SameLine();
                if (ImGui::Button(TT_293)) {
                    lam->drawable_part_opacity[i] = 1.0f;
                }
                ImGui::SameLine(); ImGui::Text(part_id->GetString().GetRawString());
                if (lam->cdi_exist) {
                    ImGui::SameLine();
                    sprintf_s(edit_name, sizeof(edit_name), "%s", lam->cdi_json->GetPartsName(i));
                    Su::HelpMarker(edit_name);
                }
                ImGui::PopID();
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}
void ShowShinobuCsmObserve(Su::UserConfig& uc, LAppModel* lam)
{
    if (lam->sdatal_ui_mark)
        return;
    ShowShinobuCsmObserveItems(uc, lam);
}
void ShowShinobuCsmObserveSeparate(Su::UserConfig& uc, LAppModel* lam)
{
    if (!lam->sdatal_ui_mark)
        return;
    static char name_buf[DEFSIZE];
    Su::MemsetStr(name_buf, sizeof(name_buf));
    strcpy_s(name_buf, sizeof(name_buf), GETGUIID(TT_384));
    strcat_s(name_buf, sizeof(name_buf), "_%d");
    snprintf(name_buf, sizeof(name_buf), name_buf, uc.user_id, uc.user_id);
    ImGui::Begin(GETGUIID(name_buf),&lam->sdatal_ui_mark);
    ShowShinobuCsmObserveItems(uc, lam);
    ImGui::End();
}
void ShowShinobuCsmObserveItems(Su::UserConfig& uc, LAppModel* lam)
{
    static ImPlotAxisFlags flags = ImPlotAxisFlags_None;
    ImGui::SeparatorText(TT_377);
    ImGui::SliderFloat(TT_378, &lam->history_t, 1.0f, 30.f, TT_379);
    ImGui::SameLine();
    ImGui::Checkbox(TT_385, &lam->sdatal_ui_mark);
    ImGui::Separator();
    ImGui::InputText(TT_422, lam->filter_param_buf, sizeof(lam->filter_param_buf), ImGuiInputTextFlags_None);
    Csm::CubismModel* cml = lam->GetModel();
    for (int i = 0; i < cml->GetParameterCount(); ++i) {
        Su::ShinobuScrollingBuffer& sdata1 = lam->sdatal_v[i];
        if (!Su::IsSubstring(cml->GetParameterId(i)->GetString().GetRawString(), lam->filter_param_buf))continue;
        ImGui::PushID(i);
        if (ImPlot::BeginPlot("##Scrolling___ShowShinobuCsmObserve", ImVec2(-1, 150))) {
            ImPlot::SetupAxes(nullptr, nullptr, flags, flags);
            ImPlot::SetupAxisLimits(ImAxis_X1, lam->add_t - lam->history_t, lam->add_t, ImGuiCond_Always);
            ImPlot::SetupAxisLimits(ImAxis_Y1, cml->GetParameterMinimumValue(i), cml->GetParameterMaximumValue(i));
            ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);
            ImPlot::PlotLine(cml->GetParameterId(i)->GetString().GetRawString(), &sdata1.Data[0].x, &sdata1.Data[0].y, sdata1.Data.size(), 0, sdata1.Offset, 2 * sizeof(float));
            ImPlot::EndPlot();
        }
        ImGui::PopID();
    }
}
void RemoveUserHandle(int re_userid)
{
    GlobalTemp::CubismModelMessage.push(std::pair<int, std::string>(-1, std::to_string(re_userid)));
    GlobalTemp::RefreshCubism = true;
}

void AddMotionStart(int uid, int gid, int nid)
{
    std::pair<int, std::pair<int, int>> mp;
    mp.first = uid;
    mp.second.first = gid;
    mp.second.second = nid;
    GlobalTemp::CubismMotionMessage.push(mp);
    GlobalTemp::RefreshCubism = true;
}

void AddExpressionStart(int uid, int nid)
{
    std::pair<int, int> ep;
    ep.first = uid;
    ep.second = nid;
    GlobalTemp::CubismExpressionMessage.push(ep);
    GlobalTemp::RefreshCubism = true;
}

void ShowShinobuWindow(bool* p_open) {
    if (*p_open == false) { return; }
    static char title_buf[128];
    sprintf_s(title_buf, sizeof(title_buf), GETGUIID(TT_38), "|/-\\"[(int)(ImGui::GetTime() / 0.25f) & 3], Su::getCurrentTime().c_str());
    ImGui::SetNextWindowPos(ImVec2(0,0), ImGuiCond_FirstUseEver);
    ImGui::Begin(title_buf, p_open);
    ShowShinobuHead();
    ShowShinobuStart();
    ShowShinobuUser();
    ShowShinobuLanguage();
    ShowShinobuStyle();
    ShowShinobuGlobal();
    ShowShinobuCubism();
    ShowShinobuHelp();
    ShowShinobuAbout();
    ImGui::End();

}
