#pragma once
#include "suconfig.h"

#define KASB_FUNCTION(ct)\
static char key[DEFSIZE2];\
static char appid[DEFSIZE2];\
static char secret[DEFSIZE2];\
static char baseurl[DEFSIZE2];\
Su::GetGuiMark(key, sizeof(key), ::inimark_map[::INIMARK::KEY], ct->name);\
Su::GetGuiMark(appid, sizeof(appid), ::inimark_map[::INIMARK::APPID], ct->name);\
Su::GetGuiMark(secret, sizeof(secret), ::inimark_map[::INIMARK::SECRET], ct->name);\
Su::GetGuiMark(baseurl, sizeof(baseurl), ::inimark_map[::INIMARK::BASEURL], ct->name);\
if (POS_0 & ct->kasb.needpos)ImGui::InputText(key, ct->kasb.key, IM_ARRAYSIZE(ct->kasb.key));\
if (POS_1 & ct->kasb.needpos)ImGui::InputText(appid, ct->kasb.appid, IM_ARRAYSIZE(ct->kasb.appid));\
if (POS_2 & ct->kasb.needpos)ImGui::InputText(secret, ct->kasb.secret, IM_ARRAYSIZE(ct->kasb.secret));\
if (POS_3 & ct->kasb.needpos)ImGui::InputText(baseurl, ct->kasb.baseurl, IM_ARRAYSIZE(ct->kasb.baseurl));

#define AP_FUNCTION(ct)\
static char address[DEFSIZE2];\
static char port[DEFSIZE2];\
Su::GetGuiMark(address, sizeof(address), ::inimark_map[::INIMARK::ADDRESS], ct->name);\
Su::GetGuiMark(port, sizeof(port), ::inimark_map[::INIMARK::PORT], ct->name);\
ImGui::InputText(address, ct->ap.address, IM_ARRAYSIZE(ct->ap.address));\
ImGui::InputText(port, ct->ap.port, IM_ARRAYSIZE(ct->ap.port));

#define CM_FUNCTION(ct,v,s,tp)\
const char* name = nullptr;\
name=v[(int)s].getLanName();\
if (ImGui::BeginCombo(ct, name))\
{\
    for (int i = 0; i < int(v.size()); i++)\
    {\
        bool isSelected = (s == i);\
        name = v[i].getLanName();\
        if (ImGui::Selectable(name, isSelected)) {\
            s = (tp)i;\
            std::cout << "Setting Index :" << s << std::endl;\
            Su::UserConfigSave(_uc);\
        }\
        if (isSelected) ImGui::SetItemDefaultFocus();\
    }\
    ImGui::EndCombo();\
}
