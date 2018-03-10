#include <stdio.h>
#include<windows.h>
extern "C" _declspec(dllimport) int Min(int a, int b);
#pragma comment(lib,"dlltest.lib")
int main()
{
printf("error because of cout!\n");
 _asm int 3;

	int a=Min(3,4);
	a=1000;
	while(a--){printf("error because of cout!\n");}

 
 return 0;
}