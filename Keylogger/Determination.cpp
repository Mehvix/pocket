#include <minwindef.h>
#include <windows.h>

using namespace std;

bool IsRunAsAdministrator()
{
    bool fIsRunAsAdmin = false;
    DWORD dwError = ERROR_SUCCESS;
    PSID pAdministratorsGroup = nullptr;

    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    if (!AllocateAndInitializeSid(
            &NtAuthority,
            2,
            SECURITY_BUILTIN_DOMAIN_RID,
            DOMAIN_ALIAS_RID_ADMINS,
            0, 0, 0, 0, 0, 0,
            &pAdministratorsGroup)) {
        dwError = GetLastError();
        goto Cleanup;
    }

    if (!CheckTokenMembership(nullptr, pAdministratorsGroup, reinterpret_cast<PBOOL>(&fIsRunAsAdmin))){
        dwError = GetLastError();
        goto Cleanup;
    }

    Cleanup:

    if (pAdministratorsGroup){
        FreeSid(pAdministratorsGroup);
        pAdministratorsGroup = nullptr;
    }

    if (ERROR_SUCCESS != dwError){
        throw dwError;
    }

    return fIsRunAsAdmin;
}