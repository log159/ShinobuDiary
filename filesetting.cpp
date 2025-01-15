#include "filesetting.h"

#define SS(SET_FUNCTION) CSimpleIniA ini;\
static char ss[DEFSIZE4];\
memset(ss, sizeof(ss), 0);\
snprintf(ss, sizeof(ss), s, id);\
ini.SetUnicode(true);\
if(ini.LoadFile(INITFILE) < 0)\
printf("New File!\n");\
ini.SET_FUNCTION(ss, k, n);\
ini.SaveFile(INITFILE);

#define SG(GET_FUNCTION,RETURN_TYPE) CSimpleIniA ini;\
static char ss[DEFSIZE4];\
memset(ss, sizeof(ss), 0);\
snprintf(ss, sizeof(ss), s, id);\
ini.SetUnicode(true);\
if (ini.LoadFile(INITFILE) < 0)\
return RETURN_TYPE(n);\
return RETURN_TYPE(ini.GET_FUNCTION(ss, k, n));


//using SI_Error = int;
//constexpr int SI_OK = 0;        //!< No error
//constexpr int SI_UPDATED = 1;   //!< An existing value was updated
//constexpr int SI_INSERTED = 2;  //!< A new value was inserted
//// note: test for any error with (retval < 0)
//constexpr int SI_FAIL = -1;     //!< Generic failure
//constexpr int SI_NOMEM = -2;    //!< Out of memory error
//constexpr int SI_FILE = -3;     //!< File error (see errno for detail error)


void FileSetting::S_SetValue(int id, const char* s, const char* k, const char* n){
    SS(SetValue);
}
std::string FileSetting::S_GetValue(int id, const char* s, const char* k, const char* n){
    SG(GetValue,std::string)
}
void FileSetting::S_SetLongValue(int id, const char* s, const char* k, int n){
    SS(SetLongValue)
}
int FileSetting::S_GetLongValue(int id, const char* s, const char* k, int n){
    SG(GetLongValue,int)
}
void FileSetting::S_SetDoubleValue(int id, const char* s, const char* k, double n){
    SS(SetDoubleValue)
}
double FileSetting::S_GetDoubleValue(int id, const char* s, const char* k, double n){
    SG(GetDoubleValue,double)
}
void FileSetting::S_SetBoolValue(int id, const char* s, const char* k, bool n){
    SS(SetBoolValue)
}
bool FileSetting::S_GetBoolValue(int id, const char* s, const char* k, bool n){
    SG(GetBoolValue, bool)
}

