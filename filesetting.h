#pragma once
#include "sizedef.h"
#include <string>
#include "./simpleini-4.22_src/ConvertUTF.h"
#include "./simpleini-4.22_src/SimpleIni.h"
#include "./somemacros.h"


class FileSetting {
public:
    static CSimpleIniA* delay_ini;
    static CSimpleIniA read_ini;
    static bool can_read;

    static void BeginSave(CSimpleIniA& ini);
    static void EndSave(CSimpleIniA& ini);
    static void RefreshRead();

    static void SetValue(int id,const char* s, const char* k,const char* wn);
    static std::string GetValue(int id, const char* s, const char* k, const char* n);
    static void SetLongValue(int id, const char* s, const char* k, int n);
    static int GetLongValue(int id, const char* s, const char* k, int n);
    static void SetDoubleValue(int id, const char* s, const char* k, double n);
    static double GetDoubleValue(int id, const char* s, const char* k, double n);
    static void SetBoolValue(int id, const char* s, const char* k, bool n);
    static bool GetBoolValue(int id, const char* s, const char* k, bool n);
    static void ClearFile();

private:
    FileSetting();
    ~FileSetting();
    FileSetting(const FileSetting& single);
    const FileSetting& operator=(const FileSetting& single);
  
};