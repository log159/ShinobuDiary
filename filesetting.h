#pragma once
#include<iostream>
#include<string>
#include "./simpleini-4.22_src/ConvertUTF.h"
#include "./simpleini-4.22_src/SimpleIni.h"
#include <wincrypt.h>
#include "sizedef.h"

using std::cout;
using std::endl;
#define INITSTR ""
#define INITINT 0
#define INITDOUBLE 0.0
#define INITBOOL false
#define INITFILE "./Saves/userconfig.ini"

class FileSetting {
public:

    static void SetValue(int id,const char* s, const char* k,const char* n);
    static std::string GetValue(int id, const char* s, const char* k, const char* n);
    static void SetLongValue(int id, const char* s, const char* k, int n);
    static int GetLongValue(int id, const char* s, const char* k, int n);
    static void SetDoubleValue(int id, const char* s, const char* k, double n);
    static double GetDoubleValue(int id, const char* s, const char* k, double n);
    static void SetBoolValue(int id, const char* s, const char* k, bool n);
    static bool GetBoolValue(int id, const char* s, const char* k, bool n);


private:
    FileSetting();
    ~FileSetting();
    FileSetting(const FileSetting& single) = delete;
    const FileSetting& operator=(const FileSetting& single) = delete;
  
};