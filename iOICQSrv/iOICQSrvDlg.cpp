
// iOICQSrvDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "iOICQSrv.h"
#include "iOICQSrvDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#define MAX_MSG 1024

#define WM_SOCKET WM_USER + 1
#define WM_SOCKET_UDP WM_SOCKET  +1 
// CiOICQSrvDlg �Ի���

LPSTR WideChar2MBCS(const CString& strCS)
{
	const UINT wLen = strCS.GetLength() + 1;
	UINT aLen = WideCharToMultiByte(CP_ACP, 0, strCS, wLen, NULL, 0, NULL, NULL);
	LPSTR lpa = new char[aLen];
	WideCharToMultiByte(CP_ACP, 0, strCS, wLen, lpa, aLen, NULL, NULL);
	return lpa;
}

void getTime(char * pc)
{
	time_t t;
	//struct tm *pt;
	//char pc[26];
	time(&t);
	ctime_s(pc, 26, &t);
	//return 
}

CiOICQSrvDlg::CiOICQSrvDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CiOICQSrvDlg::IDD, pParent)
	
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CiOICQSrvDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CiOICQSrvDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_SOCKET, &CiOICQSrvDlg::OnSocket)
	ON_MESSAGE(WM_SOCKET_UDP, &CiOICQSrvDlg::OnSocket_udp)


	ON_BN_CLICKED(IDC_SEND, &CiOICQSrvDlg::OnBnClickedSend)
	ON_BN_CLICKED(IDC_START_TCP, &CiOICQSrvDlg::OnBnClickedStartTcp)
	ON_BN_CLICKED(IDC_START_UDP, &CiOICQSrvDlg::OnBnClickedStartUdp)
	ON_EN_CHANGE(IDC_EDIT, &CiOICQSrvDlg::OnEnChangeEdit)
	ON_BN_CLICKED(IDC_CLEAR, &CiOICQSrvDlg::OnBnClickedClear)
END_MESSAGE_MAP()


// CiOICQSrvDlg ��Ϣ�������

BOOL CiOICQSrvDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(1, 1);
	err = WSAStartup(wVersionRequested, &wsaData);
	
	if (err != 0)//����0��ʾ�ɹ�
	{
		MessageBox(L"��ʼ��ʧ�ܣ�");
		return FALSE;
	}



	m_bar.Create(WS_CHILD | WS_VISIBLE | SBS_SIZEGRIP, CRect(0, 0, 0, 0), this, 101);
	int arWidth[] = {200, 500, -1};
	m_bar.SetParts(3, arWidth);				// ����


	// �����б��ؼ���m_listInfo����Ĺ���
	m_listInfo.SubclassDlgItem(IDC_LIST, this);
	//m_listInfo.InsertString(0, L"TEST");

	// ��ʼ�������׽��ֺ������б�
	m_socket = INVALID_SOCKET;

	m_socket_udp = INVALID_SOCKET;

	//m_nClient = 0;
	LONG styles;
	styles = GetWindowLong(tcpclient.m_hWnd, GWL_STYLE);
	SetWindowLong(tcpclient.m_hWnd, GWL_STYLE, styles | LVS_REPORT);

	styles = GetWindowLong(udpclient.m_hWnd, GWL_STYLE);
	SetWindowLong(udpclient.m_hWnd, GWL_STYLE, styles | LVS_REPORT);

	tcpclient.SubclassDlgItem(IDC_LIST_TCP, this);
	udpclient.SubclassDlgItem(IDC_LIST_UDP, this);

	tcpclient.InsertColumn(0, L"����", LVCFMT_LEFT, 80);
	tcpclient.InsertColumn(1, L"IP", LVCFMT_LEFT, 112);
	tcpclient.InsertColumn(2, L"�˿�", LVCFMT_LEFT, 60);

	udpclient.InsertColumn(0, L"����", LVCFMT_LEFT, 80);
	udpclient.InsertColumn(1, L"IP", LVCFMT_LEFT, 112);
	udpclient.InsertColumn(2, L"�˿�", LVCFMT_LEFT, 60);



	// ��ȡ����IP
	char szHost[256];
	gethostname(szHost, 256);
	// ͨ����������ȡ�õ�ַ��Ϣ
	HOSTENT *pHost = gethostbyname(szHost);
	for (int i = 0;; i++)
	{
		if (!pHost->h_addr_list[i])//rendb add
			break;
		std::string str = inet_ntoa(*(IN_ADDR*)(pHost->h_addr_list[i]));
		localIPList.push_back(str);
	}

	// ��ʾ����IP
	std::string localhostIP = "����IP: ";
	for (unsigned int i = 0; i < localIPList.size(); i++)
	{
		localhostIP += localIPList[i];
		localhostIP += "|";
	}
	localhostIP[localhostIP.size() - 1] = '\0';
	m_bar.SetText(CString(localhostIP.c_str()), 0, 0);

	me = Host(szHost, localIPList[0]);

	GetDlgItem(IDC_TCP_PORT)->SetWindowText(L"9999");
	GetDlgItem(IDC_UDP_PORT)->SetWindowText(L"9999");

	GetDlgItem(IDC_SEND)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT)->EnableWindow(FALSE);
	//GetDlgItem(IDC_LIST)->EnableWindow(FALSE);
	m_bar.SetText(L" TCPδ����", 1, 0);

	m_bar.SetText(L" UDPδ����", 2, 0);

	//OnBnClickedStart();
	return TRUE;  
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CiOICQSrvDlg::OnPaint()
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

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CiOICQSrvDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CiOICQSrvDlg::OnCancel()
{
	CloseAllSocket();
	CDialog::OnCancel();

}


