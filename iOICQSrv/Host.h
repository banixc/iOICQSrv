#pragma once
#include "stdafx.h"



class Host
{
public:
	//static int num;
	//int id;
	Host();
	Host(SOCKET&);
	Host(sockaddr_in&);
	//Host(SOCKET&, sockaddr_in);
	int port;

	Host(std::string, std::string);
	Host(std::string, std::string, SOCKET&);
	std::string toString();
	~Host();
	//protect:
	std::string name;
	std::string ip;
	SOCKET socket;
	sockaddr_in sin;

	/*bool operator== (const Host& host2)
	{
		return this->id == host2.id;
	}*/

	void setName();

	//SOCKET socket;
};

//int Host::num = 0;