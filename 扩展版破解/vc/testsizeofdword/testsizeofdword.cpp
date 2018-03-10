// testsizeofdword.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
typedef unsigned long       DWORD;
int main(int argc, char* argv[])
{
	DWORD dwTemp1;
	DWORD dwTemp2;
	DWORD dwTemp3;

	dwTemp1 = sizeof(DWORD)+3;
	dwTemp2 = sizeof(DWORD)-1;
	dwTemp3 = sizeof(DWORD)*sizeof(DWORD)-dwTemp2;
	printf("%d %d %d",dwTemp1,dwTemp2,dwTemp3);
	//#7 3 13
	return 0;
}

