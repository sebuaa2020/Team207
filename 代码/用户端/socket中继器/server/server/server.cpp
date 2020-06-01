//#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <thread>
#include <winsock2.h>							//包含socket的头文件
#pragma comment (lib, "ws2_32.lib")				//加载 ws2_32.dll

SOCKET servSock;
SOCKET appSock;
SOCKET rosSock;

void app_to_ros();
void ros_to_app();


int main() {
	WSADATA wsaData;							//定义一个结构体对象
	WSAStartup(MAKEWORD(2, 2), &wsaData);		//初始化WSAStartup()函数,,,(规范的版本号，指向WSADATA结构体的指针),,,MSKEWORD(2,2)主版本号2，副版本号2
	FILE *socket_config;
	socket_config = fopen("socket_config.txt", "r");
	int ros_port, trans_port;
	char ros_ip[16], trans_ip[16], c;
	fscanf(socket_config, "ros_ip=%s\n", ros_ip);
	fscanf(socket_config, "ros_port=%d\n", &ros_port);
	fscanf(socket_config, "trans_ip=%s\n", trans_ip);
	fscanf(socket_config, "trans_port=%d\n", &trans_port);
	fclose(socket_config);
	
	//作为服务端与app连接的socket
	servSock = socket(PF_INET, SOCK_STREAM, 0);		//参数（1）IP地址类型PF_INET6为IPv6，（2）数据传输方式 SOCK_STREAM 和 SOCK_DGRAM （3）传输协议 IPPROTO_TCP 和 IPPTOTO_UDP，如果写0系统会自动计算处使用那种协议
	sockaddr_in transSockAddr;										//创建sockaddr_in结构体变量
	memset(&transSockAddr, 0, sizeof(transSockAddr));						//每个字节都用0填充
	transSockAddr.sin_family = PF_INET;								//使用IPv4地址
	
	transSockAddr.sin_port = htons(trans_port);							//端口号   要用到htons()函数转换
	transSockAddr.sin_addr.s_addr = inet_addr(trans_ip);			//具体的IP地址32位

	bind(servSock, (SOCKADDR*)&transSockAddr, sizeof(SOCKADDR));				//绑定套接字，
	printf("绑定套接字成功\n");
	listen(servSock, 20);
	printf("服务器已经进入监听状态\n");
	SOCKADDR clntAddr;
	int nSize = sizeof(SOCKADDR);
	appSock = accept(servSock, (SOCKADDR*)&clntAddr, &nSize);
	printf("已与app连接\n");

	//作为客户端与ros连接的socket
	rosSock = socket(PF_INET, SOCK_STREAM, 0);
	sockaddr_in rosSockAddr;								//创建sockAddr结构体
	memset(&rosSockAddr, 0, sizeof(rosSockAddr));				 //每个字节都用0填充
	rosSockAddr.sin_family = PF_INET;
	rosSockAddr.sin_port = htons(ros_port);
	rosSockAddr.sin_addr.s_addr = inet_addr(ros_ip);

	connect(rosSock, (SOCKADDR*)&rosSockAddr, sizeof(SOCKADDR));		//建立链接
	printf("已向ros发送连接请求\n");

	std::thread t1(app_to_ros);
	std::thread t2(ros_to_app);

	t1.join();
	t2.join();
	printf("threads joint\n");
	closesocket(servSock);
	closesocket(appSock);
	closesocket(rosSock);
	printf("sockets closed\n");
	return 0;
}

void app_to_ros() {
	printf("app_to_ros start\n");
	char buff[100] = { 0 };
	int len, r;
	while (1) {
		len = recv(appSock, buff, 100, 0);
		if (len == 0 || len == -1) {
			printf("app_to_ros recv returns %d\n", len);
			break;
		}
		buff[len] = '\0';
		printf("app sent %s\n", buff);
		r = send(rosSock, buff, len + 1, 0);
		if (r == 0 || r == -1) {
			printf("app_to_ros send returns %d\n", r);
			break;
		}
	}
}

void ros_to_app() {
	printf("app_to_ros start\n");
	char buff[100] = { 0 };
	int len, r;
	while (1) {
		len = recv(rosSock, buff, 100, 0);
		if (len == 0 || len == -1) {
			printf("ros_to_app recv returns %d\n", len);
			break;
		}
		buff[len] = '\0';
		printf("ros sent %s\n", buff);
		r = send(appSock, buff, len + 1, 0);
		if (r == 0 || r == -1) {
			printf("ros_to_app send returns %d\n", r);
			break;
		}
	}
}