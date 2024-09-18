#include "stdio.h"
#include "windows.h"

HANDLE g_hChild1Std_IN_Rd = NULL;
HANDLE g_hChild1Std_IN_Wr = NULL;
HANDLE g_hChild1Std_OUT_Rd = NULL;
HANDLE g_hChild1Std_OUT_Wr = NULL;

HANDLE g_hChild2Std_IN_Rd = NULL;
HANDLE g_hChild2Std_IN_Wr = NULL;
HANDLE g_hChild2Std_OUT_Rd = NULL;
HANDLE g_hChild2Std_OUT_Wr = NULL;

int CreateChildProcess(TCHAR *szCmdline[], HANDLE hStdIn, HANDLE hStdOut);

int main()
{
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    // Create pipes for child_1
    if (!CreatePipe(&g_hChild1Std_OUT_Rd, &g_hChild1Std_OUT_Wr, &saAttr, 0))
        return -1;
    if (!SetHandleInformation(g_hChild1Std_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
        return -1;
    if (!CreatePipe(&g_hChild1Std_IN_Rd, &g_hChild1Std_IN_Wr, &saAttr, 0))
        return -1;
    if (!SetHandleInformation(g_hChild1Std_IN_Wr, HANDLE_FLAG_INHERIT, 0))
        return -1;

    // Create pipes for child_2
    if (!CreatePipe(&g_hChild2Std_OUT_Rd, &g_hChild2Std_OUT_Wr, &saAttr, 0))
        return -1;
    if (!SetHandleInformation(g_hChild2Std_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
        return -1;
    if (!CreatePipe(&g_hChild2Std_IN_Rd, &g_hChild2Std_IN_Wr, &saAttr, 0))
        return -1;
    if (!SetHandleInformation(g_hChild2Std_IN_Wr, HANDLE_FLAG_INHERIT, 0))
        return -1;


    // Create child_1 process
    //TCHAR Cmd1[] = TEXT("child_1.exe");
    char *str1 = "child_1.exe";

    if (!CreateChildProcess(&str1, g_hChild1Std_IN_Rd, g_hChild1Std_OUT_Wr))
    {
        printf("child_1 Create error");
        return -1;
    }
        

    // Create child_2 process
    //TCHAR Cmd2[] = TEXT("child_2.exe");

    char *str2 = "child_1.exe";
    if (!CreateChildProcess(&str2, g_hChild2Std_IN_Rd, g_hChild2Std_OUT_Wr))
    {
        printf("child_2 Create error");
        return -1;
    }
    

    
    DWORD dwWritten, dwRead;
    char str[256];
    printf("Enter a string: ");
    fgets(str, sizeof(str), stdin);


    // Write to child_1
    WriteFile(g_hChild1Std_IN_Wr, &str, sizeof(str) + 1, &dwWritten, NULL);

   // Read from child_2
    char result[256];
    ReadFile(g_hChild2Std_OUT_Rd, &result, sizeof(result), &dwRead, NULL);

    printf("Result: %s", result);
    return 0;
}

int CreateChildProcess(TCHAR *szCmdline[], HANDLE  hStdIn, HANDLE hStdOut)
{

    PROCESS_INFORMATION piProcInfo;
    STARTUPINFO siStartInfo;
    BOOL bSuccess = FALSE;

    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdError = hStdOut;
    siStartInfo.hStdOutput = hStdOut;
    siStartInfo.hStdInput = hStdIn;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;


    // Create the child process. 

    bSuccess = CreateProcess(
        NULL,
        &szCmdline,
        NULL,
        NULL,
        TRUE,
        0,
        NULL,
        NULL,
        &siStartInfo,
        &piProcInfo
    );

    if (!bSuccess)
        return -1;
    else {
        CloseHandle(piProcInfo.hProcess);
        CloseHandle(piProcInfo.hThread);
    }
    return 0;
}