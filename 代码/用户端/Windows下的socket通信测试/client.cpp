//#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>						//����WinSock2.hͷ�ļ�
#pragma comment(lib, "ws2_32.lib")			//���� ws2_32.dll 
int main() {
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);		//��ʼ���汾
	SOCKET sock = socket(PF_INET, SOCK_STREAM, 0);		//�����û���socket

	//���������������
	sockaddr_in sockAddr;								//����sockAddr�ṹ��
	memset(&sockAddr, 0, sizeof(sockAddr));				 //ÿ���ֽڶ���0���
	sockAddr.sin_family = PF_INET;
	sockAddr.sin_port = htons(28888);
	sockAddr.sin_addr.s_addr = inet_addr("192.168.178.128");

	connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));		//��������
	printf("�ͻ��˷�����������\n");


	//���շ��������ص�����
	char szBuffer[MAXBYTE] = { 0 };
	scanf("%s", szBuffer);
	send(sock, szBuffer, strlen(szBuffer), 0);
	recv(sock, szBuffer, MAXBYTE, NULL);
	printf("���շ��������ص���Ϣ����\n");

	//������յ�������
	printf("������: %s\n", szBuffer);

	//�ر��׽��֡���ֹʹ�� DLL
	closesocket(sock);
	WSACleanup();
	system("PAUSE");
	return 0;
}