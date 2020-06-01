#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <stdio.h>
#include <test_topic/test_msg.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <stdlib.h>
#include <string.h>
#include "thread_buffer.cpp"

#define STATE_WAIT 0
#define STATE_CONNECT 1
#define STATE_WORK 2
#define STATE_BREAK 3

#define MAX_BUFF 100

static Thread_buffer tb;
static pthread_t send_thread;
static pthread_t recv_thread;

void *socket_send(void *arg);
void *socket_recv(void *arg);

int main(int argc, char** argv)
{
    ros::init(argc, argv, "test_sender");
    ros::NodeHandle n;
    ros::Publisher test_pub = n.advertise<test_topic::test_msg>("/test_topic", 10);
    test_topic::test_msg msg;
    int send;
    int i=1;
    int state = STATE_WAIT;

//--------init Socket---------------------------
    int sfd=0, cfd=0, ret=0, count=0;
    socklen_t addrlen;
    struct sockaddr_in saddr, caddr;
    memset(&saddr, 0, sizeof(struct sockaddr_in));
    memset(&caddr, 0, sizeof(struct sockaddr_in));
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sfd == -1) {
        printf("Failed to start socket\n");
        return -1;
    }
    int opt = 1;
    setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof( opt ));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(28888);
    saddr.sin_addr.s_addr = INADDR_ANY;
    bzero(saddr.sin_zero, 8);
    ret = bind(sfd, (struct sockaddr*)(&saddr), sizeof(struct sockaddr));
    if(ret == -1) {
        printf("Failed to bind socket\n");
        return -1;
    }
    ret = listen(sfd, 10);
    if(ret == -1) {
        printf("Failed to listen port\n");
        return -1;
    }
    addrlen = sizeof(struct sockaddr);

//------------Main state control-------------------------------
    char buff[MAX_BUFF]={0};
    std::string str1,str2;
    while(n.ok())
    {   
        switch(state) {
            case STATE_WAIT:
                cfd = accept(sfd, (struct sockaddr*)(&caddr), &addrlen);
                if(cfd == -1) {
                    printf("Failed to accept\n");
                    break;
                }
                printf("accept %s\n", inet_ntoa(caddr.sin_addr));
                pthread_create(&send_thread, NULL, &socket_send, (void *)cfd);
                pthread_create(&recv_thread, NULL, &socket_recv, (void *)cfd);
                state = STATE_CONNECT;
            break;
            case STATE_CONNECT:
                tb.get_recv(&str1);
                str2 = "ROS get: " +str1;
                std::cout<<str2<<std::endl;
                tb.set_send(str2);
            break;
            case STATE_WORK:
                scanf("%d", &send);
                printf("Publisher send %d\n", send);
                msg.data = send;
                test_pub.publish(msg);
            break;
            case STATE_BREAK:
            
            break;

        }
        
    }
    return 0;
}

void *socket_send(void *arg) {
    std::cout<<"Socket send thread online"<<std::endl;
    int cfd = (long) arg;
    char cbuff[MAX_BUFF];
    std::string sbuff;
    while(1) {
        tb.get_send(&sbuff);
        int i;
        for (i=0; i<sbuff.length(); i++) {
            cbuff[i] = sbuff[i];
        }
        cbuff[i] = '\0';
        write(cfd, cbuff, strlen(cbuff));
        std::cout << "socket send: " << sbuff << std::endl;
    }
}

void *socket_recv(void *arg) {
    std::cout<<"Socket recv thread online"<<std::endl;
    int cfd = (long) arg;
    char cbuff[MAX_BUFF];
    std::string sbuff;
    while(1) {
        read(cfd, cbuff, 10);
        sbuff = std::string(cbuff);
        tb.set_recv(sbuff);
        std::cout << "socket recv: " << sbuff << std::endl;
    }
}