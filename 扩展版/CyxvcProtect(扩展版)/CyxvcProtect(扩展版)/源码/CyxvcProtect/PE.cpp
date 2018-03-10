#include "stdafx.h"
#include "PE.h"


CPE::CPE()
{
	InitValue();
}


CPE::~CPE()
{
}

//************************************************************
// ��������:	InitValue
// ����˵��:	��ʼ������
// ��	��:	cyxvc
// ʱ	��:	2015/12/25
// �� ��	ֵ:	void
//************************************************************
void CPE::InitValue()
{
	m_hFile				= NULL;
	m_pFileBuf			= NULL;
	m_pDosHeader		= NULL;
	m_pNtHeader			= NULL;
	m_pSecHeader		= NULL;
	m_dwFileSize		= 0;
	m_dwImageSize		= 0;
	m_dwImageBase		= 0;
	m_dwCodeBase		= 0;
	m_dwCodeSize		= 0;
	m_dwPEOEP			= 0;
	m_dwShellOEP		= 0;
	m_dwSizeOfHeader	= 0;
	m_dwSectionNum		= 0;
	m_dwFileAlign		= 0;
	m_dwMemAlign		= 0;
	m_PERelocDir		= { 0 };
	m_PEImportDir		= { 0 };
	m_IATSectionBase	= 0;
	m_IATSectionSize	= 0;

	m_pMyImport			= 0;
	m_pModNameBuf		= 0;
	m_pFunNameBuf		= 0;
	m_dwNumOfIATFuns	= 0;
	m_dwSizeOfModBuf	= 0;
	m_dwSizeOfFunBuf	= 0;
	m_dwIATBaseRVA		= 0;
}

//************************************************************
// ��������:	InitPE
// ����˵��:	��ʼ��PE����ȡPE�ļ�������PE��Ϣ
// ��	��:	cyxvc
// ʱ	��:	2015/12/25
// ��	��:	CString strFilePath
// �� ��	ֵ:	BOOL
//************************************************************
BOOL CPE::InitPE(CString strFilePath)
{
	//���ļ�
	if (OpenPEFile(strFilePath) == FALSE)
		return FALSE;

	//��PE���ļ��ֲ���ʽ��ȡ���ڴ�
	m_dwFileSize = GetFileSize(m_hFile, NULL);
	
	m_pFileBuf = new BYTE[m_dwFileSize];
	DWORD ReadSize = 0;
	ReadFile(m_hFile, m_pFileBuf, m_dwFileSize, &ReadSize, NULL);	
	//#���ļ�ָ��ָ���λ�ÿ�ʼ�����ݶ�����һ���ļ��У� ��֧��ͬ�����첽����
	CloseHandle(m_hFile);
	m_hFile = NULL;

	//�ж��Ƿ�ΪPE�ļ�
	if (IsPE() == FALSE)
		return FALSE;

	//��PE���ڴ�ֲ���ʽ��ȡ���ڴ�
	//����û�����Сû�ж�������
	m_dwImageSize = m_pNtHeader->OptionalHeader.SizeOfImage;
	m_dwMemAlign = m_pNtHeader->OptionalHeader.SectionAlignment;
	m_dwSizeOfHeader = m_pNtHeader->OptionalHeader.SizeOfHeaders;
	//#0x3c   DWORD SizeOfHeaders; ��DOSͷ��PEͷ����������ܴ�С
	m_dwSectionNum = m_pNtHeader->FileHeader.NumberOfSections;

	if (m_dwImageSize % m_dwMemAlign)
		m_dwImageSize = (m_dwImageSize / m_dwMemAlign + 1) * m_dwMemAlign;
	LPBYTE pFileBuf_New = new BYTE[m_dwImageSize];
	memset(pFileBuf_New, 0, m_dwImageSize);
	//�����ļ�ͷ
	memcpy_s(pFileBuf_New, m_dwSizeOfHeader, m_pFileBuf, m_dwSizeOfHeader);
	//��������
	PIMAGE_SECTION_HEADER pSectionHeader = IMAGE_FIRST_SECTION(m_pNtHeader);
	for (DWORD i = 0; i < m_dwSectionNum; i++, pSectionHeader++)
	{
		memcpy_s(pFileBuf_New + pSectionHeader->VirtualAddress,
			pSectionHeader->SizeOfRawData,
			m_pFileBuf+pSectionHeader->PointerToRawData,
			pSectionHeader->SizeOfRawData);
	}
	delete[] m_pFileBuf;
	m_pFileBuf = pFileBuf_New;
	pFileBuf_New = NULL;

	//��ȡPE��Ϣ
	GetPEInfo();
	
	return TRUE;
}

