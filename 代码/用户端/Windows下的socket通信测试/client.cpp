//#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>						//包含WinSock2.h头文件
#pragma comment(lib, "ws2_32.lib")			//加载 ws2_32.dll 
int main() {
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);		//初始化版本
	SOCKET sock = socket(PF_INET, SOCK_STREAM, 0);		//创建用户端socket

	//向服务器发起请求
	sockaddr_in sockAddr;								//创建sockAddr结构体
	memset(&sockAddr, 0, sizeof(sockAddr));				 //每个字节都用0填充
	sockAddr.sin_family = PF_INET;
	sockAddr.sin_port = htons(28888);
	sockAddr.sin_addr.s_addr = inet_addr("192.168.178.128");

	connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));		//建立链接
	printf("客户端发送链接请求\n");


	//接收服务器传回的数据
	char szBuffer[MAXBYTE] = { 0 };
	scanf("%s", szBuffer);
	send(sock, szBuffer, strlen(szBuffer), 0);
	recv(sock, szBuffer, MAXBYTE, NULL);
	printf("接收服务器传回的消息函数\n");

	//输出接收到的数据
	printf("服务器: %s\n", szBuffer);

	//关闭套接字、终止使用 DLL
	closesocket(sock);
	WSACleanup();
	system("PAUSE");
	return 0;
}