long CiOICQSrvDlg::OnSocket(WPARAM wParam, LPARAM lParam)
{
	// ȡ�����¼��������׽��־��
	SOCKET s = wParam;
	SOCKET client;
	char time[26];
	Host host;
	std::vector<Host>::iterator iter;
	// �鿴�Ƿ����
	if (WSAGETSELECTERROR(lParam))
	{
		RemoveClient(s);
		closesocket(s);
		return 0;
	}
	// ���������¼�
	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_ACCEPT:		// �����е��׽��ּ�⵽�����ӽ���

		client = accept(s, NULL, NULL);								// �����µ��׽���Ϊ����֪ͨ��Ϣ����
		WSAAsyncSelect(client, m_hWnd, WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE);
		iter = AddClient(client);
		UpdateClientStatus();
		getTime(time);
		Broadcast("[TCP]" + iter->toString() + "[" + time + "] have connect with TCP!");


		break;

	case FD_CLOSE:		// ��⵽�׽��ֶ�Ӧ�����ӱ��رա�

		getTime(time);
		iter = GetClient(s);
		if (iter != hostList.end())
		{
			Broadcast("[TCP]" + iter->toString() + "[" + time + "] have disconnect!");
			RemoveClient(s);
			closesocket(s);
			UpdateClientStatus();
		}


		break;

	case FD_READ:		// �׽��ֽ��ܵ��Է����͹��������ݰ�
			host = Host(s);
			getTime(time);

			// ������������������
			char szText[1024] = { 0 };
			recv(s, szText, 1024, 0);

			// ��ʾ���û�
			Broadcast("[TCP]" + host.toString() + "[" + time + "]: " + szText);

		break;

	}
	return 0;
}

