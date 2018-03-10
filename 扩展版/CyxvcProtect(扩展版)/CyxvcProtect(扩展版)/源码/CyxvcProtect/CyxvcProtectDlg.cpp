
// CyxvcProtectDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CyxvcProtect.h"
#include "CyxvcProtectDlg.h"
#include "afxdialogex.h"
#include <tchar.h>
#include "PACK.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CCyxvcProtectDlg::CCyxvcProtectDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCyxvcProtectDlg::IDD, pParent)
	, m_strFilePath(_T(""))
	, m_strMachineCode(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCyxvcProtectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strFilePath);
	DDX_Text(pDX, IDC_EDIT2, m_strMachineCode);
	DDX_Control(pDX, IDC_EDIT2, m_EditControl);
}

BEGIN_MESSAGE_MAP(CCyxvcProtectDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CCyxvcProtectDlg::OnBnClicked_OpenFile)
	ON_BN_CLICKED(IDC_BUTTON2, &CCyxvcProtectDlg::OnBnClicked_Pack)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_CHECK3, &CCyxvcProtectDlg::OnBnClickedCheck3)
END_MESSAGE_MAP()

BOOL CCyxvcProtectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������

	m_bSelect[0] = FALSE;
	m_bSelect[1] = FALSE;
	m_bSelect[2] = FALSE;
	m_bSelect[3] = FALSE;
	m_bSelect[4] = FALSE;
	m_EditControl.EnableWindow(FALSE);
	memset(m_arrMachineCode, 0, 16);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CCyxvcProtectDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);
}

void CCyxvcProtectDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CCyxvcProtectDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//************************************************************
// ��������:	OnBnClicked_OpenFile
// ����˵��:	���ļ���ť
// ��	��:	cyxvc
// ʱ	��:	2015/12/24
// �� ��	ֵ:	void
//************************************************************
void CCyxvcProtectDlg::OnBnClicked_OpenFile()
{
	UpdateData(TRUE);
	CFileDialog dlg(TRUE, NULL, NULL, 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		(LPCTSTR)_TEXT("All Files (*.*)|*.*||"), NULL);
	if (dlg.DoModal() == IDOK)
		m_strFilePath = dlg.GetPathName();
	else
		return;
	UpdateData(FALSE);
}


//************************************************************
// ��������:	OnBnClicked_Pack
// ����˵��:	�ӿǰ�ť
// ��	��:	cyxvc
// ʱ	��:	2015/12/24
// �� ��	ֵ:	void
//************************************************************
void CCyxvcProtectDlg::OnBnClicked_Pack()
{
	if (m_strFilePath.IsEmpty())
	{
		MessageBox(_T("��ѡ�񱻼ӿǵ��ļ���"), _T("��ʾ"), MB_OK);
		return;
	}
		
	//��ȡ�ӿ�ѡ��
	if (GetSelect() == FALSE)
		return;

	//��ʼ�ӿ�
	CPACK objPACK;
	if (objPACK.Pack(m_strFilePath, m_bSelect, m_arrMachineCode))
		MessageBox(_T("�ӿǳɹ���"), _T("��ʾ"), MB_OK);
}


//************************************************************
// ��������:	OnDropFiles
// ����˵��:	�ļ���ק��Ӧ����
// ��	��:	cyxvc
// ʱ	��:	2015/12/30
// ��	��:	HDROP hDropInfo
// �� ��	ֵ:	void
//************************************************************
void CCyxvcProtectDlg::OnDropFiles(HDROP hDropInfo)
{
	UpdateData(TRUE);
	TCHAR FileName[MAX_PATH];
	DragQueryFile(hDropInfo, 0, FileName, MAX_PATH*sizeof(TCHAR));
	m_strFilePath = FileName;
	UpdateData(FALSE);
	CDialogEx::OnDropFiles(hDropInfo);
}

//************************************************************
// ��������:	GetSelect
// ����˵��:	��ȡ�ӿ�ѡ��
// ��	��:	cyxvc
// ʱ	��:	2015/12/30
// �� ��	ֵ:	BOOL
//************************************************************
BOOL CCyxvcProtectDlg::GetSelect()
{
	//��ȡ�Ƿ���ܴ����
	m_bSelect[0] = ((CButton*)GetDlgItem(IDC_CHECK1))->GetCheck();

	//��ȡ�Ƿ����IAT
	m_bSelect[1] = ((CButton*)GetDlgItem(IDC_CHECK2))->GetCheck();

	//��ȡ�Ƿ�󶨻�����
	m_bSelect[2] = ((CButton*)GetDlgItem(IDC_CHECK3))->GetCheck();
	if (m_bSelect[2])
	{
		//�жϻ�����ĳ���
		UpdateData(TRUE);
		if (m_strMachineCode.GetLength() != 32)
		{
			MessageBox(_T("������32λ�����룡"), _T("��ʾ"), MB_OK);
			return FALSE;
		}
		//��������뵽����
		
		CString strTemp;
		for (DWORD i = 0; i < 16; i++)
		{
			strTemp = m_strMachineCode[i * 2];
			strTemp += m_strMachineCode[i * 2 + 1];
			m_arrMachineCode[i] = _tcstol(strTemp, NULL, 16);
			strTemp.Empty();
		}
		UpdateData(FALSE);
	}

	//��ȡ�Ƿ���뷴����
	m_bSelect[3] = ((CButton*)GetDlgItem(IDC_CHECK4))->GetCheck();

	//��ȡ�Ƿ񵯳�MessageBox
	m_bSelect[4] = ((CButton*)GetDlgItem(IDC_CHECK5))->GetCheck();

	return TRUE;
}


void CCyxvcProtectDlg::OnBnClickedCheck3()
{
	BOOL bIsSelect = ((CButton*)GetDlgItem(IDC_CHECK3))->GetCheck();
	if (bIsSelect)
	{
		m_EditControl.EnableWindow(TRUE);
	}
	else
	{
		UpdateData(TRUE);
		m_EditControl.EnableWindow(FALSE);
		m_strMachineCode.Empty();
		UpdateData(FALSE);
	}
	
}
