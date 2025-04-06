#pragma once
#include "imgui.h"
#include "imgui_internal.h"
#include <string>
#include <vector>
#include <iostream>
#include "./global.h"
#include "./somemacros.h"
enum INIMARK;

namespace Su {
    extern std::string getCurrentTime();
    extern std::string getCurrentYear();
    extern std::string getCurrentMonth();
    extern std::string getCurrentDay();
    extern std::string getCurrentHours();
    extern std::string getCurrentMinutes();
    extern std::string getCurrentSeconds();

    struct ShinobuExList
    {
        ImVector<ImGuiID>           Items[2];
        ImGuiSelectionBasicStorage  Selections[2];
        bool                        OptKeepSorted = true;
        static int CompareItemsByValue(const void* lhs, const void* rhs);
        void SortItems(int n);
        void MoveAll(int src, int dst);
        void MoveSelected(int src, int dst);
        void ApplySelectionRequests(ImGuiMultiSelectIO* ms_io, int side);
        bool Show(const char** ex_name);
    };
    struct ShinobuScrollingBuffer {
        int MaxSize;
        int Offset;
        ImVector<ImVec2> Data;
        ShinobuScrollingBuffer(int max_size = 4000);
        void AddPoint(float x, float y);
        void Erase();
    };
    struct ShinobuRollingBuffer {
        float Span;
        ImVector<ImVec2> Data;
        ShinobuRollingBuffer();
        void AddPoint(float x, float y);
    };
    extern void MemsetStr(char* str, size_t size);
    extern void JoinGuiMark(char* markbuf, size_t size, const char* name, const char* id);
    extern bool IsSubstring(const char* haystack, const char* needle);
    extern void StringSplit(std::string str, const char split, std::vector<std::string>& sv);
    extern void HelpMarker(const char* desc);
    extern void ColorConvertHSVtoRGB(float h, float s, float v, float& out_r, float& out_g, float& out_b);
    extern std::wstring Utf8ToWstring(const std::string& utf8_str);

    template <typename T>
    T Clamp(T value, T min, T max) {
        return (value < min) ? min : (value > max ? max : value);
    }

    template<typename FT, typename T>
    void SaveKasb(FT* uc, T* tc) {
        static char key[DEFSIZE2];
        static char appid[DEFSIZE2];
        static char secret[DEFSIZE2];
        static char baseurl[DEFSIZE2];
        JoinGuiMark(key, sizeof(key), ::inimark_map[INIMARK::KEY], tc->name);
        JoinGuiMark(appid, sizeof(appid), ::inimark_map[INIMARK::APPID], tc->name);
        JoinGuiMark(secret, sizeof(secret), ::inimark_map[INIMARK::SECRET], tc->name);
        JoinGuiMark(baseurl, sizeof(baseurl), ::inimark_map[INIMARK::BASEURL], tc->name);
        if (POS_0 & tc->kasb.needpos)FileSetting::SetValue(uc->user_id, INIGROUPMARKSTR, key, tc->kasb.key);
        if (POS_1 & tc->kasb.needpos)FileSetting::SetValue(uc->user_id, INIGROUPMARKSTR, appid, tc->kasb.appid);
        if (POS_2 & tc->kasb.needpos)FileSetting::SetValue(uc->user_id, INIGROUPMARKSTR, secret, tc->kasb.secret);
        if (POS_3 & tc->kasb.needpos)FileSetting::SetValue(uc->user_id, INIGROUPMARKSTR, baseurl, tc->kasb.baseurl);
    }
    template<typename FT, typename T>
    void SaveAp(FT* uc, T* tc) {
        static char address[DEFSIZE2];
        static char port[DEFSIZE2];
        JoinGuiMark(address, sizeof(address), ::inimark_map[INIMARK::ADDRESS], tc->name);
        JoinGuiMark(port, sizeof(port), ::inimark_map[INIMARK::PORT], tc->name);
        FileSetting::SetValue(uc->user_id, INIGROUPMARKSTR, address, tc->ap.address);
        FileSetting::SetValue(uc->user_id, INIGROUPMARKSTR, port, tc->ap.port);
    }

    template<typename FT, typename T>
    void InitKasb(FT* uc, T* tc) {
        static char key[DEFSIZE2];
        static char appid[DEFSIZE2];
        static char secret[DEFSIZE2];
        static char baseurl[DEFSIZE2];
        JoinGuiMark(key, sizeof(key), ::inimark_map[INIMARK::KEY], tc->name);
        JoinGuiMark(appid, sizeof(appid), ::inimark_map[INIMARK::APPID], tc->name);
        JoinGuiMark(secret, sizeof(secret), ::inimark_map[INIMARK::SECRET], tc->name);
        JoinGuiMark(baseurl, sizeof(baseurl), ::inimark_map[INIMARK::BASEURL], tc->name);
        if (POS_0 & tc->kasb.needpos)strcpy_s(tc->kasb.key, sizeof(tc->kasb.key), FileSetting::GetValue(uc->user_id, INIGROUPMARKSTR, key, INITSTR).c_str());
        if (POS_1 & tc->kasb.needpos)strcpy_s(tc->kasb.appid, sizeof(tc->kasb.appid), FileSetting::GetValue(uc->user_id, INIGROUPMARKSTR, appid, INITSTR).c_str());
        if (POS_2 & tc->kasb.needpos)strcpy_s(tc->kasb.secret, sizeof(tc->kasb.secret), FileSetting::GetValue(uc->user_id, INIGROUPMARKSTR, secret, INITSTR).c_str());
        if (POS_3 & tc->kasb.needpos)strcpy_s(tc->kasb.baseurl, sizeof(tc->kasb.baseurl), FileSetting::GetValue(uc->user_id, INIGROUPMARKSTR, baseurl, INITSTR).c_str());
    }
    template<typename FT, typename T>
    void InitAp(FT* uc, T* tc) {
        static char address[DEFSIZE2];
        static char port[DEFSIZE2];
        JoinGuiMark(address, sizeof(address), ::inimark_map[INIMARK::ADDRESS], tc->name);
        JoinGuiMark(port, sizeof(port), ::inimark_map[INIMARK::PORT], tc->name);
        strcpy_s(tc->ap.address, sizeof(tc->ap.address), FileSetting::GetValue(uc->user_id, INIGROUPMARKSTR, address, INITSTR).c_str());
        strcpy_s(tc->ap.port, sizeof(tc->ap.port), FileSetting::GetValue(uc->user_id, INIGROUPMARKSTR, port, INITSTR).c_str());
    }

}