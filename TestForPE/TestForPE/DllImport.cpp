#include "stdafx.h"
#include "DllImport.h"
#include <stdio.h> 
#include <iostream>
#include <TlHelp32.h> 

using namespace std;

bool AdjustPrivileges() {
	HANDLE hToken;
	TOKEN_PRIVILEGES tp;
	TOKEN_PRIVILEGES oldtp;
	DWORD dwSize = sizeof(TOKEN_PRIVILEGES);
	LUID luid;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		if (GetLastError() == ERROR_CALL_NOT_IMPLEMENTED) return true;
		else return false;
	}
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid)) {
		CloseHandle(hToken);
		return false;
	}
	ZeroMemory(&tp, sizeof(tp));
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	/* Adjust Token Privileges */
	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), &oldtp, &dwSize)) {
		CloseHandle(hToken);
		return false;
	}
	// close handles
	CloseHandle(hToken);
	return true;
}

DWORD getProcessHandle(LPCTSTR lpProcessName)//根据进程名查找进程PID 
{
	DWORD dwRet = 0;
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapShot == INVALID_HANDLE_VALUE)
	{
		printf("\n获得进程快照失败%d", GetLastError());
		return dwRet;
	}

	PROCESSENTRY32 pe32;//声明进程入口对象 
	pe32.dwSize = sizeof(PROCESSENTRY32);//填充进程入口对象大小 
	Process32First(hSnapShot, &pe32);//遍历进程列表 
	do
	{
		if (!lstrcmp(pe32.szExeFile, lpProcessName))//查找指定进程名的PID 
		{
			dwRet = pe32.th32ProcessID;
			break;
		}
	} while (Process32Next(hSnapShot, &pe32));
	CloseHandle(hSnapShot);
	return dwRet;//返回 
}

INT DllImport(std::string proName)
{
	AdjustPrivileges();
	DWORD dwPid = getProcessHandle(proName.c_str());
	//LPCSTR lpDllName = "C:\\Windows\\System32\\ws2_32.dll";//"ws2_32.dll";
	LPCSTR lpDllName = "d:/JS32.dll";
	HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, dwPid);
	if (hProcess == NULL)
	{
		printf("\n获取进程句柄错误%d", GetLastError());
		return -1;
	}

	DWORD dwSize = strlen(lpDllName) ;
	DWORD dwHasWrite;
	LPVOID lpRemoteBuf = VirtualAllocEx(hProcess, NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);
	if (WriteProcessMemory(hProcess, lpRemoteBuf, lpDllName, dwSize, &dwHasWrite))
	{
		if (dwHasWrite != dwSize)
		{
			VirtualFreeEx(hProcess, lpRemoteBuf, dwSize, MEM_COMMIT);
			CloseHandle(hProcess);
			return -1;
		}

	}
	else
	{
		printf("\n写入远程进程内存空间出错%d。", GetLastError());
		CloseHandle(hProcess);
		return -1;
	}

	DWORD dwNewThreadId;
	LPVOID lpLoadDll = LoadLibraryA;
	if (!lpLoadDll){
		printf("\n获取dll失败%d", GetLastError());
		CloseHandle(hProcess);
		return -1;
	}
	HANDLE hNewRemoteThread = CreateRemoteThreadEx(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)lpLoadDll, lpRemoteBuf, 0, NULL, &dwNewThreadId);
	if (hNewRemoteThread == NULL)
	{
		printf("\n建立远程线程失败%d", GetLastError());
		CloseHandle(hProcess);
		return -1;
	}

	WaitForSingleObject(hNewRemoteThread, INFINITE);
	CloseHandle(hNewRemoteThread);

	//准备卸载之前注入的Dll 
	DWORD dwHandle, dwID;
	LPVOID pFunc = GetModuleHandleA;//获得在远程线程中被注入的Dll的句柄 
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pFunc, lpRemoteBuf, 0, &dwID);
	WaitForSingleObject(hThread, INFINITE);
	GetExitCodeThread(hThread, &dwHandle);//线程的结束码即为Dll模块儿的句柄 
	CloseHandle(hThread);
	pFunc = FreeLibrary;
	hThread = CreateRemoteThread(hThread, NULL, 0, (LPTHREAD_START_ROUTINE)pFunc, (LPVOID)dwHandle, 0, &dwID); //将FreeLibraryA注入到远程线程中去卸载Dll 
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	CloseHandle(hProcess);
	return 0;
}
