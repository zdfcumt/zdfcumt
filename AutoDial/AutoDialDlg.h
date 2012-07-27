
// AutoDialDlg.h : ͷ�ļ�
//

#pragma once

#define WM_TRAYCLICK	WM_USER + 1

// CAutoDialDlg �Ի���
class CAutoDialDlg : public CDialog
{
// ����
public:
	CAutoDialDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_AUTODIAL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	BOOL m_bConn;
	HICON m_hICON[2];
	UINT  m_uICON[2];
	CString m_csStatus;
	CString m_csEntryName;
	CString m_csUserName;
	CString m_csPassWord;
	CString m_csCmdLine;
	NOTIFYICONDATA m_Nid;
	TCHAR m_szAppPath[MAX_PATH];
	DWORD m_dwTimeElapse;//(s)
public:

	
	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnTimer(UINT_PTR nIDEvent);	
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg LRESULT OnTrayClick(WPARAM wParam,LPARAM lParam);
	virtual BOOL DestroyWindow();

	void ToTray(BOOL bConn);
	void DeleteTray();
	void ModifyTray(BOOL bConn);
	BOOL AutoCheck();
	BOOL RunWhenStart(BOOL bStart);
	BOOL LoadConf();
	
};