//************************************************************
// ��������:	OpenPEFile
// ����˵��:	���ļ�
// ��	��:	cyxvc
// ʱ	��:	2015/12/25
// ��	��:	CString strFilePath
// �� ��	ֵ:	BOOL
//************************************************************
BOOL CPE::OpenPEFile(CString strFilePath)
{
	m_hFile = CreateFile(strFilePath,
		GENERIC_READ | GENERIC_WRITE, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	//#����һ���๦�ܵĺ������ɴ򿪻򴴽����¶��󣬲����ؿɷ��ʵľ��������̨��ͨ����Դ��
	//#Ŀ¼��ֻ���򿪣����������������ļ����ʲۣ��ܵ���
	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		MessageBox(NULL, _T("�����ļ�ʧ�ܣ�"), _T("��ʾ"), MB_OK);
		m_hFile = NULL;
		return FALSE;
	}
	return TRUE;
}

//************************************************************
// ��������:	IsPE
// ����˵��:	�ж��Ƿ�ΪPE�ļ�
// ��	��:	cyxvc
// ʱ	��:	2015/12/25
// �� ��	ֵ:	BOOL
//************************************************************
BOOL CPE::IsPE()
{
	//�ж��Ƿ�ΪPE�ļ�
	m_pDosHeader = (PIMAGE_DOS_HEADER)m_pFileBuf;
	if (m_pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
	{
		//����PE�ļ�
		MessageBox(NULL, _T("������Ч��PE�ļ���"), _T("��ʾ"), MB_OK);
		delete[] m_pFileBuf;
		InitValue();
		return FALSE;
	}
	m_pNtHeader = (PIMAGE_NT_HEADERS)(m_pFileBuf + m_pDosHeader->e_lfanew);
	if (m_pNtHeader->Signature != IMAGE_NT_SIGNATURE)
	{
		//����PE�ļ�
		MessageBox(NULL, _T("������Ч��PE�ļ���"), _T("��ʾ"), MB_OK);
		delete[] m_pFileBuf;
		InitValue();
		return FALSE;
	}
	return TRUE;
}

//************************************************************
// ��������:	GetPEInfo
// ����˵��:	��ȡPE��Ϣ
// ��	��:	cyxvc
// ʱ	��:	2015/12/25
// �� ��	ֵ:	void
//************************************************************
void CPE::GetPEInfo()
{
	m_pDosHeader	= (PIMAGE_DOS_HEADER)m_pFileBuf;
	m_pNtHeader		= (PIMAGE_NT_HEADERS)(m_pFileBuf + m_pDosHeader->e_lfanew);

	m_dwFileAlign	= m_pNtHeader->OptionalHeader.FileAlignment;
	m_dwMemAlign	= m_pNtHeader->OptionalHeader.SectionAlignment;
	m_dwImageBase	= m_pNtHeader->OptionalHeader.ImageBase;
	m_dwPEOEP		= m_pNtHeader->OptionalHeader.AddressOfEntryPoint;
	m_dwCodeBase	= m_pNtHeader->OptionalHeader.BaseOfCode;
	m_dwCodeSize	= m_pNtHeader->OptionalHeader.SizeOfCode;
	m_dwSizeOfHeader= m_pNtHeader->OptionalHeader.SizeOfHeaders;
	m_dwSectionNum	= m_pNtHeader->FileHeader.NumberOfSections;
	m_pSecHeader	= IMAGE_FIRST_SECTION(m_pNtHeader);
	m_pNtHeader->OptionalHeader.SizeOfImage = m_dwImageSize;

	//�����ض�λĿ¼��Ϣ
	m_PERelocDir = 
		IMAGE_DATA_DIRECTORY(m_pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC]);
	 
	//����IAT��ϢĿ¼��Ϣ
	m_PEImportDir =
		IMAGE_DATA_DIRECTORY(m_pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT]);
	//#���Ե�����ַ�������ҵ��ġ�ֱ�Ӽ�¼������Ŀ¼��һ���ˡ�
	//��ȡIAT���ڵ����ε���ʼλ�úʹ�С
	PIMAGE_SECTION_HEADER pSectionHeader = IMAGE_FIRST_SECTION(m_pNtHeader);
	for (DWORD i = 0; i < m_dwSectionNum; i++, pSectionHeader++)
	{
		if (m_PEImportDir.VirtualAddress >= pSectionHeader->VirtualAddress&&
			m_PEImportDir.VirtualAddress <= pSectionHeader[1].VirtualAddress)
		{
			//��������ε���ʼ��ַ�ʹ�С
			m_IATSectionBase = pSectionHeader->VirtualAddress;
			m_IATSectionSize = pSectionHeader[1].VirtualAddress - pSectionHeader->VirtualAddress;
			break;
		}
	}
}

