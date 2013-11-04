﻿#include "..\common.h"
#pragma GCC diagnostic ignored "-Wunused-parameter"

LIBRARY_EXPORT BOOL WINAPI WaffleCreateProcess(
    _In_opt_    LPCTSTR lpApplicationName,
    _Inout_opt_ LPTSTR lpCommandLine,
    _In_opt_    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    _In_opt_    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    _In_        BOOL bInheritHandles,
    _In_        DWORD dwCreationFlags,
    _In_opt_    LPVOID lpEnvironment,
    _In_opt_    LPCTSTR lpCurrentDirectory,
    _In_opt_    LPSTARTUPINFO lpStartupInfo,
    _Out_opt_   LPPROCESS_INFORMATION lpProcessInformation
    )
{
    STARTUPINFO stStartUp;
    PROCESS_INFORMATION stProcessInfo;

    if (!lpStartupInfo)
    {
        stStartUp.cb = sizeof(stStartUp);
        GetStartupInfo(&stStartUp);

        lpStartupInfo = &stStartUp;
    }
    if (!lpProcessInformation)
    {
        lpProcessInformation = &stProcessInfo;
    }

    PVOID OldValue = 0;
    WaffleDisableWow64FsRedirection(&OldValue);
    BOOL Result = CreateProcess(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
    DWORD LastError = GetLastError();
    WaffleRevertWow64FsRedirection(OldValue);

    SetLastError(LastError);
    return Result;
}

LIBRARY_EXPORT VOID WINAPI WaffleInjectDll(
    _In_    HANDLE hProcess,
    _In_    LPCTSTR lpszDllFull
    )
{
    LPVOID lpszRemoteDll = VirtualAllocEx(hProcess, NULL, MAX_PATH*sizeof(TCHAR), MEM_COMMIT, PAGE_READWRITE);
    if (lpszRemoteDll)
    {
        LPTHREAD_START_ROUTINE lpLoadLibrary = (LPTHREAD_START_ROUTINE) WaffleGetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), TEXT("LoadLibraryW"));

        WriteProcessMemory(hProcess, lpszRemoteDll, lpszDllFull, lstrlen(lpszDllFull)*sizeof(TCHAR), NULL);
        HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, lpLoadLibrary, lpszRemoteDll, 0, NULL);
        WaitForSingleObject(hThread, INFINITE);

        VirtualFreeEx(hProcess, lpszRemoteDll, 0, MEM_RELEASE);
        CloseHandle(hThread);
    }
}

LIBRARY_EXPORT VOID WINAPI WaffleExecute(
    _Out_opt_   LPWAFFLE_PROCESS_SETTING lpstProcessSetting,
    _In_opt_    LPCTSTR lpApplicationName,
    _Inout_opt_ LPTSTR lpCommandLine,
    _In_opt_    LPCTSTR lpCurrentDirectory
    )
{
    PROCESS_INFORMATION stProcessInfo;

    WaffleCreateProcess(lpApplicationName, lpCommandLine, NULL, NULL, TRUE, CREATE_SUSPENDED, 0, lpCurrentDirectory, NULL, &stProcessInfo);

    /*
#if	defined(WAFFLE_PORT_ENTRY_POINT)    //the workaround for xp, will cause problem with .net program under win 7
    CONTEXT stContext;
    stContext.ContextFlags = CONTEXT_FULL;
    GetThreadContext(stProcessInfo.hThread, &stContext);
    LPBYTE lpEntryPoint = (LPBYTE) stContext.WAFFLE_PORT_ENTRY_POINT;

    BYTE BackupEntryPoint[WAFFLE_PORT_ENTRY_POINT_LOOP_SIZE];
    ReadProcessMemory(stProcessInfo.hProcess, lpEntryPoint, BackupEntryPoint, WAFFLE_PORT_ENTRY_POINT_LOOP_SIZE, NULL);
    WriteProcessMemory(stProcessInfo.hProcess, lpEntryPoint, WAFFLE_PORT_ENTRY_POINT_LOOP, WAFFLE_PORT_ENTRY_POINT_LOOP_SIZE, NULL);
    FlushInstructionCache(stProcessInfo.hProcess, lpEntryPoint, WAFFLE_PORT_ENTRY_POINT_LOOP_SIZE);

    ResumeThread(stProcessInfo.hThread);

    for (;;)
    {
        Sleep(100);
        SuspendThread(stProcessInfo.hThread);
        GetThreadContext(stProcessInfo.hThread, &stContext);
        if ((LPBYTE) stContext.WAFFLE_PORT_PROGRAM_POINTER == lpEntryPoint)
        {
            WriteProcessMemory(stProcessInfo.hProcess, lpEntryPoint, BackupEntryPoint, WAFFLE_PORT_ENTRY_POINT_LOOP_SIZE, NULL);
            FlushInstructionCache(stProcessInfo.hProcess, lpEntryPoint, WAFFLE_PORT_ENTRY_POINT_LOOP_SIZE);
            break;
        }
        else
        {
            ResumeThread(stProcessInfo.hThread);
        }
    }
#endif
    */

    lpstProcessSetting->dwThreadId = stProcessInfo.dwThreadId;
    lpstProcessSetting->dwProcessId = stProcessInfo.dwProcessId;
    WaffleGetFileHash(lpApplicationName, lpstProcessSetting->szProcessHash);

    TCHAR szWaffleCommonDll[MAX_PATH];
    wsprintf(szWaffleCommonDll, TEXT("%s\\Waffle\\%s\\Waffle.common.1.0.dll"), lpstProcessSetting->szComponentDirectory, WAFFLE_PORT_MACHINE_STRING);
    WaffleInjectDll(stProcessInfo.hProcess, szWaffleCommonDll);

    CloseHandle(stProcessInfo.hThread);
    CloseHandle(stProcessInfo.hProcess);
}