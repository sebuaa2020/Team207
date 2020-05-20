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

#define STATE_WAIT 0
#define STATE_CONNECT 1
#define STATE_WORK 2
#define STATE_BREAK 3

#define MAX_BUFF 100

int main(int argc, char** argv)
{
    ros::init(argc, argv, "test_sender");
    ros::NodeHandle n;
    ros::Publisher test_pub = n.advertise<test_topic::test_msg>("/test_topic", 10);
    test_topic::test_msg msg;
    int send;
    int i=1;
    int state = STATE_WAIT;

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


    char buff[MAX_BUFF]={0};
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
                state = STATE_CONNECT;
            break;
            case STATE_CONNECT:
                read(cfd, buff, 10);
                printf("read: %s\n", buff);
                scanf("%s", buff);
                write(cfd, buff, strlen(buff));
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