//************************************************************
// ��������:	XorCode
// ����˵��:	����μ���
// ��	��:	cyxvc
// ʱ	��:	2015/12/25
// ��	��:	BYTE byXOR
// �� ��	ֵ:	DWORD
//************************************************************
DWORD CPE::XorCode(BYTE byXOR)
{
	PBYTE pCodeBase = (PBYTE)((DWORD)m_pFileBuf + m_dwCodeBase);
	for (DWORD i = 0; i < m_dwCodeSize; i++)
	{
		pCodeBase[i] ^= i;
	}
	return m_dwCodeSize;
}

//************************************************************
// ��������:	XorMachineCode
// ����˵��:	�������(��������ͬ����ν������)
// ��	��:	cyxvc
// ʱ	��:	2015/12/30
// �� ��	ֵ:	void
//************************************************************
void CPE::XorMachineCode(CHAR MachineCode[16])
{
	PBYTE pCodeBase = (PBYTE)((DWORD)m_pFileBuf + m_dwCodeBase);
	DWORD j = 0;
	for (DWORD i = 0; i < m_dwCodeSize; i++)
	{
		pCodeBase[i] ^= MachineCode[j++];
		if (j==16)
			j = 0;
	}
}

//************************************************************
// ��������:	SetShellReloc
// ����˵��:	����Shell���ض�λ��Ϣ
// ��	��:	cyxvc
// ʱ	��:	2015/12/25
// ��	��:	LPBYTE pShellBuf
// �� ��	ֵ:	BOOL
//************************************************************
BOOL CPE::SetShellReloc(LPBYTE pShellBuf, DWORD hShell)
{
	typedef struct _TYPEOFFSET
	{
		WORD offset : 12;			//ƫ��ֵ
		WORD Type	: 4;			//�ض�λ����(��ʽ)
	}TYPEOFFSET, *PTYPEOFFSET;

	//1.��ȡ���ӿ�PE�ļ����ض�λĿ¼��ָ����Ϣ
	PIMAGE_DATA_DIRECTORY pPERelocDir =
		&(m_pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC]);
	
	//2.��ȡShell���ض�λ��ָ����Ϣ
	PIMAGE_DOS_HEADER		pShellDosHeader = (PIMAGE_DOS_HEADER)pShellBuf;
	PIMAGE_NT_HEADERS		pShellNtHeader = (PIMAGE_NT_HEADERS)(pShellBuf + pShellDosHeader->e_lfanew);
	PIMAGE_DATA_DIRECTORY	pShellRelocDir =
		&(pShellNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC]);
	PIMAGE_BASE_RELOCATION	pShellReloc = 
		(PIMAGE_BASE_RELOCATION)((DWORD)pShellBuf + pShellRelocDir->VirtualAddress);
	
	//3.��ԭ�޸��ض�λ��Ϣ
	//����Shell.dll��ͨ��LoadLibrary���صģ�����ϵͳ��������һ���ض�λ
	//������Ҫ��Shell.dll���ض�λ��Ϣ�ָ���ϵͳû����ǰ�����ӣ�Ȼ����д�뱻�ӿ��ļ���ĩβ
	while (pShellReloc->VirtualAddress)
	{		
		PTYPEOFFSET pTypeOffset = (PTYPEOFFSET)(pShellReloc + 1);
		DWORD dwNumber = (pShellReloc->SizeOfBlock - 8) / 2;

		for (DWORD i = 0; i < dwNumber; i++)
		{
			if (*(PWORD)(&pTypeOffset[i]) == NULL)
				break;
			//RVA
			DWORD dwRVA = pTypeOffset[i].offset + pShellReloc->VirtualAddress;
			//FAR��ַ
			//***�µ��ض�λ��ַ=�ض�λ��ַ-ӳ���ַ+�µ�ӳ���ַ+�����ַ
			DWORD AddrOfNeedReloc = *(PDWORD)((DWORD)pShellBuf + dwRVA);
			*(PDWORD)((DWORD)pShellBuf + dwRVA)
				= AddrOfNeedReloc - pShellNtHeader->OptionalHeader.ImageBase + m_dwImageBase + m_dwImageSize;
		}
		//3.1�޸�Shell�ض�λ����.text��RVA
		pShellReloc->VirtualAddress += m_dwImageSize;
		// �޸���һ������
		pShellReloc = (PIMAGE_BASE_RELOCATION)((DWORD)pShellReloc + pShellReloc->SizeOfBlock);
	}

	//4.�޸�PE�ض�λĿ¼ָ�룬ָ��Shell���ض�λ����Ϣ
	pPERelocDir->Size = pShellRelocDir->Size;
	pPERelocDir->VirtualAddress = pShellRelocDir->VirtualAddress + m_dwImageSize;

	return TRUE;
}

