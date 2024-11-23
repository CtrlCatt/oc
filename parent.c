// CreateProcess.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "stdio.h"
#include "windows.h"

HANDLE g_hChildStd_IN_Rd = NULL; // child_1
HANDLE g_hChildStd_IN_Wr = NULL; //parent
HANDLE g_hChildStd_OUT_Rd_1 = NULL; //
HANDLE g_hChildStd_OUT_Wr_1 = NULL;


HANDLE g_hChildStd_OUT_Rd_2 = NULL; // parent
HANDLE g_hChildStd_OUT_Wr_2 = NULL; // child_2



int CreateChildProcess();
int CreateChild_2();

int main()
{
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;
    
    if (!CreatePipe(&g_hChildStd_OUT_Rd_1, &g_hChildStd_OUT_Wr_1, &saAttr, 0))
        return -1;
    
    if (!CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0))
        return -1;

    if (!CreatePipe(&g_hChildStd_OUT_Rd_2, &g_hChildStd_OUT_Wr_2, &saAttr, 0))
        return -1;

    if (!SetHandleInformation(g_hChildStd_OUT_Rd_2, HANDLE_FLAG_INHERIT, 0))
        return -1;
    
    if (!SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0))         
        return -1;

    if (!CreateChildProcess())
        return -1;

    if (!CreateChild_2())
        return -1;

    DWORD dwWritten, dwRead;

    char str[256];
    printf("Write string: ");
    fgets(str, sizeof(str), stdin);
    WriteFile(g_hChildStd_IN_Wr, &str, sizeof(str), &dwWritten, NULL);

    char result[256];
    ReadFile(g_hChildStd_OUT_Rd_2, &result, sizeof(result), &dwRead, NULL);
    printf("Resalt: %s", result);

    CloseHandle(g_hChildStd_OUT_Rd_2);
    CloseHandle(g_hChildStd_IN_Wr);
    return 0;
}

int CreateChildProcess()
{
    TCHAR szCmdline[] = TEXT("child.exe");
    PROCESS_INFORMATION piProcInfo;
    STARTUPINFO siStartInfo;
    BOOL bSuccess = FALSE;

    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

    // Set up members of the STARTUPINFO structure. 
    // This structure specifies the STDIN and STDOUT handles for redirection.

    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdError = g_hChildStd_OUT_Wr_1;
    siStartInfo.hStdOutput = g_hChildStd_OUT_Wr_1;
    siStartInfo.hStdInput = g_hChildStd_IN_Rd;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    // Create the child process. 

    bSuccess = CreateProcess(NULL,
        szCmdline,     // command line 
        NULL,           
        NULL,          
        TRUE,           
        0,              
        NULL,           
        NULL,           
        &siStartInfo,  // STARTUPINFO pointer 
        &piProcInfo);  // receives PROCESS_INFORMATION 

     // If an error occurs, exit the application. 
    if (!bSuccess)
        return -1;
    else
    {
        CloseHandle(piProcInfo.hProcess);
        CloseHandle(piProcInfo.hThread);

        CloseHandle(g_hChildStd_OUT_Wr_1);
        CloseHandle(g_hChildStd_IN_Rd);
    }
}

int CreateChild_2()
{
    TCHAR szCmdline[] = TEXT("child_2.exe");
    PROCESS_INFORMATION piProcInfo;
    STARTUPINFO siStartInfo;
    BOOL bSuccess = FALSE;

    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

    // Set up members of the STARTUPINFO structure. 
    // This structure specifies the STDIN and STDOUT handles for redirection.

    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdError = g_hChildStd_OUT_Wr_2;
    siStartInfo.hStdOutput = g_hChildStd_OUT_Wr_2;
    siStartInfo.hStdInput = g_hChildStd_OUT_Rd_1;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    // Create the child process. 

    bSuccess = CreateProcess(NULL,
        szCmdline,     // command line 
        NULL,          // process security attributes 
        NULL,          // primary thread security attributes 
        TRUE,          // handles are inherited 
        0,             // creation flags 
        NULL,          // use parent's environment 
        NULL,          // use parent's current directory 
        &siStartInfo,  // STARTUPINFO pointer 
        &piProcInfo);  // receives PROCESS_INFORMATION 

     // If an error occurs, exit the application. 
    if (!bSuccess)
        return -1;
    else
    {
        CloseHandle(piProcInfo.hProcess);
        CloseHandle(piProcInfo.hThread);

        CloseHandle(g_hChildStd_OUT_Wr_2);
        CloseHandle(g_hChildStd_OUT_Rd_1);
    }
}
