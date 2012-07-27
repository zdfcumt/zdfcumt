
// AutoDialDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AutoDial.h"
#include "AutoDialDlg.h"
#include <wininet.h>
#pragma comment(lib,"Wininet.lib") 
#include <ras.h>
#include <RasError.h>
#pragma comment(lib,"rasapi32.lib") 
#pragma comment( lib, "ws2_32.lib" )


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



namespace{
	static const DWORD s_dwTimeEvent	= 1000;
	static const DWORD s_dwTimeElapse	= 10 * 1000;	//10s
}
// CAutoDialDlg �Ի���




CAutoDialDlg::CAutoDialDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAutoDialDlg::IDD, pParent)
	, m_csEntryName(_T("test"))
	, m_csStatus(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAutoDialDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDT_ENTRYNAME, m_csEntryName);
	DDX_Text(pDX, IDC_EDT_STATUS, m_csStatus);
}

BEGIN_MESSAGE_MAP(CAutoDialDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP

	ON_BN_CLICKED(IDC_BTN_START, &CAutoDialDlg::OnBnClickedBtnStart)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CAutoDialDlg ��Ϣ�������

BOOL CAutoDialDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CAutoDialDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CAutoDialDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CAutoDialDlg::OnBnClickedBtnStart()
{
	UpdateData(TRUE);
	if (m_csEntryName.IsEmpty()){
		AfxMessageBox("�����벦���������ƣ�");
		m_csStatus = "�����벦���������ƣ�";
		UpdateData(FALSE);
		return;
	}

	AutoCheck();
	UpdateData(FALSE);

	SetTimer(s_dwTimeEvent, s_dwTimeElapse, NULL);
	
	//AutoCheck();
}
//
//BOOL CAutoDialDlg::AutoCheck()
//{
//	m_csStatus = "";
//	if (m_csEntryName.IsEmpty()){
//		m_csStatus = "Empty EntryName��";
//		return FALSE;
//	}
//
//	RASCONN		rasconn;   
//	DWORD		dwConnCount(0);   
//	DWORD		dwSize(0);
//	DWORD		dwRet(0);   
//
//	rasconn.dwSize	= 0x19c;//sizeof(RASCONN);   
//	dwSize			= rasconn.dwSize;   
//	dwRet			= RasEnumConnections(&rasconn, &dwSize, &dwConnCount);	
//	if (ERROR_SUCCESS != dwRet){
//		m_csStatus.Format("RasEnumConn Fail:%d", dwRet);
//		return FALSE;
//	}
//
//	if (dwConnCount > 0){//������ֱ���˳�
//		m_csStatus.Format("Connected");
//		return TRUE;
//		//RasHangUp(rasconn.hrasconn);   
//	}
//
//	//û������
//	DWORD dwConn(0);
//	dwRet = InternetDial(m_hWnd, (char*)(LPCTSTR)m_csEntryName , 0x8000, &dwConn, 0);
//	if((ERROR_SUCCESS == dwRet) && dwConn){
//		m_csStatus.Format("Connected");
//		return TRUE;
//	}
//	return TRUE;
//}



BOOL CAutoDialDlg::AutoCheck()
{
	m_csStatus = "";
	if (m_csEntryName.IsEmpty()){
		m_csStatus = "Empty EntryName��";
		return FALSE;
	}

	RASCONN		rasconn;   
	DWORD		dwConnCount(0);   
	DWORD		dwSize(0);
	DWORD		dwRet(0);   

	rasconn.dwSize	= 0x19c;//sizeof(RASCONN);   
	dwSize			= rasconn.dwSize;   
	dwRet			= RasEnumConnections(&rasconn, &dwSize, &dwConnCount);	
	if (ERROR_SUCCESS != dwRet){
		m_csStatus.Format("RasEnumConn Fail:%d", dwRet);
		return FALSE;
	}

	if (dwConnCount > 0){//������ֱ���˳�
		m_csStatus.Format("Connected");
		return TRUE;
		//RasHangUp(rasconn.hrasconn);   
	}

	//û������
	DWORD			dwConn(0);
	RASDIALPARAMS	stRasParam;
	BOOL			bHasPw;
	stRasParam.dwSize = sizeof(RASDIALPARAMS);
	strcpy_s(stRasParam.szEntryName, RAS_MaxEntryName, m_csEntryName.GetBuffer());
	
	dwRet = RasGetEntryDialParams(NULL, &stRasParam, &bHasPw);
	if (ERROR_SUCCESS != dwRet){
		m_csStatus.Format("GetEntry Fail:%d", dwRet);
		return FALSE;
	}

	HRASCONN hRasConn(NULL);
	dwRet = RasDial(NULL, NULL, &stRasParam, 0, NULL, &hRasConn);
	if (ERROR_SUCCESS != dwRet){
		m_csStatus.Format("RasDial Fail:%d", dwRet);
		return FALSE;
	}
	return TRUE;
}


void CAutoDialDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (nIDEvent == s_dwTimeEvent){
		AutoCheck();
		UpdateData(FALSE);
	}
	CDialog::OnTimer(nIDEvent);
}
