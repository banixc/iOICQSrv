
// iOICQSrvDlg.h : 头文件
//

#pragma once
#include <vector>
#include "afxwin.h"

// CiOICQSrvDlg 对话框
class CiOICQSrvDlg : public CDialogEx
{
// 构造
public:
	CiOICQSrvDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_IOICQSRV_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 两个子窗口控件，一个是状态栏，一个是列表框
	CStatusBarCtrl m_bar;
	CListBox m_listInfo;
	std::vector<std::string> localIPList;
	Host me;

	// 监听套节字句柄

	int port;

	SOCKET m_socket;

	// 客户连接列表
	std::vector<Host> hostList;

	//TCP
	// 创建套节字，并设置为监听状态，准备接受客户的连接
	BOOL CreateAndListen();
	// 关闭所有套节字，包括监听套节字和所有accept函数返回的套节字
	void CloseAllSocket();
	// 向客户连接列表中添加一个客户
	std::vector<Host>::iterator AddClient(SOCKET&);
	std::vector<Host>::iterator GetClient(SOCKET&);


	// 从客户连接列表中移处一个客户
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


	// 生成的消息映射函数
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
