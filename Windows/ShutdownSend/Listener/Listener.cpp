//	Listener.cpp - Sample application for CSerial
//
//	Copyright (C) 1999-2003 Ramon de Klein (Ramon.de.Klein@ict.nl)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#define STRICT
#include <tchar.h>
#include <windows.h>
//#include <WinSock2.h>
#include <stdio.h>
#include <queue>
#include <string.h>
#include "Serial.h"
#include "zini.h"
#include <process.h>  
#include "resource.h"
#include "Log.h"
#include "CSocketServer.h"
#include "Config.h"
#include "CHeartBeatClient.h"
#include "CSerialShutdown.h"

#pragma comment(lib, "ws2_32.lib")  
#pragma comment(lib, "user32.lib")  
#pragma comment(lib, "Shell32.lib")

CSocketServer g_socketShowdown;
CHeartBeatClient g_heartBeat;
CSerialShutdown g_serialShutdown;

//std::string TCHAR2STRING(TCHAR *STR)
//{
//	int iLen = WideCharToMultiByte(CP_ACP, 0, STR, -1, NULL, 0, NULL, NULL);
//	char* chRtn = new char[iLen * sizeof(char)];
//	WideCharToMultiByte(CP_ACP, 0, STR, -1, chRtn, iLen, NULL, NULL);
//	std::string str(chRtn);
//	return str;
//}
//
//int ShowError (LONG lError, LPCTSTR lptszMessage)
//{
//	// Generate a message text
//	//TCHAR tszMessage[256];
//	//wsprintf(tszMessage,_T("%s\n(error code %d)"), lptszMessage, lError);
//
//	// Display message-box and return with an error-code
//	//::MessageBox(0,tszMessage,_T("Listener"), MB_ICONSTOP|MB_OK);
//	printf("%s\n",TCHAR2STRING((TCHAR*)lptszMessage).c_str());
//	printf("%s(error code %d)\n", lError);
//	return 1;
//}


static VOID ShowNotifyIcon(HWND hWnd, BOOL addOrHide)
{
	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(nid));
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uID = 0;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	//nid.uCallbackMessage = WM_TRAYMESSAGE;
	nid.hIcon = LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(MAIN_ICON));
	lstrcpy(nid.szTip, TEXT("Click to maxmize!"));

	if (addOrHide)
	{
		Shell_NotifyIcon(NIM_ADD, &nid);
	}

	else
	{
		Shell_NotifyIcon(NIM_DELETE, &nid);
	}
};

void HideToTray(HWND hWnd)
{

	ShowWindow(hWnd, SW_HIDE);
	ShowWindow(hWnd, SW_HIDE);
	ShowNotifyIcon(hWnd, true);

	//在任务栏和atl+tab列表中隐藏：  
	//::ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);
}

void initWindow()
{
	HideToTray(GetConsoleWindow());
}

int __cdecl _tmain (int /*argc*/, char** /*argv*/)
{
    CSerial serial;
	LONG    lLastError = ERROR_SUCCESS;

	//初始化窗口，最小化到托盘，并设置图标
	initWindow();
	if (!Config::instance().init())
		return Log("init config failed");

	if(!g_socketShowdown.init())
		return Log("init shutdown server failed");

	if (!g_heartBeat.init())
		return Log("init heart beat failed");

	if (!g_serialShutdown.init())
		return Log("init  serial shutdown failed");

    return 0;
}
