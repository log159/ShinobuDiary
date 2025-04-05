/* 兼容C++17 */
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include "sufunction.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <windows.h>
#include <cmath>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <codecvt>
#include "./global.h"
#include "../translator.h"
#include "../shinobugui_src/sizedef.h"

#define GETCURRENT(S)\
auto now = std::chrono::system_clock::now();\
auto timeT = std::chrono::system_clock::to_time_t(now);\
std::tm timeStruct;\
localtime_s(&timeStruct, &timeT);\
std::ostringstream oss;\
oss << std::put_time(&timeStruct, S);\
return oss.str();

namespace Su {
    std::string getCurrentTime() {
        auto now = std::chrono::system_clock::now();
        auto timeT = std::chrono::system_clock::to_time_t(now);
        std::tm timeStruct;
        localtime_s(&timeStruct, &timeT);
        auto duration = now.time_since_epoch();
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000;
        std::ostringstream oss;
        oss << std::put_time(&timeStruct, "%Y-%m-%d %H:%M:%S");
        oss << "." << std::setfill('0') << std::setw(3) << milliseconds;
        return oss.str();
    }
    std::string getCurrentYear() {
        GETCURRENT("%Y")
    }
    std::string getCurrentMonth() {
        GETCURRENT("%m")
    }
    std::string getCurrentDay() {
        GETCURRENT("%d")
    }
    std::string getCurrentHours() {
        GETCURRENT("%H")
    }
    std::string getCurrentMinutes() {
        GETCURRENT("%M")
    }
    std::string getCurrentSeconds() {
        GETCURRENT("%S")
    }