//************************************************************
// ��������:	MergeBuf
// ����˵��:	�ϲ�PE�ļ���Shell
// ��	��:	cyxvc
// ʱ	��:	2015/12/25
// ��	��:	LPBYTE pFileBuf
// ��	��:	DWORD pFileBufSize
// ��	��:	LPBYTE pShellBuf
// ��	��:	DWORD pShellBufSize
// ��	��:	LPBYTE & pFinalBuf
// �� ��	ֵ:	void
//************************************************************
void CPE::MergeBuf(LPBYTE pFileBuf, DWORD pFileBufSize,
	LPBYTE pShellBuf, DWORD pShellBufSize, 
	LPBYTE& pFinalBuf, DWORD& pFinalBufSize)
{
	//��ȡ���һ�����ε���Ϣ
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuf;
	PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)(pFileBuf + pDosHeader->e_lfanew);
	PIMAGE_SECTION_HEADER pSectionHeader = IMAGE_FIRST_SECTION(pNtHeader);
	PIMAGE_SECTION_HEADER pLastSection =
		&pSectionHeader[pNtHeader->FileHeader.NumberOfSections - 1];

	//1.�޸���������
	pNtHeader->FileHeader.NumberOfSections += 1;

	//2.�༭���α�ͷ�ṹ����Ϣ
	PIMAGE_SECTION_HEADER AddSectionHeader =
		&pSectionHeader[pNtHeader->FileHeader.NumberOfSections - 1];
	memcpy_s(AddSectionHeader->Name, 8, ".cyxvc", 7);

	//VOffset(1000����)
	DWORD dwTemp = 0;
	dwTemp = (pLastSection->Misc.VirtualSize / m_dwMemAlign) * m_dwMemAlign;
	if (pLastSection->Misc.VirtualSize % m_dwMemAlign)
	{
		dwTemp += m_dwMemAlign;
	}
	AddSectionHeader->VirtualAddress = pLastSection->VirtualAddress + dwTemp;

	//Vsize��ʵ����ӵĴ�С��
	AddSectionHeader->Misc.VirtualSize = pShellBufSize;

	//ROffset�����ļ���ĩβ��
	AddSectionHeader->PointerToRawData = pFileBufSize;

	//RSize(200����)
	dwTemp = (pShellBufSize / m_dwFileAlign) * m_dwFileAlign;
	if (pShellBufSize % m_dwFileAlign)
	{
		dwTemp += m_dwFileAlign;
	}
	AddSectionHeader->SizeOfRawData = dwTemp;

	//��־
	AddSectionHeader->Characteristics = 0XE0000040;

	//3.�޸�PEͷ�ļ���С���ԣ������ļ���С
	dwTemp = (pShellBufSize / m_dwMemAlign) * m_dwMemAlign;
	if (pShellBufSize % m_dwMemAlign)
	{
		dwTemp += m_dwMemAlign;
	}
	pNtHeader->OptionalHeader.SizeOfImage += dwTemp;


	//4.����ϲ�����Ҫ�Ŀռ�
	//4.0.���㱣��IAT���õĿռ��С
	DWORD dwIATSize = 0;
	dwIATSize = m_dwSizeOfModBuf + m_dwSizeOfFunBuf + m_dwNumOfIATFuns*sizeof(MYIMPORT);

	if (dwIATSize % m_dwMemAlign)
	{
		dwIATSize = (dwIATSize / m_dwMemAlign + 1)*m_dwMemAlign;
	}
	pNtHeader->OptionalHeader.SizeOfImage += dwIATSize;
	AddSectionHeader->Misc.VirtualSize += dwIATSize;
	AddSectionHeader->SizeOfRawData += dwIATSize;

	pFinalBuf = new BYTE[pFileBufSize + dwTemp + dwIATSize];
	pFinalBufSize = pFileBufSize + dwTemp + dwIATSize;
	memset(pFinalBuf, 0, pFileBufSize + dwTemp + dwIATSize);
	memcpy_s(pFinalBuf, pFileBufSize, pFileBuf, pFileBufSize);
	memcpy_s(pFinalBuf + pFileBufSize, dwTemp, pShellBuf, dwTemp);

	//����IAT��Ϣ
	if (dwIATSize == 0)
	{
		return;
	}
	DWORD dwIATBaseRVA = pFileBufSize + pShellBufSize;
	m_dwIATBaseRVA = dwIATBaseRVA;

	memcpy_s(pFinalBuf + dwIATBaseRVA,
		dwIATSize, m_pMyImport, m_dwNumOfIATFuns*sizeof(MYIMPORT));

	//����ģ����
	for (DWORD i = 0; i < m_dwSizeOfModBuf; i++)
	{
		if (((char*)m_pModNameBuf)[i] != 0)
		{
			((char*)m_pModNameBuf)[i] ^= 0x15;
		}
	}

	memcpy_s(pFinalBuf + dwIATBaseRVA + m_dwNumOfIATFuns*sizeof(MYIMPORT),
		dwIATSize, m_pModNameBuf, m_dwSizeOfModBuf);

	//IAT����������
	for (DWORD i = 0; i < m_dwSizeOfFunBuf; i++)
	{
		if (((char*)m_pFunNameBuf)[i] != 0)
		{
			((char*)m_pFunNameBuf)[i] ^= 0x15;
		}
	}

	memcpy_s(pFinalBuf + dwIATBaseRVA + m_dwNumOfIATFuns*sizeof(MYIMPORT)+m_dwSizeOfModBuf,
		dwIATSize, m_pFunNameBuf, m_dwSizeOfFunBuf);
}

