﻿#include "global.h"
#include <vector>
#include <string>
#include <sstream>

#define IS_WINDOWS
#ifdef IS_WINDOWS
#include <windows.h>
#include <io.h>
#endif // IS_WINDOWS

std::ostringstream GlobalTemp:: DebugOss;
bool GlobalTemp::               ShowStyleEditor                 = false;
HWND GlobalTemp::               WindowMainHandle;
WNDCLASSEXW GlobalTemp::        WindowMainWc;
bool GlobalTemp::               WindowMainShow                  = false;
int GlobalTemp::                CubismFrameCount                = 0;
bool GlobalTemp::               RefreshTable                    = true;
bool GlobalTemp::               RefreshCubism                   = true;
bool GlobalTemp::               CubismIsRunning                 = false;
bool GlobalTemp::               TableBools[DEFSIZE][DEFSIZEK16] = {false};
float GlobalTemp::              GuiButtonWidth                  = 200.f;
bool  GlobalTemp::              WindowCubismShow                = true;
RectF GlobalTemp::              ShinobuWindowRect               = { 0.f,0.f,0.f,0.f };


const LPCWSTR GlobalTemp::CubismDirectXClassName    = L"Cubism DirectX11 Sample";
const LPCWSTR GlobalTemp::CubismWindowClassName     = L"ShinobuDiary Cubism";
std::queue<std::pair<int, std::string>>             GlobalTemp::CubismModelMessage;
std::queue<std::pair<int, std::pair<int, int>>>     GlobalTemp::CubismMotionMessage;
std::queue<std::pair<int, int>>                     GlobalTemp::CubismExpressionMessage;

