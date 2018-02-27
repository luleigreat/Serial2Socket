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
#include "DataQueue.h"
#include "Log.h"
#include "CSocketSwitch.h"
#include "CSocketServer.h"
#include "CSerial2Socket.h"
#include "Config.h"
#include "CHeartBeatClient.h"
#include "CSerialShutdown.h"

#pragma comment(lib, "ws2_32.lib")  
#pragma comment(lib, "user32.lib")  
#pragma comment(lib, "Shell32.lib")

CDataQueue g_queue;
CSocketSwitch g_socketSwitch;
CSerial2Socket g_Serial;
bool g_socketInited = true;

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

UINT WINAPI reconnectThread(void* pParam)
{
	while (!g_socketInited)
	{
		Sleep(Config::instance().getTimeConfig().reconnect_gap);

		if (!g_socketInited && g_socketSwitch.init(g_queue))
		{
			Log("socket switch init success!");
			g_socketInited = true;
		}
	}
	return 0;
}

int __cdecl _tmain (int /*argc*/, char** /*argv*/)
{
    CSerial serial;
	LONG    lLastError = ERROR_SUCCESS;

	//初始化窗口，最小化到托盘，并设置图标
	initWindow();
	if (!Config::instance().init())
		return Log("init config failed");

	//初始化串口转Socket服务
	if (!g_socketSwitch.init(g_queue))
	{
		g_socketInited = false;
		Log("init socket switch failed");
	}
	else
	{
		Log("socket switch init success!");
	}

	if (!g_socketInited)
	{
		/** 线程ID */
		UINT threadId;
		/** 开启串口数据监听线程 */
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, reconnectThread, NULL, 0, &threadId);
		if (!hThread)
		{
			Log("Failed to create thread!");
		}
		::CloseHandle(hThread);
	}

	if (!g_Serial.init(g_queue))
		return Log("init serial failed");

    return 0;
}