    int ShinobuExList::CompareItemsByValue(const void* lhs, const void* rhs)
    {
        const int* a = (const int*)lhs;
        const int* b = (const int*)rhs;
        return (*a - *b) > 0 ? +1 : -1;
    }
    void ShinobuExList::SortItems(int n)
    {
        qsort(Items[n].Data, (size_t)Items[n].Size, sizeof(Items[n][0]), CompareItemsByValue);
    }
    void ShinobuExList::MoveAll(int src, int dst)
    {
        IM_ASSERT((src == 0 && dst == 1) || (src == 1 && dst == 0));
        for (ImGuiID item_id : Items[src])
            Items[dst].push_back(item_id);
        Items[src].clear();
        SortItems(dst);
        Selections[src].Swap(Selections[dst]);
        Selections[src].Clear();
    }
    void ShinobuExList::MoveSelected(int src, int dst)
    {
        for (int src_n = 0; src_n < Items[src].Size; src_n++)
        {
            ImGuiID item_id = Items[src][src_n];
            if (!Selections[src].Contains(item_id))
                continue;
            Items[src].erase(&Items[src][src_n]);
            Items[dst].push_back(item_id);
            src_n--;
        }
        if (OptKeepSorted)
            SortItems(dst);
        Selections[src].Swap(Selections[dst]);
        Selections[src].Clear();
    }
    void ShinobuExList::ApplySelectionRequests(ImGuiMultiSelectIO* ms_io, int side)
    {
        Selections[side].UserData = Items[side].Data;
        Selections[side].AdapterIndexToStorageId = [](ImGuiSelectionBasicStorage* self, int idx) {
            ImGuiID* items = (ImGuiID*)self->UserData;
            return items[idx];
        };
        Selections[side].ApplyRequests(ms_io);
    }
    bool ShinobuExList::Show(const char** ex_name)
    {
        bool happen_move = false;
        if (ImGui::BeginTable(u8"###___ShinobuExList", 3, ImGuiTableFlags_None))
        {
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableNextRow();
            int request_move_selected = -1;
            int request_move_all = -1;
            float child_height_0 = 0.0f;
            for (int side = 0; side < 2; side++)
            {
                ImVector<ImGuiID>& items = Items[side];
                ImGuiSelectionBasicStorage& selection = Selections[side];
                ImGui::TableSetColumnIndex((side == 0) ? 0 : 2);
                ImGui::Text("%s (%d)", (side == 0) ? TT_347 : TT_348, items.Size);
                const float items_height = ImGui::GetTextLineHeightWithSpacing();
                ImGui::SetNextWindowContentSize(ImVec2(0.0f, items.Size * items_height));
                bool child_visible;
                if (side == 0)
                {
                    ImGui::SetNextWindowSizeConstraints(ImVec2(0.0f, ImGui::GetFrameHeightWithSpacing() * 4), ImVec2(FLT_MAX, FLT_MAX));
                    child_visible = ImGui::BeginChild("0", ImVec2(-FLT_MIN, ImGui::GetFontSize() * 20), ImGuiChildFlags_FrameStyle | ImGuiChildFlags_ResizeY);
                    child_height_0 = ImGui::GetWindowSize().y;
                }
                else
                {
                    child_visible = ImGui::BeginChild("1", ImVec2(-FLT_MIN, child_height_0), ImGuiChildFlags_FrameStyle);
                }
                if (child_visible)
                {
                    ImGuiMultiSelectFlags flags = ImGuiMultiSelectFlags_None;
                    ImGuiMultiSelectIO* ms_io = ImGui::BeginMultiSelect(flags, selection.Size, items.Size);
                    ApplySelectionRequests(ms_io, side);
                    for (int item_n = 0; item_n < items.Size; item_n++)
                    {
                        ImGuiID item_id = items[item_n];
                        bool item_is_selected = selection.Contains(item_id);
                        ImGui::SetNextItemSelectionUserData(item_n);
                        ImGui::Selectable(ex_name[item_id], item_is_selected, ImGuiSelectableFlags_AllowDoubleClick);
                        if (ImGui::IsItemFocused())
                        {
                            if (ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter))
                                request_move_selected = side;
                            if (ImGui::IsMouseDoubleClicked(0))
                                request_move_selected = side;
                        }
                    }
                    ms_io = ImGui::EndMultiSelect();
                    ApplySelectionRequests(ms_io, side);
                }
                ImGui::EndChild();
            }
            ImGui::TableSetColumnIndex(1);
            ImGui::NewLine();
            ImVec2 button_sz = { ImGui::GetFrameHeight(), ImGui::GetFrameHeight() };

            if (ImGui::Button(">>", button_sz))
                request_move_all = 0;
            if (ImGui::Button(">", button_sz))
                request_move_selected = 0;
            if (ImGui::Button("<", button_sz))
                request_move_selected = 1;
            if (ImGui::Button("<<", button_sz))
                request_move_all = 1;
            if (request_move_all != -1) {
                MoveAll(request_move_all, request_move_all ^ 1);
                happen_move = true;
            }
            if (request_move_selected != -1) {
                happen_move = true;
                MoveSelected(request_move_selected, request_move_selected ^ 1);
            }
            ImGui::EndTable();
        }
        return happen_move;
    }

    ShinobuScrollingBuffer::ShinobuScrollingBuffer(int max_size) {
        MaxSize = max_size;
        Offset = 0;
        Data.reserve(MaxSize);
    }
    void ShinobuScrollingBuffer::AddPoint(float x, float y) {
        if (Data.size() < MaxSize)
            Data.push_back(ImVec2(x, y));
        else {
            Data[Offset] = ImVec2(x, y);
            Offset = (Offset + 1) % MaxSize;
        }
    }
    void ShinobuScrollingBuffer::Erase() {
        if (Data.size() > 0) {
            Data.shrink(0);
            Offset = 0;
        }
    }

    ShinobuRollingBuffer::ShinobuRollingBuffer() {
        Span = 10.0f;
        Data.reserve(2000);
    }
    void ShinobuRollingBuffer::AddPoint(float x, float y) {
        float xmod = fmodf(x, Span);
        if (!Data.empty() && xmod < Data.back().x)
            Data.shrink(0);
        Data.push_back(ImVec2(xmod, y));
    }

    void MemsetStr(char* str, size_t size) {
        memset(str, 0, size);
    }
    void JoinGuiMark(char* markbuf, size_t size, const char* name, const char* id) {
        MemsetStr(markbuf, size);
        strcpy_s(markbuf, size, name);
        strcat_s(markbuf, size, "##");
        strcat_s(markbuf, size, id);
    }
    void StringSplit(std::string str, const char split,std::vector<std::string>& sv)
    {
        sv.clear();
        std::istringstream iss(str);
        std::string token;
        while (getline(iss, token, split))
            sv.push_back(token);
    }
    void HelpMarker(const char* desc)
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
    void ColorConvertHSVtoRGB(float h, float s, float v, float& out_r, float& out_g, float& out_b)
    {
        if (s == 0.0f)
        {
            out_r = out_g = out_b = v;
            return;
        }
        h = fmodf(h , 1.0f) / (60.0f / 360.0f);
        int   i = (int)h;
        float f = h - (float)i;
        float p = v * (1.0f - s);
        float q = v * (1.0f - s * f);
        float t = v * (1.0f - s * (1.0f - f));

        switch (i)
        {
        case 0: out_r = v; out_g = t; out_b = p; break;
        case 1: out_r = q; out_g = v; out_b = p; break;
        case 2: out_r = p; out_g = v; out_b = t; break;
        case 3: out_r = p; out_g = q; out_b = v; break;
        case 4: out_r = t; out_g = p; out_b = v; break;
        case 5: default: out_r = v; out_g = p; out_b = q; break;
        }
    }
    std::wstring Utf8ToWstring(const std::string& utf8_str) {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.from_bytes(utf8_str);
    }
    bool IsSubstring(const char* haystack, const char* needle) {
        return std::strstr(haystack, needle) != nullptr;
    }
}
