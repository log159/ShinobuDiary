//using SI_Error = int;
//constexpr int SI_OK = 0;        //!< No error
//constexpr int SI_UPDATED = 1;   //!< An existing value was updated
//constexpr int SI_INSERTED = 2;  //!< A new value was inserted
//// note: test for any error with (retval < 0)
//constexpr int SI_FAIL = -1;     //!< Generic failure
//constexpr int SI_NOMEM = -2;    //!< Out of memory error
//constexpr int SI_FILE = -3;     //!< File error (see errno for detail error)

#include "filesetting.h"
#include <iostream>
#include "../shinobugui_src/sizedef.h"
#include "../shinobugui_src/somemacros.h"
using std::cout;
using std::endl;

CSimpleIniA* FileSetting::delay_ini = nullptr;
CSimpleIniA FileSetting::read_ini;
bool FileSetting::can_read = false;
#define SS(SET_FUNCTION) \
if (delay_ini == nullptr)return;\
static char ss[DEFPATHSIZE];\
memset(ss, sizeof(ss), 0);\
snprintf(ss, sizeof(ss), s, id);\
FileSetting::delay_ini->SET_FUNCTION(ss, k, n);

#define SG(GET_FUNCTION,RETURN_TYPE) \
static char ss[DEFPATHSIZE];\
memset(ss, sizeof(ss), 0);\
snprintf(ss, sizeof(ss), s, id);\
read_ini.SetUnicode(true);\
if (!can_read)\
return RETURN_TYPE(n);\
return RETURN_TYPE(read_ini.GET_FUNCTION(ss, k, n));

void FileSetting::BeginSave(CSimpleIniA & ini)
{
    delay_ini = &ini;
    ini.SetUnicode(true);
    if (ini.LoadFile(INITFILE_WAY) < 0)
        std::cout << "New Ini File!" << std::endl;
}
void FileSetting::EndSave(CSimpleIniA& ini)
{
    delay_ini = nullptr;
    ini.SaveFile(INITFILE_WAY);
    std::cout << "FileSetting Save" << std::endl;
    FileSetting::RefreshRead();
    std::cout << "FileSetting RefreshRead" << std::endl;
}
void FileSetting::RefreshRead()
{
    if (read_ini.LoadFile(INITFILE_WAY)<0) can_read = false;
    else can_read = true;
}
void FileSetting::SetValue(int id, const char* s, const char* k, const char* wn) {
    std::string str = wn;
    std::string target = " ";
    std::string replacement = "~SPACE~";
    size_t pos = 0;
    while ((pos = str.find(target, pos)) != std::string::npos) {
        str.replace(pos, target.length(), replacement);
        pos += replacement.length();
    }
    const char* n = str.c_str();
    SS(SetValue);
}
void FileSetting::ClearFile()
{
    if (std::remove(INITFILE_WAY) == 0)
        std::cout << "File " << INITFILE_WAY << " deleted successfully." << std::endl;
    else
        std::cout << "Error deleting file." << std::endl;
}
std::string FileSetting::GetValue(int id, const char* s, const char* k, const char* n) {
    CSimpleIniA ini;
    static char ss[DEFPATHSIZE];
    memset(ss, sizeof(ss), 0);
    snprintf(ss, sizeof(ss), s, id);
    ini.SetUnicode(true);
    if (ini.LoadFile(INITFILE_WAY) < 0)
        return std::string(n);
    std::string str = ini.GetValue(ss, k, n);
    std::string target = "~SPACE~";
    std::string replacement = " ";
    size_t pos = 0;
    while ((pos = str.find(target, pos)) != std::string::npos) {
        str.replace(pos, target.length(), replacement);
        pos += replacement.length();
    }
    return str;
}
void FileSetting::SetLongValue(int id, const char* s, const char* k, int n) {
    SS(SetLongValue)
}
int FileSetting::GetLongValue(int id, const char* s, const char* k, int n) {
    SG(GetLongValue, int)
}
void FileSetting::SetDoubleValue(int id, const char* s, const char* k, double n) {
    SS(SetDoubleValue)
}
double FileSetting::GetDoubleValue(int id, const char* s, const char* k, double n) {
    SG(GetDoubleValue, double)
}
void FileSetting::SetBoolValue(int id, const char* s, const char* k, bool n) {
    SS(SetBoolValue)
}
bool FileSetting::GetBoolValue(int id, const char* s, const char* k, bool n) {
    SG(GetBoolValue, bool)
}


