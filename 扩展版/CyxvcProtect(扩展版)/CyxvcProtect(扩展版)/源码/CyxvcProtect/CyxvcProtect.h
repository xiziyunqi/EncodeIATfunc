
// CyxvcProtect.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CCyxvcProtectApp: 
// �йش����ʵ�֣������ CyxvcProtect.cpp
//

class CCyxvcProtectApp : public CWinApp
{
public:
	CCyxvcProtectApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CCyxvcProtectApp theApp;