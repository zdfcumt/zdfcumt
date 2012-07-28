
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
	static const CString s_csEntryName	= "Home";
	static const char*	s_szTip[2]		= {"ZAutoDial:����������", "ZAutoDial:������������"};
	static const CString s_csConfFile	= "conf.ini";
}
// CAutoDialDlg �Ի���




CAutoDialDlg::CAutoDialDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAutoDialDlg::IDD, pParent)
	, m_csEntryName(s_csEntryName)
	, m_csStatus(_T(""))
	, m_bConn(TRUE)
	, m_csUserName("")
	, m_csPassWord("")
	, m_csCmdLine("")
	, m_dwTimeElapse(10)
{
	m_uICON[0] = IDI_CONN;
	m_uICON[1] = IDI_DISCONN;
	m_hICON[0] = AfxGetApp()->LoadIcon(m_uICON[0]/*IDR_MAINFRAME*/);
	m_hICON[1] = AfxGetApp()->LoadIcon(m_uICON[1]);
	m_hIcon = m_hICON[0];
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
	ON_WM_NCHITTEST()

	ON_MESSAGE(WM_TRAYCLICK,OnTrayClick)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CAutoDialDlg ��Ϣ�������

BOOL CAutoDialDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	SetWindowPos(NULL,0,0,0,0,NULL);
	SetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE,WS_EX_TOOLWINDOW|WS_EX_STATICEDGE); 
	
	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	if (0 == GetModuleFileName(NULL, m_szAppPath, MAX_PATH)){
		AfxMessageBox("��ȡ��������·��ʧ��");
		EndDialog(IDCANCEL);
		return FALSE;
	}
	if (!LoadConf()){
		AfxMessageBox("���������ļ�ʧ��");
		EndDialog(IDCANCEL);
		return FALSE;
	}

	RunWhenStart(TRUE);

	SetWindowText("ZAutoDial");
	m_bConn = AutoCheck();
	SetTimer(s_dwTimeEvent, m_dwTimeElapse * 1000, NULL);
	ToTray(m_bConn);
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

	SetTimer(s_dwTimeEvent, m_dwTimeElapse, NULL);
	
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
	//DWORD			dwConn(0);
	//RASDIALPARAMS	stRasParam;
	//BOOL			bHasPw;
	//stRasParam.dwSize = sizeof(RASDIALPARAMS);
	//strcpy_s(stRasParam.szEntryName, RAS_MaxEntryName, m_csEntryName.GetBuffer());
	//
	//dwRet = RasGetEntryDialParams(NULL, &stRasParam, &bHasPw);
	//if (ERROR_SUCCESS != dwRet){
	//	m_csStatus.Format("GetEntry Fail:%d", dwRet);
	//	return FALSE;
	//}

	//HRASCONN hRasConn(NULL);
	//dwRet = RasDial(NULL, NULL, &stRasParam, 0, NULL, &hRasConn);
	//if (ERROR_SUCCESS != dwRet){
	//	m_csStatus.Format("RasDial Fail:%d", dwRet);
	//	return FALSE;
	//}

	WinExec(m_csCmdLine, SW_HIDE);
	
	return FALSE;
}


void CAutoDialDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (nIDEvent == s_dwTimeEvent){
		BOOL bRet = AutoCheck();
		if (m_bConn != bRet){
			m_bConn = bRet;
			ModifyTray(m_bConn);
			UpdateData(FALSE);
		}
		
	}
	CDialog::OnTimer(nIDEvent);
}

LRESULT CAutoDialDlg::OnNcHitTest(CPoint point)
{
    CRect rc;
    GetClientRect(&rc);
    ClientToScreen(&rc);
    return rc.PtInRect(point) ? HTCAPTION : CDialog::OnNcHitTest(point);
	//return CDialog::OnNcHitTest(point);
}

LRESULT CAutoDialDlg::OnTrayClick(WPARAM wParam,LPARAM lParam)
{
	if (wParam != IDI_CONN && wParam != IDI_DISCONN) return 1;
	switch (lParam)
	{
	case WM_RBUTTONUP:
		{
			LPPOINT lpoint=new tagPOINT;
			::GetCursorPos(lpoint);//�õ����λ��
			CMenu menu;
			menu.CreatePopupMenu();//����һ������ʽ�˵�
			menu.AppendMenu(MF_STRING,WM_DESTROY,"�˳�");
			menu.TrackPopupMenu(TPM_LEFTALIGN,lpoint->x,lpoint->y,this);
			HMENU hmenu=menu.Detach();
			menu.DestroyMenu();
			delete lpoint;
		}
		break;
	case WM_LBUTTONDBLCLK:
			this->ShowWindow(SW_SHOW);//�򵥵���ʾ���������¶�
			break;
	default:
		return 0;
		break;
	}
	return 0;
}