//************************************************************
// ��������:	SetNewOEP
// ����˵��:	�޸��µ�OEPΪShell��Start����
// ��	��:	cyxvc
// ʱ	��:	2015/12/25
// ��	��:	DWORD dwOEP
// �� ��	ֵ:	void
//************************************************************
void CPE::SetNewOEP(DWORD dwShellOEP)
{
	m_dwShellOEP = dwShellOEP + m_dwImageSize;
	m_pNtHeader->OptionalHeader.AddressOfEntryPoint = m_dwShellOEP;
}

//************************************************************
// ��������:	ClsImportTab
// ����˵��:	ĨȥIAT(�����)����
// ��	��:	cyxvc
// ʱ	��:	2015/12/28
// �� ��	ֵ:	void
//************************************************************
void CPE::ClsImportTab()
{
	if (m_PEImportDir.VirtualAddress == 0)
	{
		return;
	}
	//1.��ȡ�����ṹ��ָ��
	PIMAGE_IMPORT_DESCRIPTOR pPEImport =
		(PIMAGE_IMPORT_DESCRIPTOR)(m_pFileBuf + m_PEImportDir.VirtualAddress);

	//2.��ʼѭ��ĨȥIAT(�����)����
	//ÿѭ��һ��Ĩȥһ��Dll�����е�����Ϣ
	while (pPEImport->Name)
	{
		//2.1.Ĩȥģ����
		DWORD dwModNameRVA = pPEImport->Name;
		char* pModName = (char*)(m_pFileBuf + dwModNameRVA);
		memset(pModName, 0, strlen(pModName));

		PIMAGE_THUNK_DATA pIAT = (PIMAGE_THUNK_DATA)(m_pFileBuf + pPEImport->FirstThunk);
		PIMAGE_THUNK_DATA pINT = (PIMAGE_THUNK_DATA)(m_pFileBuf + pPEImport->OriginalFirstThunk);

		//2.2.ĨȥIAT��INT�ͺ������������
		while (pIAT->u1.AddressOfData)
		{
			//�ж�������������������
			if (IMAGE_SNAP_BY_ORDINAL(pIAT->u1.Ordinal))
			{
				//Ĩȥ��ž��ǽ�pIAT���
			}
			else
			{
				//���������
				DWORD dwFunNameRVA = pIAT->u1.AddressOfData;
				PIMAGE_IMPORT_BY_NAME pstcFunName = (PIMAGE_IMPORT_BY_NAME)(m_pFileBuf + dwFunNameRVA);
				//����������ͺ������
				memset(pstcFunName, 0, strlen(pstcFunName->Name) + sizeof(WORD));
			}
			memset(pINT, 0, sizeof(IMAGE_THUNK_DATA));
			memset(pIAT, 0, sizeof(IMAGE_THUNK_DATA));
			pINT++;
			pIAT++;
		}

		//2.3.Ĩȥ�����Ŀ¼��Ϣ
		memset(pPEImport, 0, sizeof(IMAGE_IMPORT_DESCRIPTOR));

		//������һ��ģ��
		pPEImport++;
	}
}

