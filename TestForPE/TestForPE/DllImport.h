#include <Windows.h> 
#include <string>

using std::string;

DWORD getProcessHandle(LPCTSTR lpProcessName);//根据进程名查找进程PID 
INT DllImport(std::string proName);