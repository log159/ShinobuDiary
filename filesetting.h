#pragma once
#include<iostream>
#include<string>
#include "./simpleini-4.22_src/ConvertUTF.h"
#include "./simpleini-4.22_src/SimpleIni.h"
#include <wincrypt.h>

using std::cout;
using std::endl;
#define INITSTR ""
#define INITINT 0
#define INITDOUBLE 0.0
#define INITBOOL false
#define INITFILE "./Saves/userconfig.ini"
#define DEFSIZE 256
#define DEFSIZE2 DEFSIZE*2
#define DEFSIZE4 DEFSIZE*4
#define DEFSIZE8 DEFSIZE*8
#define DEFSIZE16 DEFSIZE*16
#define DEFSIZEK 1024
#define DEFSIZEK16 DEFSIZEK * 16


class FileSetting {
public:

    static void S_SetValue(int id,const char* s, const char* k,const char* n);
    static std::string S_GetValue(int id, const char* s, const char* k, const char* n);
    static void S_SetLongValue(int id, const char* s, const char* k, int n);
    static int S_GetLongValue(int id, const char* s, const char* k, int n);
    static void S_SetDoubleValue(int id, const char* s, const char* k, double n);
    static double S_GetDoubleValue(int id, const char* s, const char* k, double n);
    static void S_SetBoolValue(int id, const char* s, const char* k, bool n);
    static bool S_GetBoolValue(int id, const char* s, const char* k, bool n);


private:
    FileSetting();
    ~FileSetting();
    FileSetting(const FileSetting& single) = delete;
    const FileSetting& operator=(const FileSetting& single) = delete;
  
};