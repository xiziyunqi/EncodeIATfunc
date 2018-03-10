// arrayaddrandfirstobject.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<iostream>
#include <WTypes.h>  
#include <tchar.h>
using namespace std;
typedef unsigned long          *PDWORD;
typedef unsigned char BYTE;
typedef BYTE near           *PBYTE;
	PBYTE sAddr;
	void Fun1()
	{
BYTE byByte[]= {
			0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x77};
			PBYTE pNewAddr = (PBYTE)VirtualAlloc(0, sizeof(byByte), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
			memcpy(pNewAddr, byByte, sizeof(byByte));
			//#new¿Õ¼ä1(DWORD)pNewAddr;
			//#new¿Õ¼ä2g_pTRUEIAT[i]£»
			//sAddr = pNewAddr;
			sAddr = byByte;
	}
int main(int argc, char* argv[])
{

	int a=1,b=2;
	int array={a};
	cout<<a<<endl;
	cout<<array<<endl;
Fun1();
	cout<<sAddr;
	return 0;
}

