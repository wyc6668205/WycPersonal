// TestForPE.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>
#include <iostream>
#include <fstream>
#include <Psapi.h>

using namespace std;

#ifndef IMAGE_NT_HEADERS_SIZE
#ifdef _WIN64
#define IMAGE_NT_HEADERS_SIZE 112
#else
#define IMAGE_NT_HEADERS_SIZE 96
#endif
#endif // !IMAGE_NT_HEADERS_SIZE

IMAGE_DOS_HEADER GetDosHeader(char *pMap){
	// dos header 64 bytes 固定格式
	IMAGE_DOS_HEADER image_dos_header;
	memcpy(&image_dos_header, pMap, 64);
	
	return image_dos_header;
}

IMAGE_NT_HEADERS GetNtHeader(char *pMap){
	// NT header 32/64 (96/112) bytes 
	IMAGE_NT_HEADERS image_nt_header;
	memcpy(&image_nt_header, pMap, IMAGE_NT_HEADERS_SIZE + 128);

	return image_nt_header;
}

void Dll(){
	HMODULE hModule = LoadLibrary("ws2_32.dll");
	FARPROC proc = GetProcAddress(hModule, "WSAEventSelect");
	if (proc == NULL)
		return;
	else
		cout << "success" << endl;
}

int _tmain(int argc, _TCHAR* argv[])
{
	Dll();
	//GetProcAddress
	return 0;

 	ifstream file;
 	file.open("C:\\Windows\\winsxs\\wow64_microsoft-windows-user32_31bf3856ad364e35_6.1.7601.17514_none_35b31c02b85ccb6e\\user32.dll", ios::binary | ios::in);
 	char buff[1024] = { 0 };
 	if (!file.is_open())
 		return -1;
	// cout << (char)0x50 << (char)0x45 << endl;

 	file.read(buff, 64);
 	if (file.gcount() != 64)
 		return -2;
 	IMAGE_DOS_HEADER image_dos_header = GetDosHeader(buff);

	file.seekg(image_dos_header.e_lfanew, ios::beg);
	file.read(buff, 20 + IMAGE_NT_HEADERS_SIZE + 128);
	if (file.gcount() != 20 + IMAGE_NT_HEADERS_SIZE + 128)
		return -3;
	IMAGE_NT_HEADERS image_NT_header = GetNtHeader(buff);



	return 0;
}

