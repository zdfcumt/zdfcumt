
// AutoDialDlg.h : ͷ�ļ�
//

#pragma once


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
public:

	CString m_csEntryName;
	afx_msg void OnBnClickedBtnStart();

	//False��Encounter Errors 
	BOOL AutoCheck();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CString m_csStatus;
};