//************************************************************
// ��������:	SaveImportTab
// ����˵��:	���Լ���������ݽṹ��ʽ����IAT(�����)
// ��	��:	cyxvc
// ʱ	��:	2015/12/28
// �� ��	ֵ:	void
//************************************************************
void CPE::SaveImportTab()
{
	if (m_PEImportDir.VirtualAddress == 0)
	{
		return;
	}
	//0.��ȡ�����ṹ��ָ��
	PIMAGE_IMPORT_DESCRIPTOR pPEImport =
		(PIMAGE_IMPORT_DESCRIPTOR)(m_pFileBuf + m_PEImportDir.VirtualAddress);

	//1.��һ��ѭ��ȷ�� m_pModNameBuf �� m_pFunNameBuf �Ĵ�С
	DWORD dwSizeOfModBuf = 0;
	DWORD dwSizeOfFunBuf = 0;
	m_dwNumOfIATFuns = 0;
	while (pPEImport->Name)
	{
		DWORD dwModNameRVA = pPEImport->Name;
		char* pModName = (char*)(m_pFileBuf + dwModNameRVA);
		dwSizeOfModBuf += (strlen(pModName) + 1);

		PIMAGE_THUNK_DATA pIAT = (PIMAGE_THUNK_DATA)(m_pFileBuf + pPEImport->FirstThunk);

		while (pIAT->u1.AddressOfData)
		{
			if (IMAGE_SNAP_BY_ORDINAL(pIAT->u1.Ordinal))
			{
				m_dwNumOfIATFuns++;
			}
			else
			{
				m_dwNumOfIATFuns++;
				//##ò�����ã�ֻ��¼��С
				//#DWORD dwFunNameRVA = pIAT->u1.AddressOfData;
				//#PIMAGE_IMPORT_BY_NAME pstcFunName = (PIMAGE_IMPORT_BY_NAME)(m_pFileBuf + dwFunNameRVA);
				//#dwSizeOfFunBuf += (strlen(pstcFunName->Name) + 1);
			}
			pIAT++;
		}
		pPEImport++;
	}

	//2.�ڶ���ѭ��������Ϣ
	//���´������Լ�д�Ķ��е�����ˣ��������Լ���������ݽṹ�����Կ������ɶ��Բ�ǿ
	m_pModNameBuf = new CHAR[dwSizeOfModBuf];
	m_pFunNameBuf = new CHAR[dwSizeOfFunBuf];
	m_pMyImport = new MYIMPORT[m_dwNumOfIATFuns];
	memset(m_pModNameBuf, 0, dwSizeOfModBuf);
	memset(m_pFunNameBuf, 0, dwSizeOfFunBuf);
	memset(m_pMyImport, 0, sizeof(MYIMPORT)*m_dwNumOfIATFuns);

	pPEImport =	(PIMAGE_IMPORT_DESCRIPTOR)(m_pFileBuf + m_PEImportDir.VirtualAddress);
	DWORD TempNumOfFuns = 0;
	DWORD TempModRVA = 0;
	DWORD TempFunRVA = 0;
	while (pPEImport->Name)
	{
		DWORD dwModNameRVA = pPEImport->Name;
		char* pModName = (char*)(m_pFileBuf + dwModNameRVA);
		memcpy_s((PCHAR)m_pModNameBuf + TempModRVA, strlen(pModName) + 1, 
			pModName, strlen(pModName) + 1);

		PIMAGE_THUNK_DATA pIAT = (PIMAGE_THUNK_DATA)(m_pFileBuf + pPEImport->FirstThunk);

		while (pIAT->u1.AddressOfData)
		{
			if (IMAGE_SNAP_BY_ORDINAL(pIAT->u1.Ordinal))
			{
				//��������ų���ʽ�ĺ�����Ϣ
				m_pMyImport[TempNumOfFuns].m_dwIATAddr = (DWORD)pIAT - (DWORD)m_pFileBuf;
				m_pMyImport[TempNumOfFuns].m_bIsOrdinal = TRUE;
				m_pMyImport[TempNumOfFuns].m_Ordinal = pIAT->u1.Ordinal & 0x7FFFFFFF;
				m_pMyImport[TempNumOfFuns].m_dwModNameRVA = TempModRVA;
			}
			else
			{
				//�������ƺų���ʽ�ĺ�����Ϣ
				m_pMyImport[TempNumOfFuns].m_dwIATAddr = (DWORD)pIAT - (DWORD)m_pFileBuf;
				//#��1��IAT��ַ���ɵ�������pPEImport->FirstThunk�õ��ģ��õ�����RVA��
				DWORD dwFunNameRVA = pIAT->u1.AddressOfData;
				PIMAGE_IMPORT_BY_NAME pstcFunName = (PIMAGE_IMPORT_BY_NAME)(m_pFileBuf + dwFunNameRVA);
				memcpy_s((PCHAR)m_pFunNameBuf + TempFunRVA, strlen(pstcFunName->Name) + 1,
					pstcFunName->Name, strlen(pstcFunName->Name) + 1);

				m_pMyImport[TempNumOfFuns].m_dwFunNameRVA = TempFunRVA;
				m_pMyImport[TempNumOfFuns].m_dwModNameRVA = TempModRVA;
				TempFunRVA += (strlen(pstcFunName->Name) + 1);
			}
			TempNumOfFuns++;
			pIAT++;
		}
		TempModRVA += (strlen(pModName) + 1);
		pPEImport++;
	}

	//��������m_pMyImport
	MYIMPORT stcTemp = { 0 };
	DWORD dwTempNum = m_dwNumOfIATFuns / 2;
	for (DWORD i = 0; i < dwTempNum; i++)
	{
		m_pMyImport[i];
		m_pMyImport[m_dwNumOfIATFuns - i - 1];
		memcpy_s(&stcTemp, sizeof(MYIMPORT), &m_pMyImport[i], sizeof(MYIMPORT));
		memcpy_s(&m_pMyImport[i], sizeof(MYIMPORT), &m_pMyImport[m_dwNumOfIATFuns - i - 1], sizeof(MYIMPORT));
		memcpy_s(&m_pMyImport[m_dwNumOfIATFuns - i - 1], sizeof(MYIMPORT), &stcTemp, sizeof(MYIMPORT));
	}

	//������Ϣ
	m_dwSizeOfModBuf = dwSizeOfModBuf;
	m_dwSizeOfFunBuf = dwSizeOfFunBuf;
}