void CAutoDialDlg::ToTray(BOOL bConn)
{
	int nIdx	= (bConn ? 0 : 1);
	
	m_Nid.cbSize	= (DWORD)sizeof(NOTIFYICONDATA);
	m_Nid.hWnd		= this->m_hWnd;
	m_Nid.uFlags	= NIF_ICON|NIF_MESSAGE|NIF_TIP ;	
	m_Nid.uCallbackMessage	= WM_TRAYCLICK;	//�Զ������Ϣ����
	m_Nid.uID		= m_uICON[0];
	m_Nid.hIcon		= m_hICON[nIdx];	
	strcpy(m_Nid.szTip, s_szTip[nIdx]);			//��Ϣ��ʾ��Ϊ�������Զ��ز���
	Shell_NotifyIcon(NIM_ADD, &m_Nid);				//�����������ͼ��
	ShowWindow(SW_HIDE);//����������
}


void CAutoDialDlg::DeleteTray()
{	  
    Shell_NotifyIcon(NIM_DELETE,&m_Nid);    //��������ɾ��ͼ�� 
 }

void CAutoDialDlg::ModifyTray(BOOL bConn)
{
	int nIdx	= (bConn ? 0 : 1);
	m_Nid.uID		= m_uICON[0];
	m_Nid.hIcon		= m_hICON[nIdx];	
	strcpy(m_Nid.szTip, s_szTip[nIdx]);			//��Ϣ��ʾ��Ϊ�������Զ��ز���
	Shell_NotifyIcon(NIM_MODIFY,&m_Nid);    //��������ɾ��ͼ�� 
}
BOOL CAutoDialDlg::DestroyWindow()
{
	DeleteTray();
	return CDialog::DestroyWindow();
}

BOOL CAutoDialDlg::LoadConf()
{
	CString csConfFile(m_szAppPath);
	if (!csConfFile.IsEmpty()){
		csConfFile.Replace('\\', '/');
		if (csConfFile.ReverseFind('/')){
			csConfFile = csConfFile.Mid(0, csConfFile.ReverseFind('/') + 1) + s_csConfFile;
		}else{
			return FALSE;
		}
	}else{
		csConfFile = s_csConfFile;
	}

	char szEntryName[MAX_PATH] = {0};
	char szUserName[MAX_PATH] = {0};
	char szPassWord[MAX_PATH] = {0};

	GetPrivateProfileString("MAIN", "EntryName", "", szEntryName, MAX_PATH - 1, csConfFile);
	GetPrivateProfileString("MAIN", "UserName", "", szUserName, MAX_PATH - 1, csConfFile);
	GetPrivateProfileString("MAIN", "PassWord", "", szPassWord, MAX_PATH - 1, csConfFile);
	m_dwTimeElapse = GetPrivateProfileInt("MAIN", "Elapse", 10, csConfFile);
	
	m_csEntryName	= szEntryName;
	m_csUserName	= szUserName;
	m_csPassWord	= szPassWord;

	if (m_csEntryName.IsEmpty() || m_csUserName.IsEmpty()){
		return FALSE;
	}

	m_csCmdLine.Format("rasdial %s %s %s", m_csEntryName, m_csUserName, m_csPassWord); 
	return TRUE;
}


BOOL CAutoDialDlg::RunWhenStart(BOOL bStart)
{
	const char* lpszRegPath = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
	const char* lpszAppKeyName = "ZAutoDial";

	LONG nResult;
	HKEY hRegKey = NULL;
	//WriteLogMessage(0, _T("���Դ�ע���"));
	nResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpszRegPath, 0, KEY_ALL_ACCESS, &hRegKey);
	if (nResult == ERROR_SUCCESS)
	{
		//WriteLogMessage(0, _T("��ע���ɹ���"));
		if (bStart)
		{
			//WriteLogMessage(0, _T("���������������"));
			nResult = RegSetValueEx(hRegKey, lpszAppKeyName, 0, REG_SZ, (const unsigned char *)m_szAppPath, sizeof(m_szAppPath));//m_szAppPath��Ҫ���������ĳ����·��
			if (nResult == ERROR_SUCCESS){
				//WriteLogMessage(0, _T("���óɹ���"));
			}else{
				//WriteLogMessage(0, _T("����ʧ�ܡ�"));
			}
		}
		else
		{
			//WriteLogMessage(0, _T("ȡ�������������"));
			nResult = RegDeleteValue(hRegKey, lpszAppKeyName);
			if (nResult == ERROR_SUCCESS){
				//WriteLogMessage(0, _T("���óɹ���"));
			}else{
				char szBuffer[1024];
				FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
				NULL, nResult, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				szBuffer, sizeof(szBuffer), NULL);
				//szLogMessage.Format("ɾ��ע�����ʧ�ܣ�[%s]", szBuffer);
				//WriteLogMessage(0, szLogMessage);
			}
		}
		RegCloseKey(hRegKey);
	}
	else
	{
		//WriteLogMessage(0, _T("��ע���ʧ�ܡ�"));
	}

	if (nResult == ERROR_SUCCESS)
		return TRUE;
	else
		return FALSE;
}