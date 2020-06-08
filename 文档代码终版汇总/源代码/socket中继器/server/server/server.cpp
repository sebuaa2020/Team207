//#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <thread>
#include <winsock2.h>							//����socket��ͷ�ļ�
#pragma comment (lib, "ws2_32.lib")				//���� ws2_32.dll

SOCKET servSock;
SOCKET appSock;
SOCKET rosSock;

void app_to_ros();
void ros_to_app();


int main() {
	WSADATA wsaData;							//����һ���ṹ�����
	WSAStartup(MAKEWORD(2, 2), &wsaData);		//��ʼ��WSAStartup()����,,,(�淶�İ汾�ţ�ָ��WSADATA�ṹ���ָ��),,,MSKEWORD(2,2)���汾��2�����汾��2
	FILE *socket_config;
	socket_config = fopen("socket_config.txt", "r");
	int ros_port, trans_port;
	char ros_ip[16], trans_ip[16], c;
	fscanf(socket_config, "ros_ip=%s\n", ros_ip);
	fscanf(socket_config, "ros_port=%d\n", &ros_port);
	fscanf(socket_config, "trans_ip=%s\n", trans_ip);
	fscanf(socket_config, "trans_port=%d\n", &trans_port);
	fclose(socket_config);
	
	//��Ϊ�������app���ӵ�socket
	servSock = socket(PF_INET, SOCK_STREAM, 0);		//������1��IP��ַ����PF_INET6ΪIPv6����2�����ݴ��䷽ʽ SOCK_STREAM �� SOCK_DGRAM ��3������Э�� IPPROTO_TCP �� IPPTOTO_UDP�����д0ϵͳ���Զ����㴦ʹ������Э��
	sockaddr_in transSockAddr;										//����sockaddr_in�ṹ�����
	memset(&transSockAddr, 0, sizeof(transSockAddr));						//ÿ���ֽڶ���0���
	transSockAddr.sin_family = PF_INET;								//ʹ��IPv4��ַ
	
	transSockAddr.sin_port = htons(trans_port);							//�˿ں�   Ҫ�õ�htons()����ת��
	transSockAddr.sin_addr.s_addr = inet_addr(trans_ip);			//�����IP��ַ32λ

	bind(servSock, (SOCKADDR*)&transSockAddr, sizeof(SOCKADDR));				//���׽��֣�
	printf("���׽��ֳɹ�\n");
	listen(servSock, 20);
	printf("�������Ѿ��������״̬\n");
	SOCKADDR clntAddr;
	int nSize = sizeof(SOCKADDR);
	appSock = accept(servSock, (SOCKADDR*)&clntAddr, &nSize);
	printf("����app����\n");

	//��Ϊ�ͻ�����ros���ӵ�socket
	rosSock = socket(PF_INET, SOCK_STREAM, 0);
	sockaddr_in rosSockAddr;								//����sockAddr�ṹ��
	memset(&rosSockAddr, 0, sizeof(rosSockAddr));				 //ÿ���ֽڶ���0���
	rosSockAddr.sin_family = PF_INET;
	rosSockAddr.sin_port = htons(ros_port);
	rosSockAddr.sin_addr.s_addr = inet_addr(ros_ip);

	connect(rosSock, (SOCKADDR*)&rosSockAddr, sizeof(SOCKADDR));		//��������
	printf("����ros������������\n");

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