long CiOICQSrvDlg::OnSocket_udp(WPARAM wParam, LPARAM lParam)
{
	// ȡ�����¼��������׽��־��
	SOCKET s = wParam;

	char time[26];

	std::vector<Host>::iterator iter;
	// �鿴�Ƿ����
	if (WSAGETSELECTERROR(lParam))
	{
		RemoveClient_udp(s);
		closesocket(s);
		return 0;
	}
	// ���������¼�
	switch (WSAGETSELECTEVENT(lParam))
	{/*
	case FD_CLOSE:		// ��⵽�׽��ֶ�Ӧ�����ӱ��رա�

		getTime(time);
		iter = GetClient_udp(s);
		if (iter != hostList_udp.end())
		{
			Broadcast(iter->toString() + "@" + time + " have disconnect!");
			RemoveClient_udp(s);
			closesocket(s);
			UpdateClientStatus_udp();
		}


		break;
		*/
	case FD_READ:		// �׽��ֽ��ܵ��Է����͹��������ݰ�

		char szText[1024] = { 0 };





		// ����������
		sockaddr_in sin;

		/*sin.sin_family = AF_INET;
		sin.sin_port = htons(port_udp);
		sin.sin_addr.S_un.S_addr = inet_addr(iter->ip.c_str());
		*/

		int len = sizeof(sin);

		int rev = recvfrom(m_socket_udp, szText, 1024, 0, (sockaddr *)&sin, &len);


		if (rev > 0)
		{
			szText[rev] = 0x00;
			iter = GetClient_udp(sin);
			getTime(time);
			//�¼��봦��
			if (iter == hostList_udp.end())
			{
				iter = AddClient_udp(sin);
				UpdateClientStatus_udp();
				iter->socket = s;
				iter->setName();
				Broadcast("[UDP]" + iter->toString() + "[" + time + "] have connect with UDP!");
			}


			Broadcast("[UDP]" + iter->toString() + "[" + time + "]: " + szText);
		}


		CString sss("123asdas");

		std::wstring str = sss;

		std::string s = std::string(str.begin(), str.end());

		break;

	}
	return 0;
}

BOOL CiOICQSrvDlg::CreateAndListen()
{
	if (m_socket == INVALID_SOCKET)
		closesocket(m_socket);

	// �����׽���
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_socket == INVALID_SOCKET)
		return FALSE;

	// ��дҪ�����ı��ص�ַ
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = INADDR_ANY;
	// �󶨶˿�
	if (bind(m_socket, (sockaddr*)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
		return FALSE;
	}

	// ����socketΪ����֪ͨ��Ϣ����
	WSAAsyncSelect(m_socket, m_hWnd, WM_SOCKET, FD_ACCEPT | FD_CLOSE);
	// �������ģʽ
	listen(m_socket, 5);

	//��ʾ��������
	UpdateClientStatus();
	return TRUE;

}

BOOL CiOICQSrvDlg::CreateAndListen_udp()
{

	if (m_socket_udp == INVALID_SOCKET)
		closesocket(m_socket_udp);

	m_socket_udp = socket(AF_INET, SOCK_DGRAM, 0);

	if (m_socket_udp == INVALID_SOCKET)
	{
		MessageBox(L"socket error !");
		return FALSE;
	}
	sockaddr_in sin;//sockaddr_in��sockaddr�ǲ��еĽṹ
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port_udp);

	if (bind(m_socket_udp, (sockaddr*)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		closesocket(m_socket_udp);
		m_socket_udp = INVALID_SOCKET;
		return FALSE;
	}

	WSAAsyncSelect(m_socket_udp, m_hWnd, WM_SOCKET_UDP, FD_READ | FD_WRITE);

	UpdateClientStatus_udp();
	return TRUE;

}

void CiOICQSrvDlg::RemoveClient(SOCKET s)
{
	std::vector<Host>::iterator i = hostList.begin();
	for (; i < hostList.end(); i++)
	{
		if (i->socket == s)
		{
			hostList.erase(i);
			return;
		}
	}
	return;
}

void CiOICQSrvDlg::RemoveClient_udp(SOCKET s)
{
	std::vector<Host>::iterator i = hostList_udp.begin();
	for (; i < hostList_udp.end(); i++)
	{
		if (i->socket == s)
		{
			hostList_udp.erase(i);
			return;
		}
	}
	return;
}

std::vector<Host>::iterator CiOICQSrvDlg::AddClient(SOCKET& s)
{
	hostList.push_back(s);
	return hostList.end() - 1;
}

std::vector<Host>::iterator CiOICQSrvDlg::AddClient_udp(sockaddr_in sin)
{
	hostList_udp.push_back(sin);
	return hostList_udp.end() - 1;
}

std::vector<Host>::iterator CiOICQSrvDlg::GetClient(SOCKET& s)
{
	std::vector<Host>::iterator i = hostList.begin();
	for (; i < hostList.end(); i++)
	{
		if (i->socket == s)
		{
			return i;
		}
	}
	return hostList.end();
}

