#include "stdafx.h"
#include <Windows.h>
int _tmain(int argc, _TCHAR* argv[])
{
        _asm int 3//�������һ����Ҫ������������ʱ����ص������������ڲ�ͬ��������ʹ�����������Ҫ��λ��
        while(1)
        {
        while(::MessageBox(0,L"�Ƿ��¼��������?",L"Test",MB_YESNO)==IDYES)
        {
                return 0;
        }
        }
        return 0;
}
