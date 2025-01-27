﻿#include "global.h"
#include <vector>
#include <string>


//平台支持
#define IS_WINDOWS
//#define IS_WCHAR
#ifdef IS_WINDOWS
#include <windows.h>

#endif // IS_WINDOWS

bool GlobalTemp::ShowStyleEditor = false;
HWND GlobalTemp::WindowMainHandle;
WNDCLASSEXW GlobalTemp::WindowMainWc;
int GlobalTemp::CubismFrameCount = 0;
std::string GlobalTemp::LunarCalendar = Su::GetLunar();

void GlobalConfig::GlobalConfigInit(GlobalConfig* gc) {
    static bool initHas = false;if (initHas == true) return;initHas = true;
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    std::cout << u8"全局配置初始化" << std::endl;

    gc->select_lan = (LAN)FileSetting::GetLongValue(0, INIGROUPMARKSTR, inifreemark_map[::FREEMARK::LANGUAGESEL_ID], INITINT);
    gc->user_num = FileSetting::GetLongValue(0, INIGROUPMARKSTR, inifreemark_map[::FREEMARK::USER_NUM], INITINT);
    gc->select_theme_id = FileSetting::GetLongValue(0, INIGROUPMARKSTR, inifreemark_map[::FREEMARK::THEMESEL_ID], INITINT);
    gc->defaultStyle = ImGui::GetStyle();
    FILE* fpp;
    if (fopen_s(&fpp, STYLEWAY, "rb") == 0) {
        ImGuiStyle st;
        fread(&st, sizeof(ImGuiStyle), 1, fpp);
        ImGui::GetStyle() = st;
        fclose(fpp);
    }
    gc->select_font = (std::string)FileSetting::GetValue(0, INIGROUPMARKSTR, inifreemark_map[::FREEMARK::FONTSEL_NAME], INITSTR);
    gc->window_cubism_style_id = FileSetting::GetLongValue(0, INIGROUPMARKSTR, inifreemark_map[::FREEMARK::WINDOW_CUBISM_STYLE_ID], INITINT);

    gc->window_main_style_id = FileSetting::GetLongValue(0, INIGROUPMARKSTR, inifreemark_map[::FREEMARK::WINDOW_MAIN_STYLE_ID], INITINT);
    gc->window_main_dock_id = FileSetting::GetLongValue(0, INIGROUPMARKSTR, inifreemark_map[::FREEMARK::WINDOW_MAIN_DOCK_ID], INITINT);
    gc->window_main_transparent_id = FileSetting::GetLongValue(0, INIGROUPMARKSTR, inifreemark_map[::FREEMARK::WINDOW_MAIN_TRANSPARENT_ID], INITINT);

    if (gc->window_main_style_id == 0)
        ::SetWindowPos(GlobalTemp::WindowMainHandle, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    else if (gc->window_main_style_id == 1)
        ::SetWindowPos(GlobalTemp::WindowMainHandle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    if (gc->window_main_dock_id == 0)  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    else if (gc->window_main_dock_id == 1) io.ConfigFlags &= (~ImGuiConfigFlags_DockingEnable);
    if (gc->window_main_transparent_id == 0)  io.ConfigDockingTransparentPayload = true;
    else if (gc->window_main_transparent_id == 1) io.ConfigDockingTransparentPayload = false;

    gc->window_main_forecastfps = (float)FileSetting::GetDoubleValue(0, INIGROUPMARKSTR, inifreemark_map[::FREEMARK::WINDOW_MAIN_FORECASTFPS], 120.0f);
    gc->window_main_addtimefps = FileSetting::GetLongValue(0, INIGROUPMARKSTR, inifreemark_map[::FREEMARK::WINDOW_MAIN_ADDTIMEFPS], 0);

    gc->window_cubism_addtimefps = FileSetting::GetLongValue(0, INIGROUPMARKSTR, inifreemark_map[::FREEMARK::WINDOW_CUBISM_ADDTIMEFPS], 0);


    
#ifdef IS_WINDOWS
    WIN32_FIND_DATA wfd;
    HANDLE hFind;
    hFind = FindFirstFile(FONTSWAY, &wfd);
    if (hFind == INVALID_HANDLE_VALUE) {
        cout << "Invalid File Handle. GetLastError reports " << GetLastError() << endl;
    }
    else {
        std::vector<const char*> temp_fonts;
        std::vector<const char*> temp_name_fonts;

        char fileName[MAX_PATH];
        char fileNameWay[MAX_PATH];
        cout << " File Handle. GetLastError reports " << GetLastError() << endl;
#if defined(IS_WCHAR)
        WideCharToMultiByte(CP_UTF8, 0, wfd.cFileName, -1, fileName, MAX_PATH, NULL, NULL);
#elif !defined(IS_WCHAR)
        strcpy_s(fileName, sizeof(fileName), wfd.cFileName);
#endif
        temp_name_fonts.push_back(_strdup(fileName));
        snprintf(fileNameWay,sizeof(fileNameWay),FONTSTR,fileName);
        temp_fonts.push_back(_strdup(fileNameWay));
        while (FindNextFile(hFind, &wfd)) {
#if defined(IS_WCHAR)
            WideCharToMultiByte(CP_UTF8, 0, wfd.cFileName, -1, fileName, MAX_PATH, NULL, NULL);
#elif !defined(IS_WCHAR)
            strcpy_s(fileName, sizeof(fileName), wfd.cFileName);
#endif
            temp_name_fonts.push_back(_strdup(fileName));
            snprintf(fileNameWay, sizeof(fileNameWay), FONTSTR, fileName);
            temp_fonts.push_back(_strdup(fileNameWay));
        }
        gc->fonts_size = temp_fonts.size();
        gc->fonts_list = new const char* [gc->fonts_size];
        gc->fonts_name_list = new const char* [gc->fonts_size];
        for (int i = 0; i < gc->fonts_size; ++i) {
            gc->fonts_list[i] = temp_fonts[i];
            gc->fonts_name_list[i] = temp_name_fonts[i];
            cout <<i<<" "<< gc->fonts_list[i] << endl;
        }
        cout << "--------------------------------" << endl;
        temp_fonts.clear();
    }
    FindClose(hFind);

#endif // IS_WINDOWS
    //设置字体
    ImFont* font_current = ImGui::GetFont();
    for (int i = 0; i < gc->fonts_size; ++i) {
        ImFont* font = io.Fonts->AddFontFromFileTTF(gc->fonts_list[i],30.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
        if (strcmp(font->GetDebugName(), gc->select_font.c_str()) == 0) {
            io.FontDefault = font;
        }
    }
    io.FontGlobalScale =(float)FileSetting::GetDoubleValue(0, INIGROUPMARKSTR, inifreemark_map[::FREEMARK::FONT_GLOBAL_SCALE], 1.0);


}

void GlobalConfig::GlobalConfigSave(GlobalConfig* gc)
{

    std::cout << u8"全局配置保存" << std::endl;
    FileSetting::SetLongValue(0, INIGROUPMARKSTR, inifreemark_map[::FREEMARK::LANGUAGESEL_ID], (int)::GlobalConfig::getInstance()->select_lan);
    FileSetting::SetLongValue(0, INIGROUPMARKSTR, inifreemark_map[::FREEMARK::USER_NUM], (int)::GlobalConfig::getInstance()->user_num);
    FileSetting::SetLongValue(0, INIGROUPMARKSTR, inifreemark_map[::FREEMARK::THEMESEL_ID], (int)::GlobalConfig::getInstance()->select_theme_id);

    FileSetting::SetLongValue(0, INIGROUPMARKSTR, inifreemark_map[::FREEMARK::WINDOW_MAIN_STYLE_ID], (int)::GlobalConfig::getInstance()->window_main_style_id);
    FileSetting::SetLongValue(0, INIGROUPMARKSTR, inifreemark_map[::FREEMARK::WINDOW_MAIN_DOCK_ID], (int)::GlobalConfig::getInstance()->window_main_dock_id);
    FileSetting::SetLongValue(0, INIGROUPMARKSTR, inifreemark_map[::FREEMARK::WINDOW_MAIN_TRANSPARENT_ID], (int)::GlobalConfig::getInstance()->window_main_transparent_id);

    FILE* fp;
    fopen_s(&fp, STYLEWAY, "wb");
    if (fp != NULL) {
        fwrite(&ImGui::GetStyle(), sizeof(ImGuiStyle), 1, fp);
        fclose(fp);
    }
    FileSetting::SetValue(0, INIGROUPMARKSTR, inifreemark_map[::FREEMARK::FONTSEL_NAME],::GlobalConfig::getInstance()->select_font.c_str());
    FileSetting::SetLongValue(0, INIGROUPMARKSTR, inifreemark_map[::FREEMARK::WINDOW_CUBISM_STYLE_ID], ::GlobalConfig::getInstance()->window_cubism_style_id);

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    FileSetting::SetDoubleValue(0, INIGROUPMARKSTR, inifreemark_map[::FREEMARK::FONT_GLOBAL_SCALE], (double)io.FontGlobalScale);
    FileSetting::SetDoubleValue(0, INIGROUPMARKSTR, inifreemark_map[::FREEMARK::WINDOW_MAIN_FORECASTFPS], (double)::GlobalConfig::getInstance()->window_main_forecastfps);
    FileSetting::SetLongValue(0, INIGROUPMARKSTR, inifreemark_map[::FREEMARK::WINDOW_MAIN_ADDTIMEFPS],::GlobalConfig::getInstance()->window_main_addtimefps);

    FileSetting::SetLongValue(0, INIGROUPMARKSTR, inifreemark_map[::FREEMARK::WINDOW_CUBISM_ADDTIMEFPS], ::GlobalConfig::getInstance()->window_cubism_addtimefps);

}

GlobalConfig::~GlobalConfig()
{
    if (fonts_size == 0 && fonts_list == NULL) { return; }
    for (int i = 0; i < fonts_size; ++i) {
        free((void*)fonts_list[i]);
        free((void*)fonts_name_list[i]);
    }
    delete[] fonts_list;
    delete[] fonts_name_list;
}
