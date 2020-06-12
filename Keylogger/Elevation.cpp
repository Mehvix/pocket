#include <windows.h>
#include <minwindef.h>

bool IsRunAsAdministrator();
void ElevateNow()
{
    bool bAlreadyRunningAsAdministrator = false;
    try{
        bAlreadyRunningAsAdministrator = IsRunAsAdministrator();
    }
    catch(...){
        asm ("nop");
    }
    if(!bAlreadyRunningAsAdministrator){
        char szPath[MAX_PATH];
        if (GetModuleFileName(nullptr, szPath, ARRAYSIZE(szPath))){
            SHELLEXECUTEINFO sei = { sizeof(sei) };

            sei.lpVerb = "runas";
            sei.lpFile = szPath;
            sei.hwnd = nullptr;
            sei.nShow = SW_NORMAL;

            if (!ShellExecuteEx(&sei)){
                DWORD dwError = GetLastError();
                if (dwError == ERROR_CANCELLED)
                    CreateThread(nullptr,0,(LPTHREAD_START_ROUTINE)ElevateNow,nullptr,0,0);
            }
        }
    }
}