void GlobalConfig::GlobalConfigInit(GlobalConfig* gc) {
    static bool initHas = false;if (initHas == true) return;initHas = true;
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    std::cout << u8"全局配置初始化" << std::endl;

    gc->select_lan = (LAN)FileSetting::GetLongValue(0, INIGROUPMARKSTR, inimark_map[::INIMARK::LANGUAGESEL_ID], INITINT);
    gc->user_num = FileSetting::GetLongValue(0, INIGROUPMARKSTR, inimark_map[::INIMARK::USER_NUM], INITINT);
    gc->select_theme_id = FileSetting::GetLongValue(0, INIGROUPMARKSTR, inimark_map[::INIMARK::THEMESEL_ID], INITINT);
    gc->defaultStyle = ImGui::GetStyle();
    FILE* fpp;
    if (fopen_s(&fpp, STYLE_WAY, "rb") == 0) {
        ImGuiStyle st;
        fread(&st, sizeof(ImGuiStyle), 1, fpp);
        ImGui::GetStyle() = st;
        fclose(fpp);
    }
    gc->window_cubism_style_id = FileSetting::GetLongValue(0, INIGROUPMARKSTR, inimark_map[::INIMARK::WINDOW_CUBISM_STYLE_ID], INITINT);
    gc->window_main_close = FileSetting::GetLongValue(0, INIGROUPMARKSTR, inimark_map[::INIMARK::WINDOW_MAIN_CLOSE], INITINT);
    gc->window_main_style_id = FileSetting::GetLongValue(0, INIGROUPMARKSTR, inimark_map[::INIMARK::WINDOW_MAIN_STYLE_ID], INITINT);
    gc->window_main_dock_id = FileSetting::GetLongValue(0, INIGROUPMARKSTR, inimark_map[::INIMARK::WINDOW_MAIN_DOCK_ID], INITINT);
    gc->window_main_transparent_id = FileSetting::GetLongValue(0, INIGROUPMARKSTR, inimark_map[::INIMARK::WINDOW_MAIN_TRANSPARENT_ID], INITINT);
    if (gc->window_main_style_id == 0)
        ::SetWindowPos(GlobalTemp::WindowMainHandle, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    else if (gc->window_main_style_id == 1)
        ::SetWindowPos(GlobalTemp::WindowMainHandle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    if (gc->window_main_dock_id == 0)  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    else if (gc->window_main_dock_id == 1) io.ConfigFlags &= (~ImGuiConfigFlags_DockingEnable);
    if (gc->window_main_transparent_id == 0)  io.ConfigDockingTransparentPayload = true;
    else if (gc->window_main_transparent_id == 1) io.ConfigDockingTransparentPayload = false;
    gc->window_main_alpha = (float)FileSetting::GetDoubleValue(0, INIGROUPMARKSTR, inimark_map[::INIMARK::WINDOW_MAIN_ALPHA], 100.0f);
    gc->window_main_forecastfps = (float)FileSetting::GetDoubleValue(0, INIGROUPMARKSTR, inimark_map[::INIMARK::WINDOW_MAIN_FORECASTFPS], 120.0f);
    gc->window_main_addtimefps = FileSetting::GetLongValue(0, INIGROUPMARKSTR, inimark_map[::INIMARK::WINDOW_MAIN_ADDTIMEFPS], 0);
    gc->window_cubism_addtimefps = FileSetting::GetLongValue(0, INIGROUPMARKSTR, inimark_map[::INIMARK::WINDOW_CUBISM_ADDTIMEFPS], 0);
    gc->window_main_fast_id = FileSetting::GetLongValue(0, INIGROUPMARKSTR, inimark_map[::INIMARK::WINDOW_MAIN_FAST_ID], INITINT);
    gc->window_main_icon_offs = (float)FileSetting::GetDoubleValue(0, INIGROUPMARKSTR, inimark_map[::INIMARK::WINDOW_MAIN_ICON_OFFS], 0.30f);
    gc->window_main_icon_offx = (float)FileSetting::GetDoubleValue(0, INIGROUPMARKSTR, inimark_map[::INIMARK::WINDOW_MAIN_ICON_OFFX], 30.0f);
    gc->window_main_icon_offy = (float)FileSetting::GetDoubleValue(0, INIGROUPMARKSTR, inimark_map[::INIMARK::WINDOW_MAIN_ICON_OFFY], 8.0f);

#ifdef IS_WINDOWS
    gc->select_font = (std::string)FileSetting::GetValue(0, INIGROUPMARKSTR, inimark_map[::INIMARK::FONTSEL_NAME], INITSTR);
    io.FontGlobalScale =(float)FileSetting::GetDoubleValue(0, INIGROUPMARKSTR, inimark_map[::INIMARK::FONT_GLOBAL_SCALE], 1.0);
    std::vector<std::pair<std::string,std::string>> temp_fonts;
    std::string basePath = FONTS_BASE_PATH;
    std::string searchPattern = basePath + "*.ttf";
    wchar_t widePattern[MAX_PATH];
    MultiByteToWideChar(CP_UTF8, 0, searchPattern.c_str(), -1, widePattern, MAX_PATH);
    struct _wfinddata_t fdata;
    intptr_t fh = _wfindfirst(widePattern, &fdata);
    if (fh == -1) {
        std::cout << "No .ttf files found." << std::endl;
    }
    else {
        do {
            if (!(fdata.attrib & _A_SUBDIR)) {
                char fileName[MAX_PATH];
                WideCharToMultiByte(CP_UTF8, 0, fdata.name, -1, fileName, MAX_PATH, NULL, NULL);
                temp_fonts.push_back(std::pair<std::string, std::string>(fileName, basePath + fileName));
            }
        } while (_wfindnext(fh, &fdata) == 0);
        _findclose(fh);
        for (const auto& file : temp_fonts) {
            std::cout << file.second << std::endl;
        }
        std::cout << u8"TTF 文件读取完毕!" << std::endl;
        gc->fonts_size = temp_fonts.size();
        gc->fonts_list = new const char* [gc->fonts_size];
        gc->fonts_name_list = new const char* [gc->fonts_size];
        for (int i = 0; i < gc->fonts_size; ++i) {
            size_t length = strlen(temp_fonts[i].second.c_str());
            char* temp = new char[length + 1];
            strcpy_s(temp, length + 1, temp_fonts[i].second.c_str());
            gc->fonts_list[i] = temp;
            length = strlen(temp_fonts[i].first.c_str());
            temp = new char[length + 1];
            strcpy_s(temp, length + 1, temp_fonts[i].first.c_str());
            gc->fonts_name_list[i] = temp;
        }
    }
    
#endif //IS_WINDOWS
    //支持的字体范围
    static const ImWchar ranges[] = {
        0x0020, 0x007F, // 基本拉丁（ASCII）
        0x00A0, 0x00FF, // 拉丁-1 补充

        0x3000, 0x303F, // 日文符号和标点
        0x3040, 0x309F, // 平假名
        0x30A0, 0x30FF, // 片假名
        0xFF00, 0xFFEF, // 半角片假名及全角字符

        0x3400, 0x4DBF, // CJK 扩展 A
        0x4E00, 0x9FFF, // 基本汉字
        0xF900, 0xFAFF, // 兼容汉字
    };

    //设置字体
    bool canSetDef = false;
    std::vector<std::string> fv;
    ImFont* font_current = ImGui::GetFont();
    for (int i = 0; i < gc->fonts_size; ++i) {
        ImFont* font = io.Fonts->AddFontFromFileTTF(gc->fonts_list[i], 30.0f, NULL, ranges);
        fv.push_back(font->GetDebugName());
        if (strcmp(font->GetDebugName(), gc->select_font.c_str()) == 0) {
            io.FontDefault = font;
            canSetDef = true;
        }
    }
    if (canSetDef == false) {
        gc->select_font = fv.size() > 0 ? fv.front() : "ProggyClean.ttf";
    }
}

void GlobalConfig::GlobalConfigSave(GlobalConfig* gc)
{
    std::cout << u8"全局配置保存" << std::endl;
    CSimpleIniA ini;
    FileSetting::BeginSave(ini);

    FileSetting::SetLongValue(0, INIGROUPMARKSTR, inimark_map[::INIMARK::LANGUAGESEL_ID], (int)::GlobalConfig::getInstance()->select_lan);
    FileSetting::SetLongValue(0, INIGROUPMARKSTR, inimark_map[::INIMARK::USER_NUM], (int)::GlobalConfig::getInstance()->user_num);
    FileSetting::SetLongValue(0, INIGROUPMARKSTR, inimark_map[::INIMARK::THEMESEL_ID], (int)::GlobalConfig::getInstance()->select_theme_id);
    FileSetting::SetLongValue(0, INIGROUPMARKSTR, inimark_map[::INIMARK::WINDOW_MAIN_CLOSE], (int)::GlobalConfig::getInstance()->window_main_close);
    FileSetting::SetLongValue(0, INIGROUPMARKSTR, inimark_map[::INIMARK::WINDOW_MAIN_STYLE_ID], (int)::GlobalConfig::getInstance()->window_main_style_id);
    FileSetting::SetLongValue(0, INIGROUPMARKSTR, inimark_map[::INIMARK::WINDOW_MAIN_DOCK_ID], (int)::GlobalConfig::getInstance()->window_main_dock_id);
    FileSetting::SetLongValue(0, INIGROUPMARKSTR, inimark_map[::INIMARK::WINDOW_MAIN_TRANSPARENT_ID], (int)::GlobalConfig::getInstance()->window_main_transparent_id);
    FILE* fp;
    fopen_s(&fp, STYLE_WAY, "wb");
    if (fp != NULL) {
        fwrite(&ImGui::GetStyle(), sizeof(ImGuiStyle), 1, fp);
        fclose(fp);
    }
    FileSetting::SetLongValue(0, INIGROUPMARKSTR, inimark_map[::INIMARK::WINDOW_CUBISM_STYLE_ID], ::GlobalConfig::getInstance()->window_cubism_style_id);

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    FileSetting::SetDoubleValue(0, INIGROUPMARKSTR, inimark_map[::INIMARK::WINDOW_MAIN_ALPHA], (double)::GlobalConfig::getInstance()->window_main_alpha);
    FileSetting::SetDoubleValue(0, INIGROUPMARKSTR, inimark_map[::INIMARK::WINDOW_MAIN_FORECASTFPS], (double)::GlobalConfig::getInstance()->window_main_forecastfps);
    FileSetting::SetLongValue(0, INIGROUPMARKSTR, inimark_map[::INIMARK::WINDOW_MAIN_ADDTIMEFPS],::GlobalConfig::getInstance()->window_main_addtimefps);
    FileSetting::SetLongValue(0, INIGROUPMARKSTR, inimark_map[::INIMARK::WINDOW_CUBISM_ADDTIMEFPS], ::GlobalConfig::getInstance()->window_cubism_addtimefps);
    FileSetting::SetLongValue(0, INIGROUPMARKSTR, inimark_map[::INIMARK::WINDOW_MAIN_FAST_ID], ::GlobalConfig::getInstance()->window_main_fast_id);
    FileSetting::SetDoubleValue(0, INIGROUPMARKSTR, inimark_map[::INIMARK::WINDOW_MAIN_ICON_OFFS], (double)::GlobalConfig::getInstance()->window_main_icon_offs);
    FileSetting::SetDoubleValue(0, INIGROUPMARKSTR, inimark_map[::INIMARK::WINDOW_MAIN_ICON_OFFX], (double)::GlobalConfig::getInstance()->window_main_icon_offx);
    FileSetting::SetDoubleValue(0, INIGROUPMARKSTR, inimark_map[::INIMARK::WINDOW_MAIN_ICON_OFFY], (double)::GlobalConfig::getInstance()->window_main_icon_offy);
    FileSetting::SetValue(0, INIGROUPMARKSTR, inimark_map[::INIMARK::FONTSEL_NAME], ::GlobalConfig::getInstance()->select_font.c_str());
    FileSetting::SetDoubleValue(0, INIGROUPMARKSTR, inimark_map[::INIMARK::FONT_GLOBAL_SCALE], (double)io.FontGlobalScale);
    FileSetting::EndSave(ini);
    GlobalTemp::RefreshTable = true;
}

GlobalConfig::~GlobalConfig()
{
    if (fonts_size == 0 || fonts_list == NULL) { return; }
    for (int i = 0; i < fonts_size; ++i) {
        delete[] fonts_list[i];
        delete[] fonts_name_list[i];
    }
    delete[] fonts_list;
    delete[] fonts_name_list;
}
