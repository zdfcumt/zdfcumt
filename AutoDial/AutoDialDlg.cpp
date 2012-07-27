
// AutoDialDlg.cpp : 实现文件
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
// CAutoDialDlg 对话框




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


// CAutoDialDlg 消息处理程序

BOOL CAutoDialDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CAutoDialDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CAutoDialDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CAutoDialDlg::OnBnClickedBtnStart()
{
	UpdateData(TRUE);
	if (m_csEntryName.IsEmpty()){
		AfxMessageBox("请输入拨号连接名称！");
		m_csStatus = "请输入拨号连接名称！";
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
//		m_csStatus = "Empty EntryName！";
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
//	if (dwConnCount > 0){//有连接直接退出
//		m_csStatus.Format("Connected");
//		return TRUE;
//		//RasHangUp(rasconn.hrasconn);   
//	}
//
//	//没有连接
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
		m_csStatus = "Empty EntryName！";
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

	if (dwConnCount > 0){//有连接直接退出
		m_csStatus.Format("Connected");
		return TRUE;
		//RasHangUp(rasconn.hrasconn);   
	}

	//没有连接
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
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == s_dwTimeEvent){
		AutoCheck();
		UpdateData(FALSE);
	}
	CDialog::OnTimer(nIDEvent);
}
