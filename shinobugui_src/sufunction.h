#pragma once
#include "imgui.h"
#include "imgui_internal.h"
#include <string>
#include <vector>
#include <iostream>

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

    extern void StringSplit(std::string str, const char split, std::vector<std::string>& sv);
    extern void HelpMarker(const char* desc);
    extern void ColorConvertHSVtoRGB(float h, float s, float v, float& out_r, float& out_g, float& out_b);

    template <typename T>
    T Clamp(T value, T min, T max) {
        return (value < min) ? min : (value > max ? max : value);
    }
}