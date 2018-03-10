#include "stdafx.h"
#include "PACK.h"
#include <psapi.h>
#include "../Shell/Shell.h"
#pragma comment(lib,"../Debug/Shell.lib")

CPACK::CPACK()
{
}


CPACK::~CPACK()
{
}

//************************************************************
// ��������:	Pack
// ����˵��:	�ӿ�
// ��	��:	cyxvc
// ʱ	��:	2015/12/25
// ��	��:	CString strFilePath
// �� ��	ֵ:	BOOL
//************************************************************
BOOL CPACK::Pack(CString strFilePath, BOOL bSelect[5], CHAR MachineCode[16])
{	//#0.MFC�����������˻�����
	//1.��ȡ�ļ�PE�ļ���Ϣ������
	CPE objPE;
	if (objPE.InitPE(strFilePath) == FALSE)
		return FALSE;

	//2.0.���ܲ���
	//2.1.IAT����
	if (bSelect[1] == TRUE)
	{
		//2.1.1.����IAT(�����)��Ϣ
		objPE.SaveImportTab();
		//2.1.2.ĨȥԭʼIAT(�����)
		objPE.ClsImportTab();
	}


	//2.2.���ܴ����
	if (bSelect[0] == TRUE)
		objPE.XorCode(0x15);

	//2.2.3�������
	if (bSelect[2] == TRUE)
		objPE.XorMachineCode(MachineCode);


	//3.����Ҫ����Ϣ���浽Shell
	HMODULE hShell = LoadLibrary(L"../Debug/Shell.dll");
	if (hShell == NULL)
	{
		MessageBox(NULL, _T("����Shell.dllģ��ʧ�ܣ���ȷ������������ԣ�"), _T("��ʾ"), MB_OK);
		//�ͷ���Դ
		delete[] objPE.m_pFileBuf;
		return FALSE;
	}
	//#������Ҫ��Ϣ���Զ���ṹ��PSHELL_DATA
	PSHELL_DATA pstcShellData = (PSHELL_DATA)GetProcAddress(hShell, "g_stcShellData");
	//#
	pstcShellData->dwXorKey = 0x15;
	pstcShellData->dwCodeBase = objPE.m_dwCodeBase;
	pstcShellData->dwXorSize = objPE.m_dwCodeSize;
	pstcShellData->dwPEOEP = objPE.m_dwPEOEP;
	pstcShellData->dwPEImageBase = objPE.m_dwImageBase;
	pstcShellData->stcPERelocDir = objPE.m_PERelocDir;
	pstcShellData->stcPEImportDir = objPE.m_PEImportDir;
	pstcShellData->dwIATSectionBase = objPE.m_IATSectionBase;
	pstcShellData->dwIATSectionSize = objPE.m_IATSectionSize;

	pstcShellData->dwNumOfIATFuns = objPE.m_dwNumOfIATFuns;
	pstcShellData->dwSizeOfModBuf = objPE.m_dwSizeOfModBuf;
	pstcShellData->dwSizeOfFunBuf = objPE.m_dwSizeOfFunBuf;

	pstcShellData->bSelect[0] = bSelect[0];
	pstcShellData->bSelect[1] = bSelect[1];
	pstcShellData->bSelect[2] = bSelect[2];
	pstcShellData->bSelect[3] = bSelect[3];
	pstcShellData->bSelect[4] = bSelect[4];

	//4.��Shell���ӵ�PE�ļ�
	//4.1.��ȡShell����
	MODULEINFO modinfo = { 0 };
	/*##��2������ģ��Ľ��̵�ģ�����Ϣ��ģ���ַ����С�����
	typedef struct _MODULEINFO {
	LPVOID lpBaseOfDll;
	DWORD SizeOfImage;
	LPVOID EntryPoint;
	} MODULEINFO, *LPMODULEINFO;
	*/

	GetModuleInformation(GetCurrentProcess(), hShell, &modinfo, sizeof(MODULEINFO));
	//#��1����ȡģ����Ϣ��������MODULEINFO
	/*##
	hProcess
	ָ�����ģ��Ľ��̵ľ����������һ��Ҫ�� PROCESS_QUERY_INFORMATION �� PROCESS_VM_READ Ȩ�ޡ��ο� Process Security and Access Rights[3]
	hModule
	ָ��ģ��ľ����
	lpmodinfo
	ָ�� MODULEINFO �ṹ��ָ�룬�ýṹ���ᴢ�����ģ�����Ϣ��
	cb
	�ṹ MODULEINFO  �Ĵ�С�����ֽڼ��㣩��
	*/
	//#
	//-----------------------------------------------------	
	pstcShellData->dwIATBaseRVA = objPE.m_dwImageSize + modinfo.SizeOfImage;
	//#��3��ΪʲôIAT��ַ�Ǿ����С+dll�����С
	//-----------------------------------------------------
	PBYTE  pShellBuf = new BYTE[modinfo.SizeOfImage];
	/*##
	֮ǰ�Ѿ�ͨ��LoadLibrary(L"Shell.dll")�ķ�ʽ������Shellģ�飬��Ϊ�˲��������һ���������
	һ��ռ�ר�Ŵ��Shell���֣�ͬʱ��ȡһ��Shell�ľ����С��Ϊ����������׼����
	##��ʵ�Ƕ��һ�ٰɣ�����loadlibrary���ڴ����
	*/
	memcpy_s(pShellBuf, modinfo.SizeOfImage, hShell, modinfo.SizeOfImage);
	//4.2.����Shell�ض�λ��Ϣ
	objPE.SetShellReloc(pShellBuf, (DWORD)hShell);
	//#1.��ԭ��loadlibrary����dll���е��ض�λ��2.��pe�ļ����ض�λָ��dll��
	//#�ض�λĿ¼���ǵ�ָ��ƫ��Ҫ����pe�ļ��ľ����С��
	//4.3.�޸ı��ӿǳ����OEP��ָ��Shell
	DWORD dwShellOEP = pstcShellData->dwStartFun - (DWORD)hShell;
	objPE.SetNewOEP(dwShellOEP);
	//4.4.�ϲ�PE�ļ���Shell�Ĵ��뵽�µĻ�����
	LPBYTE pFinalBuf = NULL;
	DWORD dwFinalBufSize = 0;
	objPE.MergeBuf(objPE.m_pFileBuf, objPE.m_dwImageSize,
		pShellBuf, modinfo.SizeOfImage, 
		pFinalBuf, dwFinalBufSize);

	//5.�����ļ���������ɵĻ�������
	SaveFinalFile(pFinalBuf, dwFinalBufSize, strFilePath);
	
	//6.�ͷ���Դ
	delete[] objPE.m_pFileBuf;
	delete[] pShellBuf;
	delete[] pFinalBuf;

	delete[] objPE.m_pMyImport;
	delete[] objPE.m_pModNameBuf;
	delete[] objPE.m_pFunNameBuf;
	objPE.InitValue();

	return TRUE;
}


