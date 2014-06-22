#include "stdafx.h"
#include <tchar.h>
#include <stdio.h>

#include <string.h>
#include <conio.h>


#include <windows.h>
#include <wininet.h>
#pragma comment(lib,"wininet.lib")

#include <iostream>
using namespace std;

//Support printing bool value
#define BOOL_FMT(bool_expr) "%s=%s\n", #bool_expr, (bool_expr) ? "true" : "false"



BOOL SetConnectionProxy(char * proxyAdressStr, char * connNameStr = NULL)
{
	INTERNET_PER_CONN_OPTION_LIST conn_options;
	BOOL    bReturn;

	DWORD   dwBufferSize = sizeof(conn_options);
	conn_options.dwSize = dwBufferSize;

	conn_options.pszConnection = connNameStr;

	conn_options.dwOptionCount = 3;
	conn_options.pOptions = new INTERNET_PER_CONN_OPTION[3];

	if (!conn_options.pOptions)
		return FALSE;

	conn_options.pOptions[0].dwOption = INTERNET_PER_CONN_FLAGS;
	conn_options.pOptions[0].Value.dwValue = PROXY_TYPE_DIRECT | PROXY_TYPE_PROXY;

	conn_options.pOptions[1].dwOption = INTERNET_PER_CONN_PROXY_SERVER;
	conn_options.pOptions[1].Value.pszValue = proxyAdressStr;

	conn_options.pOptions[2].dwOption = INTERNET_PER_CONN_PROXY_BYPASS;
	conn_options.pOptions[2].Value.pszValue = "local";

	bReturn = InternetSetOption(NULL, INTERNET_OPTION_PER_CONNECTION_OPTION, &conn_options, dwBufferSize);

	printf(BOOL_FMT(bReturn));

	delete[] conn_options.pOptions;

	InternetSetOption(NULL, INTERNET_OPTION_SETTINGS_CHANGED, NULL, 0);
	InternetSetOption(NULL, INTERNET_OPTION_REFRESH, NULL, 0);
	return bReturn;
}

//Check if the bypass proxy list has been set or not.
bool IsBypassLocalServer()
{
	bool res = false;

	INTERNET_PER_CONN_OPTION_LIST list;
	memset(&list, 0, sizeof(list));
	DWORD dwSize = sizeof(list);

	INTERNET_PER_CONN_OPTION entry;
	memset(&entry, 0, sizeof(entry));
	entry.dwOption = INTERNET_PER_CONN_PROXY_BYPASS;

	list.dwSize = sizeof(list);
	list.dwOptionCount = 1;
	list.pOptions = &entry;

	if (InternetQueryOption(NULL, INTERNET_OPTION_PER_CONNECTION_OPTION, &list, &dwSize))
	{
		if (entry.Value.pszValue != NULL)
		{
			res = strstr(entry.Value.pszValue, _T("local")) != NULL;
			GlobalFree(entry.Value.pszValue);
			entry.Value.pszValue = NULL;
		}
	}
	return res;
}

BOOL RemoveConnectionProxy(char* connectionNameStr = NULL)
{
	INTERNET_PER_CONN_OPTION_LIST conn_options;
	BOOL    bReturn;
	DWORD   dwBufferSize = sizeof(conn_options);

	conn_options.dwSize = dwBufferSize;

	conn_options.pszConnection = connectionNameStr;
	conn_options.dwOptionCount = 1;

	conn_options.pOptions = new INTERNET_PER_CONN_OPTION[conn_options.dwOptionCount];

	if (!conn_options.pOptions)
		return FALSE;

	conn_options.pOptions[0].dwOption = INTERNET_PER_CONN_FLAGS;
	conn_options.pOptions[0].Value.dwValue = PROXY_TYPE_DIRECT;

	bReturn = InternetSetOption(NULL, INTERNET_OPTION_PER_CONNECTION_OPTION, &conn_options, dwBufferSize);

	delete[] conn_options.pOptions;
	InternetSetOption(NULL, INTERNET_OPTION_SETTINGS_CHANGED, NULL, 0);
	InternetSetOption(NULL, INTERNET_OPTION_REFRESH, NULL, 0);

	return bReturn;
}

//e.g. C:\>SetProxy.exe # No proxy settings "Direct Access"
//	   C:\SetProxy.exe 221.22.3.2:8080 #The new argument will be set as the proxy address & port number
int _tmain(int argc, _TCHAR* argv[])
{
	if (argc <= 1)
	{
		RemoveConnectionProxy();
		printf("No proxy.\r\n");
		exit(1);
	}
	char *pFilename = argv[1];
	SetConnectionProxy(pFilename);
	bool res = IsBypassLocalServer();
	printf(BOOL_FMT(res));

	return 0;
}

