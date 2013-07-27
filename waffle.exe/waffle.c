﻿#define  UNICODE
#define  _UNICODE
#include "waffle.h"
//#include <shellapi.h>
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

VOID WINAPI Main()
{
    //Get base directory
    TCHAR szPath[MAX_PATH];
    WaffleGetModuleDirectory(NULL, szPath, sizeof(szPath) / sizeof(szPath[0]));

    //Pickup target
    int nArg = WaffleArgc();
    TCHAR szPlugin[MAX_PATH];
    TCHAR szTarget[MAX_PATH];
    TCHAR szDirectory[MAX_PATH];
    if (nArg >= 3)
    {
        //1.文件名 2.插件名 3.目标
        WaffleArgv(2, szPlugin, sizeof(szPlugin));

        WaffleArgv(3, szTarget, sizeof(szTarget));
    }
    else
    {
        OPENFILENAME stOpenFile;
        RtlZeroMemory(&stOpenFile, sizeof(stOpenFile));
        RtlZeroMemory(szTarget, sizeof(szTarget));

        stOpenFile.lStructSize = sizeof(stOpenFile);
        stOpenFile.lpstrFile = szTarget;
        stOpenFile.nMaxFile = sizeof(szTarget) / sizeof(TCHAR);
        stOpenFile.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
        GetOpenFileName(&stOpenFile);
        
        lstrcpy(szPlugin, TEXT("Mojibake"));
    }

    lstrcpy(szDirectory, szTarget);
    int i = lstrlen(szTarget);
    for (; szDirectory[i] != '\\'; i--);
    szDirectory[i] = '\0';

    WORD MachineType = WaffleGetMachineType(szTarget);
    if (MachineType == WAFFLE_PORT_MACHINE)
    {
        LPWAFFLE_PROCESS_SETTING lpstProcessSetting = WaffleCreateProcessSetting();
        lstrcpy(lpstProcessSetting->szPlugin, szPlugin);

        HGLOBAL lpszArgument = GlobalAlloc(GPTR, (lstrlen(szTarget) + lstrlen(WaffleArgp(4)) + 3 + 1) * sizeof(TCHAR));
        wsprintf(lpszArgument, TEXT("\"%s\" %s"), szTarget, WaffleArgp(4));
        WaffleExecute(szTarget, lpszArgument, szDirectory, lpstProcessSetting);
        GlobalFree(lpszArgument);
    }
    else
    {
        TCHAR szLoader[MAX_PATH];

        STARTUPINFO stStartUp;
        PROCESS_INFORMATION stProcessInfo;

        stStartUp.cb = sizeof(stStartUp);
        GetStartupInfo(&stStartUp);

        switch (MachineType)
        {
        case WAFFLE_PORT_MACHINE_I386:
            {
                wsprintf(szLoader, TEXT("%s%s"), szPath, TEXT("\\..\\I386\\Waffle.exe"));
                break;
            }
        case WAFFLE_PORT_MACHINE_AMD64:
            {
                wsprintf(szLoader, TEXT("%s%s"), szPath, TEXT("\\..\\AMD64\\Waffle.exe"));
                break;
            }
        case 0xFFFF:
            {
                MessageBox(0, TEXT("FIXME:Unable to open the target"), 0, 0);
                ExitProcess(0);
                break;
            }
        default:
            {
                MessageBox(0, TEXT("FIXME:Unsupported file or .net program"), 0, 0);       //Could be .net program
                ExitProcess(0);
            }
        }

        HGLOBAL lpszArgument;
        if (nArg >= 3)
        {
            lpszArgument = GlobalAlloc(GPTR, (lstrlen(szLoader) + lstrlen(WaffleArgp(2)) + 3 + 1) * sizeof(TCHAR));
            wsprintf(lpszArgument, TEXT("\"%s\" %s"), szLoader, WaffleArgp(2));
        }
        else
        {
            lpszArgument = GlobalAlloc(GPTR, (lstrlen(szLoader) + lstrlen(szPlugin) + lstrlen(szTarget) + 6 + 1) * sizeof(TCHAR));
            wsprintf(lpszArgument, TEXT("\"%s\" %s \"%s\""), szLoader, szPlugin, szTarget);
        }
        CreateProcess(szLoader, lpszArgument, NULL, NULL, TRUE, 0, 0, szDirectory, &stStartUp, &stProcessInfo);
        GlobalFree(lpszArgument);
    }

    ExitProcess(0);
    return;
}