//************************************************************
// ��������:	SaveFinalFile
// ����˵��:	�����ļ�
// ��	��:	cyxvc
// ʱ	��:	2015/12/25
// ��	��:	LPBYTE pFinalBuf
// ��	��:	DWORD pFinalBufSize
// ��	��:	CString strFilePath
// �� ��	ֵ:	BOOL
//************************************************************
BOOL CPACK::SaveFinalFile(LPBYTE pFinalBuf, DWORD pFinalBufSize, CString strFilePath)
{
	//����������Ϣ�����С���ļ������Сͬ�ڴ�����С��
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFinalBuf;
	PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)(pFinalBuf + pDosHeader->e_lfanew);
	PIMAGE_SECTION_HEADER pSectionHeader = IMAGE_FIRST_SECTION(pNtHeader);
	for (DWORD i = 0; i < pNtHeader->FileHeader.NumberOfSections; i++, pSectionHeader++)
	{
		pSectionHeader->PointerToRawData = pSectionHeader->VirtualAddress;
	}

	//�������Ҫ��Ŀ¼����Ϣ
	//ֻ��������ض�λ����Դ��
	DWORD dwCount = 15;
	for (DWORD i = 0; i < dwCount; i++)
	{
		if (i != IMAGE_DIRECTORY_ENTRY_EXPORT && 
			i != IMAGE_DIRECTORY_ENTRY_RESOURCE &&
			i != IMAGE_DIRECTORY_ENTRY_BASERELOC )
		{
			pNtHeader->OptionalHeader.DataDirectory[i].VirtualAddress = 0;
			pNtHeader->OptionalHeader.DataDirectory[i].Size = 0;
		}
	}

	//��ȡ����·��
	TCHAR strOutputPath[MAX_PATH] = { 0 };
	LPWSTR strSuffix = PathFindExtension(strFilePath);
	wcsncpy_s(strOutputPath, MAX_PATH, strFilePath, wcslen(strFilePath));
	PathRemoveExtension(strOutputPath);
	wcscat_s(strOutputPath, MAX_PATH, L"_cyxvc");
	wcscat_s(strOutputPath, MAX_PATH, strSuffix);

	//�����ļ�
	HANDLE hNewFile = CreateFile(
		strOutputPath,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hNewFile == INVALID_HANDLE_VALUE)
	{
		MessageBox(NULL, _T("�����ļ�ʧ�ܣ�"), _T("��ʾ"), MB_OK);
		return FALSE;
	}
	DWORD WriteSize = 0;
	BOOL bRes = WriteFile(hNewFile, pFinalBuf, pFinalBufSize, &WriteSize, NULL);
	if (bRes)
	{
		CloseHandle(hNewFile);
		return TRUE;
	}
	else
	{
		CloseHandle(hNewFile);
		MessageBox(NULL, _T("�����ļ�ʧ�ܣ�"), _T("��ʾ"), MB_OK);
		return FALSE;
	}
}