std::vector<Host>::iterator CiOICQSrvDlg::GetClient_udp(sockaddr_in sin)
{
	std::vector<Host>::iterator i = hostList_udp.begin();
	char * a = inet_ntoa(sin.sin_addr);
	int port = sin.sin_port;

	for (; i < hostList_udp.end(); i++)
	{
		char * b = inet_ntoa(i->sin.sin_addr);

		if ((strcmp(a, b) == 0) && port == i->port)
		{
			return i;
		}
	}
	return hostList_udp.end();
}

void CiOICQSrvDlg::CloseAllSocket()
{
	// �رռ����׽���
	if (m_socket != INVALID_SOCKET)
	{
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}

	// �ر����пͻ�������
	for (unsigned int i = 0; i < hostList.size(); i++)
	{
		closesocket(hostList[i].socket);
	}
	hostList.clear();
}

void CiOICQSrvDlg::CloseAllSocket_udp()
{
	// �رռ����׽���
	if (m_socket_udp != INVALID_SOCKET)
	{
		closesocket(m_socket_udp);
		m_socket_udp = INVALID_SOCKET;
	}

	// �ر����пͻ�������
	for (unsigned int i = 0; i < hostList_udp.size(); i++)
	{
		closesocket(hostList_udp[i].socket);
	}
	hostList_udp.clear();
}

void CiOICQSrvDlg::UpdateClientStatus()
{
	std::string str = "TCP������ ���߿ͻ���: ";
	//std::string str = "";

	std::stringstream ss;
	std::string num;
	ss << hostList.size();
	ss >> num;

	m_bar.SetText(CString((str + num).c_str()),1,0);


	/*for (std::vector<Host>::iterator iter = hostList.begin(); iter < hostList.end; iter++)
	{
		tcpclient.InsertItem(0,CString(iter->name.c_str()));
	}*/

	tcpclient.DeleteAllItems();
	for (unsigned int i = 0; i < hostList.size(); i++)
	{
		tcpclient.InsertItem(i, CString(hostList[i].name.c_str()));
		tcpclient.SetItemText(i, 1, CString(hostList[i].ip.c_str()));

		std::stringstream ss;
		std::string num;
		ss << hostList[i].port;
		ss >> num;

		tcpclient.SetItemText(i, 2, CString(num.c_str()));
	}


	//ss << hostList_udp.size();
	//ss >> num;

	//m_bar.SetText(CString((str + num).c_str()),2, 0);
	
	
}


void CiOICQSrvDlg::UpdateClientStatus_udp()
{
	std::string str = "UDP������ ��ͻ���: ";
	//std::string str = "";

	std::stringstream ss;
	std::string num;
	//ss << hostList.size();
	//ss >> num;

	//m_bar.SetText(CString((str + num).c_str()), 1, 0);

	ss << hostList_udp.size();
	ss >> num;

	m_bar.SetText(CString((str + num).c_str()), 2, 0);


	udpclient.DeleteAllItems();
	for (unsigned int i = 0; i < hostList_udp.size(); i++)
	{
		udpclient.InsertItem(i, CString(hostList_udp[i].name.c_str()));
		udpclient.SetItemText(i, 1, CString(hostList_udp[i].ip.c_str()));

		std::stringstream ss;
		std::string num;
		ss << hostList_udp[i].port;
		ss >> num;

		udpclient.SetItemText(i, 2, CString(num.c_str()));
	}



}

void CiOICQSrvDlg::Broadcast(std::string msg)
{

	for (unsigned int i = 0; i < hostList.size(); i++)
	{
		send(hostList[i].socket, msg.c_str(), strlen(msg.c_str()) + 1, 0);
	}

	for (unsigned int i = 0; i < hostList_udp.size(); i++)
	{

		sockaddr_in sin;
		sin.sin_family = AF_INET;
		sin.sin_port = hostList_udp[i].port;
		sin.sin_addr.S_un.S_addr = inet_addr((hostList_udp[i].ip).c_str());
		int len = sizeof(hostList_udp[i].sin);
		sendto(m_socket_udp,msg.c_str(), strlen(msg.c_str()) + 1, 0, (sockaddr *)&(sin), len);

	}

	m_listInfo.InsertString(-1, CString(msg.c_str()));
}




