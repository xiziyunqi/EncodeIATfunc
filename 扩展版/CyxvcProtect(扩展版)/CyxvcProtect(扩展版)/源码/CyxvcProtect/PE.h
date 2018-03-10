#pragma once

class CPE
{
public:
	CPE();
	~CPE();
public:
	HANDLE					m_hFile;			//PE�ļ����
	LPBYTE					m_pFileBuf;			//PE�ļ�������
	DWORD					m_dwFileSize;		//�ļ���С
	DWORD					m_dwImageSize;		//�����С
	PIMAGE_DOS_HEADER		m_pDosHeader;		//Dosͷ
	PIMAGE_NT_HEADERS		m_pNtHeader;		//NTͷ
	PIMAGE_SECTION_HEADER	m_pSecHeader;		//��һ��SECTION�ṹ��ָ��
	DWORD					m_dwImageBase;		//�����ַ
	DWORD					m_dwCodeBase;		//�����ַ
	DWORD					m_dwCodeSize;		//�����С
	DWORD					m_dwPEOEP;			//OEP��ַ
	DWORD					m_dwShellOEP;		//��OEP��ַ
	DWORD					m_dwSizeOfHeader;	//�ļ�ͷ��С
	DWORD					m_dwSectionNum;		//��������

	DWORD					m_dwFileAlign;		//�ļ�����
	DWORD					m_dwMemAlign;		//�ڴ����

	DWORD					m_IATSectionBase;	//IAT���ڶλ�ַ
	DWORD					m_IATSectionSize;	//IAT���ڶδ�С

	IMAGE_DATA_DIRECTORY	m_PERelocDir;		//�ض�λ����Ϣ
	IMAGE_DATA_DIRECTORY	m_PEImportDir;		//�������Ϣ
	
	//�Զ���IAT�ṹ��
	//������Դ���ͷ�
	typedef struct _MYIMPORT
	{
		DWORD	m_dwIATAddr;
		DWORD	m_dwModNameRVA;
		DWORD	m_dwFunNameRVA;
		BOOL	m_bIsOrdinal;
		DWORD	m_Ordinal;
	}MYIMPORT, *PMYIMPORT;
	PMYIMPORT				m_pMyImport;
	PVOID					m_pModNameBuf;
	PVOID					m_pFunNameBuf;
	DWORD					m_dwNumOfIATFuns;
	DWORD					m_dwSizeOfModBuf;
	DWORD					m_dwSizeOfFunBuf;

	DWORD					m_dwIATBaseRVA;

public:
	BOOL InitPE(CString strFilePath);			//��ʼ��PE����ȡPE�ļ�������PE��Ϣ
	void InitValue();							//��ʼ����
	BOOL OpenPEFile(CString strFilePath);		//���ļ�
	BOOL IsPE();								//�ж��Ƿ�ΪPE�ļ�
	void GetPEInfo();							//��ȡPE��Ϣ
	DWORD XorCode(BYTE byXOR);					//����μ���
	void XorMachineCode(CHAR MachineCode[16]);						//�������

	BOOL SetShellReloc(LPBYTE pShellBuf, DWORD hShell);		
												//����Shell���ض�λ��Ϣ

	void MergeBuf(LPBYTE pFileBuf, DWORD pFileBufSize, 
		LPBYTE pShellBuf, DWORD pShellBufSize, 
		LPBYTE& pFinalBuf, DWORD& pFinalBufSize);
												//�ϲ�PE�ļ���Shell
	void SetNewOEP(DWORD dwShellOEP);			//�޸��µ�OEPΪShell��Start����

	void SaveImportTab();						//���Լ���������ݽṹ��ʽ����IAT(�����)
	void ClsImportTab();						//ĨȥIAT(�����)����
};

