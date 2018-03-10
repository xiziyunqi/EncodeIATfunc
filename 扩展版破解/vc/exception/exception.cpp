#include <stdio.h>  
#include "stdafx.h"  
#include "tchar.h"
int exceptFilter()  
{  
    printf("filter\n");  
  
    return 1;  
}  
  
int _tmain(int argc, _TCHAR* argv[])  
{  
    if(argc==1)  
    {  
        __try  
        {  
            *(int*)0=1;  
        }  
        __except(exceptFilter())  
        {}  
    }  
    printf("End\n");  
    return 0;  
}  