void CiOICQSrvDlg::OnBnClickedSend()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	if (!GetDlgItem(IDC_SEND)->IsWindowEnabled())return;
	CString sText;
	GetDlgItem(IDC_EDIT)->GetWindowText(sText);


	LPSTR lp = WideChar2MBCS(sText);
	char time[26];
	getTime(time);
	Broadcast("[SERVER][" + localIPList[0] + "][" + time + "]: " + lp);
	delete lp;
	GetDlgItem(IDC_EDIT)->SetWindowText(L"");

}


void CiOICQSrvDlg::OnBnClickedStartTcp()
{

	if (m_socket == INVALID_SOCKET)  // ��������
	{

		CString sPort;
		GetDlgItem(IDC_TCP_PORT)->GetWindowText(sPort);
		char * lp = WideChar2MBCS(sPort);
		port = atoi(lp);

		delete lp;


		if (port < 1 || port > 65535)
		{
			MessageBox(L"�˿ںŴ���");
			return;
		}

		// ���������׽��֣�ʹ���������״̬
		if (!CreateAndListen())
		{
			MessageBox(L"�����������");
			return;
		}

		// ��������Ӵ��ڿؼ�״̬
		GetDlgItem(IDC_START_TCP)->SetWindowText(L"ֹͣTCP����");

		GetDlgItem(IDC_TCP_PORT)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT)->EnableWindow(TRUE);

	}
	else				// ֹͣ����
	{
		tcpclient.DeleteAllItems();

		CloseAllSocket();

		GetDlgItem(IDC_START_TCP)->SetWindowText(L"��TCP����");

		GetDlgItem(IDC_TCP_PORT)->EnableWindow(TRUE);

		m_bar.SetText(L"TCPδ����", 1, 0);

		if (m_socket_udp == INVALID_SOCKET)
		{
			GetDlgItem(IDC_EDIT)->EnableWindow(FALSE);
		}

	}


}


void CiOICQSrvDlg::OnBnClickedStartUdp()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	//m_bar.SetText(L" ������...", 2, 0);

	if (m_socket_udp == INVALID_SOCKET)  // ��������
	{

		CString sPort;
		GetDlgItem(IDC_UDP_PORT)->GetWindowText(sPort);
		char * lp = WideChar2MBCS(sPort);
		port_udp = atoi(lp);
		delete lp;

		if (port_udp < 1 || port_udp > 65535)
		{
			MessageBox(L"�˿ںŴ���");
			return;
		}

		if (!CreateAndListen_udp())
		{
			MessageBox(L"�����������");
			return;
		}

		GetDlgItem(IDC_START_UDP)->SetWindowText(L"ֹͣUDP����");
		GetDlgItem(IDC_UDP_PORT)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT)->EnableWindow(TRUE);

	}
	else				// ֹͣ����
	{
		udpclient.DeleteAllItems();
		CloseAllSocket_udp();
		GetDlgItem(IDC_START_UDP)->SetWindowText(L"��UDP����");
		GetDlgItem(IDC_UDP_PORT)->EnableWindow(TRUE);

		m_bar.SetText(L"UDPδ����", 2, 0);
		
		if (m_socket == INVALID_SOCKET)
		{
			GetDlgItem(IDC_EDIT)->EnableWindow(FALSE);
		}

	}
}


void CiOICQSrvDlg::OnEnChangeEdit()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	CString sText;
	GetDlgItem(IDC_EDIT)->GetWindowText(sText);
	if (sText.GetLength() < 1 || sText.GetLength() > MAX_MSG)
	{
		GetDlgItem(IDC_SEND)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_SEND)->EnableWindow(TRUE);
	}
}


void CiOICQSrvDlg::OnBnClickedClear()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	m_listInfo.ResetContent();

}


BOOL CiOICQSrvDlg::PreTranslateMessage(MSG* pMsg)
{
	//if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) OnBnClickedSend();
	else
		return CDialog::PreTranslateMessage(pMsg);
}