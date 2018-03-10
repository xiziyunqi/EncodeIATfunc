
// CyxvcProtectDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"


// CCyxvcProtectDlg �Ի���
class CCyxvcProtectDlg : public CDialogEx
{
// ����
public:
	CCyxvcProtectDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_CYXVCPROTECT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedCheck3();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	DECLARE_MESSAGE_MAP()
public:
	CString m_strFilePath;					//���ӿ��ļ�·��
	CString m_strMachineCode;					//�������ַ���
	CHAR m_arrMachineCode[16];					//����������
	CEdit m_EditControl;					//������EditControl�ؼ�����
	BOOL m_bSelect[5];						//�ӿ�ѡ��
	afx_msg void OnBnClicked_OpenFile();	//���ļ���ť
	afx_msg void OnBnClicked_Pack();		//�ӿǰ�ť
	BOOL GetSelect();						//��ȡ�ӿ�ѡ��				
};
