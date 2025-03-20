#pragma once
#include <iostream>
#include <string.h>
#include <string>
#include <vector>
#include "timeconfig.h"
#include "sizedef.h"
#include <cmath>
#include "imgui.h"
#include "imgui_internal.h"

namespace Su {

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
    extern void MemsetStr(char* str, size_t size);
    extern void GetGuiMark(char* markbuf, size_t size, const char* name, const char* id);
    extern void StringSplit(std::string str, const char split, std::vector<std::string>& sv);
    extern void HelpMarker(const char* desc);
    extern void ColorConvertHSVtoRGB(float h, float s, float v, float& out_r, float& out_g, float& out_b);

}