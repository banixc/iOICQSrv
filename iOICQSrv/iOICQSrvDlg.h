
// iOICQSrvDlg.h : ͷ�ļ�
//

#pragma once
#include <vector>
#include "afxwin.h"

// CiOICQSrvDlg �Ի���
class CiOICQSrvDlg : public CDialogEx
{
// ����
public:
	CiOICQSrvDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_IOICQSRV_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// �����Ӵ��ڿؼ���һ����״̬����һ�����б��
	CStatusBarCtrl m_bar;
	CListBox m_listInfo;
	std::vector<std::string> localIPList;
	Host me;

	// �����׽��־��

	int port;

	SOCKET m_socket;

	// �ͻ������б�
	std::vector<Host> hostList;

	//TCP
	// �����׽��֣�������Ϊ����״̬��׼�����ܿͻ�������
	BOOL CreateAndListen();
	// �ر������׽��֣����������׽��ֺ�����accept�������ص��׽���
	void CloseAllSocket();
	// ��ͻ������б������һ���ͻ�
	std::vector<Host>::iterator AddClient(SOCKET&);
	std::vector<Host>::iterator GetClient(SOCKET&);


	// �ӿͻ������б����ƴ�һ���ͻ�
	void RemoveClient(SOCKET);

	void UpdateClientStatus();

	void Broadcast(std::string);

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnCancel();


	CListCtrl tcpclient;
	CListCtrl udpclient;


	//UDP
	SOCKET m_socket_udp;

	std::vector<Host> hostList_udp;
	BOOL CreateAndListen_udp();
	void CloseAllSocket_udp();
	std::vector<Host>::iterator AddClient_udp(sockaddr_in);
	std::vector<Host>::iterator GetClient_udp(sockaddr_in);


	void UpdateClientStatus_udp();

	void RemoveClient_udp(SOCKET);

	int port_udp;
	//void UpdateClientStatus();


	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	//afx_msg void OnBnClickedStart();
	afx_msg long OnSocket(WPARAM wParam, LPARAM lParam);
	afx_msg long OnSocket_udp(WPARAM wParam, LPARAM lParam);


	afx_msg void OnBnClickedSend();
	afx_msg void OnBnClickedStartTcp();
	afx_msg void OnBnClickedStartUdp();
	afx_msg void OnEnChangeEdit();
	afx_msg void OnBnClickedClear();
};
