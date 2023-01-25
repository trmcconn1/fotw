#include <windows.h>
#include "stdafx.h"

extern HWND dlghandle; // handle to main dialog window


// Canned Fatal Error routine
void ErrorExit(LPTSTR lpszFunction) 
{ 
    TCHAR szBuf[256]; 
    LPVOID lpMsgBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    wsprintf(szBuf, 
        (LPCWSTR)L"%s. Last system error = %d: %s", 
        (LPCWSTR)lpszFunction, dw, lpMsgBuf); 
 
    MessageBox(NULL, szBuf, L"He's dead, Jim", MB_OK); 

    LocalFree(lpMsgBuf);
    ExitProcess(dw); 
}


void ErrorNonFatal(LPTSTR lpszFunction) 
{ 
    TCHAR szBuf[256]; 
   

    wsprintf(szBuf, 
        (LPCWSTR)L"%s", 
        (LPCWSTR)lpszFunction); 
 
    MessageBox(dlghandle, szBuf, L"Non Fatal Error", MB_OK); 
    
}