#pragma once
#include "PE.h"

class CPACK
{
public:
	CPACK();
	~CPACK();
public:
	//�ӿǴ���
	BOOL Pack(CString strFilePath, BOOL bSelect[5], CHAR MachineCode[16]);

	//�������ռӿǺ���ļ�
	BOOL SaveFinalFile(LPBYTE pFinalBuf, DWORD pFinalBufSize,CString strFilePath);		
};

