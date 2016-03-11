#include "stdafx.h"
#include "Host.h"

Host::Host()
{
	this->name = "Localhost";
	this->ip = "127.0.0.2";
	//this->id = num++;
}



Host::Host(std::string name, std::string ip)
{
	this->name = name;
	this->ip = ip;
	//this->id = num++;

}


Host::Host(std::string name, std::string ip, SOCKET& socket)
{
	this->name = name;
	this->ip = ip;
	this->socket = socket;
	//this->id = num++;
}



Host::Host(SOCKET& s)
{

	//this->id = num++;


	this->socket = s;
	//Host host;
	// ȡ�öԷ���IP��ַ�Ͷ˿ںţ�ʹ��getpeername������
	// Peer�Է��ĵ�ַ��Ϣ
	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	int nSockAddrLen = sizeof(sockAddr);
	getpeername(socket, (SOCKADDR*)&sockAddr, &nSockAddrLen);
	// ת��Ϊ�����ֽ�˳��
	int nPeerPort = ntohs(sockAddr.sin_port);
	// ת��Ϊ�ַ���IP
	//CString sPeerIP;
	char sPeerIP[20];
	sprintf_s(sPeerIP, "%s", inet_ntoa(sockAddr.sin_addr));
	//sPeerIP.Format(L"%s", inet_ntoa(sockAddr.sin_addr));

	this->ip = sPeerIP;

	this->port = nPeerPort;

	// ȡ�öԷ�����������
	// ȡ�������ֽ�˳���IPֵ
	DWORD dwIP = inet_addr(sPeerIP);
	// ��ȡ�������ƣ�ע�����е�һ��������ת��
	hostent* pHost = gethostbyaddr((LPSTR)&dwIP, 4, AF_INET);

	//char szHostName[256];

	if (pHost == NULL)
	{
		//strncpy_s(szHostName, "Unknow!", 256);
		this->name = "Unknow";
	}
	else
	{
		//strncpy_s(szHostName, pHost->h_name, 256);
		this->name = pHost->h_name;
	}



}
/*
Host::Host(SOCKET& s, sockaddr_in sin)
{
	Host(s);
	this->sin = sin;
}*/

Host::Host(sockaddr_in& s)
{
	port = s.sin_port;
	//sin.

	//sockaddr_in sockAddr;
	//memset(&s, 0, sizeof(sin));
	//int nSockAddrLen = sizeof(sin);
	//getpeername(socket, (SOCKADDR*)&sin, &nSockAddrLen);
	// ת��Ϊ�����ֽ�˳��
	//int nPeerPort = ntohs(sin.sin_port);
	// ת��Ϊ�ַ���IP
	//CString sPeerIP;
	char sPeerIP[20];
	sprintf_s(sPeerIP, "%s", inet_ntoa(s.sin_addr));
	//sPeerIP.Format(L"%s", inet_ntoa(sockAddr.sin_addr));

	ip = sPeerIP;







}


Host::~Host()
{
}


std::string Host::toString()
{

	std::stringstream ss;
	std::string num;
	ss << port;//hostList.size();
	ss >> num;
	return "[" + name + "][" + ip + ":" + num + "]";

}


void Host::setName()
{

	// ȡ�öԷ�����������
	// ȡ�������ֽ�˳���IPֵ
	DWORD dwIP = inet_addr(ip.c_str());
	// ��ȡ�������ƣ�ע�����е�һ��������ת��
	hostent* pHost = gethostbyaddr((LPSTR)&dwIP, 4, AF_INET);

	//char szHostName[256];

	if (pHost == NULL)
	{
		//strncpy_s(szHostName, "Unknow!", 256);
		this->name = "Unknow";
	}
	else
	{
		//strncpy_s(szHostName, pHost->h_name, 256);
		this->name = pHost->h_name;
	}
}