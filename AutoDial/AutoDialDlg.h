
// AutoDialDlg.h : 头文件
//

#pragma once


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
public:

	CString m_csEntryName;
	afx_msg void OnBnClickedBtnStart();

	//False：Encounter Errors 
	BOOL AutoCheck();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CString m_csStatus;
};
