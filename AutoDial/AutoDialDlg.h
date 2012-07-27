
// AutoDialDlg.h : 头文件
//

#pragma once

#define WM_TRAYCLICK	WM_USER + 1

// CAutoDialDlg 对话框
class CAutoDialDlg : public CDialog
{
// 构造
public:
	CAutoDialDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_AUTODIAL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
