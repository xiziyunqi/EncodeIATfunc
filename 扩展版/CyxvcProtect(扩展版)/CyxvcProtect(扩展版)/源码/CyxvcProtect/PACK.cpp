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
// 函数名称:	Pack
// 函数说明:	加壳
// 作	者:	cyxvc
// 时	间:	2015/12/25
// 参	数:	CString strFilePath
// 返 回	值:	BOOL
//************************************************************
BOOL CPACK::Pack(CString strFilePath, BOOL bSelect[5], CHAR MachineCode[16])
{	//#0.MFC整体框架用于人机交互
	//1.读取文件PE文件信息并保存
	CPE objPE;
	if (objPE.InitPE(strFilePath) == FALSE)
		return FALSE;

	//2.0.加密操作
	//2.1.IAT加密
	if (bSelect[1] == TRUE)
	{
		//2.1.1.保存IAT(导入表)信息
		objPE.SaveImportTab();
		//2.1.2.抹去原始IAT(导入表)
		objPE.ClsImportTab();
	}


	//2.2.加密代码段
	if (bSelect[0] == TRUE)
		objPE.XorCode(0x15);

	//2.2.3机器码绑定
	if (bSelect[2] == TRUE)
		objPE.XorMachineCode(MachineCode);


	//3.将必要的信息保存到Shell
	HMODULE hShell = LoadLibrary(L"../Debug/Shell.dll");
	if (hShell == NULL)
	{
		MessageBox(NULL, _T("加载Shell.dll模块失败，请确保程序的完整性！"), _T("提示"), MB_OK);
		//释放资源
		delete[] objPE.m_pFileBuf;
		return FALSE;
	}
	//#保存重要信息到自定义结构体PSHELL_DATA
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

	//4.将Shell附加到PE文件
	//4.1.读取Shell代码
	MODULEINFO modinfo = { 0 };
	/*##《2》包含模块的进程的模块的信息，模块基址、大小和入口
	typedef struct _MODULEINFO {
	LPVOID lpBaseOfDll;
	DWORD SizeOfImage;
	LPVOID EntryPoint;
	} MODULEINFO, *LPMODULEINFO;
	*/

	GetModuleInformation(GetCurrentProcess(), hShell, &modinfo, sizeof(MODULEINFO));
	//#《1》获取模块信息并储存在MODULEINFO
	/*##
	hProcess
	指向包含模块的进程的句柄。这个句柄一定要有 PROCESS_QUERY_INFORMATION 和 PROCESS_VM_READ 权限。参考 Process Security and Access Rights[3]
	hModule
	指向模块的句柄。
	lpmodinfo
	指向 MODULEINFO 结构的指针，该结构将会储存关于模块的信息。
	cb
	结构 MODULEINFO  的大小（以字节计算）。
	*/
	//#
	//-----------------------------------------------------	
	pstcShellData->dwIATBaseRVA = objPE.m_dwImageSize + modinfo.SizeOfImage;
	//#《3》为什么IAT地址是镜像大小+dll镜像大小
	//-----------------------------------------------------
	PBYTE  pShellBuf = new BYTE[modinfo.SizeOfImage];
	/*##
	之前已经通过LoadLibrary(L"Shell.dll")的方式加载了Shell模块，但为了操作方便我还是申请了
	一块空间专门存放Shell部分，同时获取一下Shell的镜像大小，为增加区段做准备。
	##其实是多此一举吧，另外loadlibrary是内存对齐
	*/
	memcpy_s(pShellBuf, modinfo.SizeOfImage, hShell, modinfo.SizeOfImage);
	//4.2.设置Shell重定位信息
	objPE.SetShellReloc(pShellBuf, (DWORD)hShell);
	//#1.还原由loadlibrary加载dll进行的重定位，2.把pe文件的重定位指向dll的
	//#重定位目录（记得指针偏移要加上pe文件的镜像大小）
	//4.3.修改被加壳程序的OEP，指向Shell
	DWORD dwShellOEP = pstcShellData->dwStartFun - (DWORD)hShell;
	objPE.SetNewOEP(dwShellOEP);
	//4.4.合并PE文件和Shell的代码到新的缓冲区
	LPBYTE pFinalBuf = NULL;
	DWORD dwFinalBufSize = 0;
	objPE.MergeBuf(objPE.m_pFileBuf, objPE.m_dwImageSize,
		pShellBuf, modinfo.SizeOfImage, 
		pFinalBuf, dwFinalBufSize);

	//5.保存文件（处理完成的缓冲区）
	SaveFinalFile(pFinalBuf, dwFinalBufSize, strFilePath);
	
	//6.释放资源
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
// 函数名称:	SaveFinalFile
// 函数说明:	保存文件
// 作	者:	cyxvc
// 时	间:	2015/12/25
// 参	数:	LPBYTE pFinalBuf
// 参	数:	DWORD pFinalBufSize
// 参	数:	CString strFilePath
// 返 回	值:	BOOL
//************************************************************
BOOL CPACK::SaveFinalFile(LPBYTE pFinalBuf, DWORD pFinalBufSize, CString strFilePath)
{
	//修正区段信息对齐大小（文件对齐大小同内存对齐大小）
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFinalBuf;
	PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)(pFinalBuf + pDosHeader->e_lfanew);
	PIMAGE_SECTION_HEADER pSectionHeader = IMAGE_FIRST_SECTION(pNtHeader);
	for (DWORD i = 0; i < pNtHeader->FileHeader.NumberOfSections; i++, pSectionHeader++)
	{
		pSectionHeader->PointerToRawData = pSectionHeader->VirtualAddress;
	}

	//清除不需要的目录表信息
	//只留输出表，重定位表，资源表
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

	//获取保存路径
	TCHAR strOutputPath[MAX_PATH] = { 0 };
	LPWSTR strSuffix = PathFindExtension(strFilePath);
	wcsncpy_s(strOutputPath, MAX_PATH, strFilePath, wcslen(strFilePath));
	PathRemoveExtension(strOutputPath);
	wcscat_s(strOutputPath, MAX_PATH, L"_cyxvc");
	wcscat_s(strOutputPath, MAX_PATH, strSuffix);

	//保存文件
	HANDLE hNewFile = CreateFile(
		strOutputPath,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hNewFile == INVALID_HANDLE_VALUE)
	{
		MessageBox(NULL, _T("保存文件失败！"), _T("提示"), MB_OK);
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
		MessageBox(NULL, _T("保存文件失败！"), _T("提示"), MB_OK);
		return FALSE;
	}
}
