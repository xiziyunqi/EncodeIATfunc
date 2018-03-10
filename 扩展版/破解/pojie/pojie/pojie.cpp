#include "stdafx.h"
#include <Windows.h>
int _tmain(int argc, _TCHAR* argv[])
{
        _asm int 3//这个代码一定需要，这个代码调用时会加载调试器，用来在不同调试器中使程序断在我想要的位置
        while(1)
        {
        while(::MessageBox(0,L"是否记录到导出表?",L"Test",MB_YESNO)==IDYES)
        {
                return 0;
        }
        }
        return 0;
}
