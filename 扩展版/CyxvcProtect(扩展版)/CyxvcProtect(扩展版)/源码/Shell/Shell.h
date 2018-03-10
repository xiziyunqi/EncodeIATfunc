#ifdef SHELL_EXPORTS
#define SHELL_API __declspec(dllexport)
#else
#define SHELL_API __declspec(dllimport)
#endif

typedef struct _MYIMPORT
{
	DWORD	m_dwIATAddr;			//IAT��ַ
	DWORD	m_dwModNameRVA;			//ģ����ƫ��
	DWORD	m_dwFunNameRVA;			//������ƫ��
	BOOL	m_bIsOrdinal;			//�Ƿ�Ϊ��ŵ�������
	DWORD	m_Ordinal;				//���
}MYIMPORT, *PMYIMPORT;
//#��¼��λIAT�����Ľṹ��
//����ShellData�ṹ��
extern"C"  typedef struct _SHELL_DATA
{
	DWORD dwStartFun;							//��������
	DWORD dwPEOEP;								//������ڵ�
	DWORD dwXorKey;								//����KEY
	DWORD dwCodeBase;							//�������ʼ��ַ
	DWORD dwXorSize;							//����μ��ܴ�С
	DWORD dwPEImageBase;						//PE�ļ�ӳ���ַ

	IMAGE_DATA_DIRECTORY	stcPERelocDir;		//�ض�λ����Ϣ
	IMAGE_DATA_DIRECTORY	stcPEImportDir;		//�������Ϣ

	DWORD					dwIATSectionBase;	//IAT���ڶλ�ַ
	DWORD					dwIATSectionSize;	//IAT���ڶδ�С

	//����IAT���õ��ĵı���
	PMYIMPORT				pMyImport;
	PVOID					pModNameBuf;
	PVOID					pFunNameBuf;
	DWORD					dwNumOfIATFuns;
	DWORD					dwSizeOfModBuf;
	DWORD					dwSizeOfFunBuf;
	DWORD					dwIATBaseRVA;

	//�ӿ�
	BOOL bSelect[5];

}SHELL_DATA, *PSHELL_DATA;

//����ShellData�ṹ�����
extern"C" SHELL_API SHELL_DATA g_stcShellData;



//Shell�����õ��ĺ��������Ͷ���
typedef DWORD(WINAPI *fnGetProcAddress)(_In_ HMODULE hModule, _In_ LPCSTR lpProcName);
typedef HMODULE(WINAPI *fnLoadLibraryA)(_In_ LPCSTR lpLibFileName);
typedef HMODULE(WINAPI *fnGetModuleHandleA)(_In_opt_ LPCSTR lpModuleName);
typedef BOOL(WINAPI *fnVirtualProtect)(_In_ LPVOID lpAddress, _In_ SIZE_T dwSize, _In_ DWORD flNewProtect, _Out_ PDWORD lpflOldProtect);
typedef LPVOID(WINAPI *fnVirtualAlloc)(_In_opt_ LPVOID lpAddress, _In_ SIZE_T dwSize, _In_ DWORD flAllocationType, _In_ DWORD flProtect);
typedef void(WINAPI *fnExitProcess)(_In_ UINT uExitCode);
typedef int(WINAPI *fnMessageBox)(HWND hWnd, LPSTR lpText, LPSTR lpCaption, UINT uType);
