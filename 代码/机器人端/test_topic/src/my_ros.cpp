#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <stdio.h>
#include <test_topic/MC_msg.h>
#include <test_topic/Nav_msg.h>
#include <test_topic/Grab_msg.h>
#include <test_topic/Ret_msg.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <stdlib.h>
#include <string.h>
#include "thread_buffer.cpp"
#include "cmd_handler.cpp"
#include <sys/stat.h>
#include <fcntl.h>

#define STATE_WAIT 0
#define STATE_CONNECT 1
#define STATE_WORK 2
#define STATE_BREAK 3

#define MAX_BUFF 100

static Thread_buffer tb;
static pthread_t send_thread;
static pthread_t recv_thread;
static bool isconnect = false;

void *socket_send(void *arg);
void *socket_recv(void *arg);
int rethandler(const test_topic::Ret_msg::ConstPtr& msg);

int main(int argc, char** argv)
{
    ros::init(argc, argv, "my_ros");
    ros::NodeHandle n;
    ros::Publisher mc_pub = n.advertise<test_topic::MC_msg>("/MC_msg", 10);
    ros::Publisher nav_pub = n.advertise<test_topic::Nav_msg>("/Nav_msg", 10);
    ros::Publisher grab_pub = n.advertise<test_topic::Grab_msg>("/Grab_msg", 10);
    ros::Subscriber ret_sub = n.subscribe<test_topic::Ret_msg>("/return_msg", 10, &rethandler);
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
//-----------Load login password-------------------------------

    FILE *pass;
    pass = fopen("password.txt", "r");
    if (pass == NULL) {
        printf("try to init password as admin\n");
        pass = fopen("password.txt", "w");
        if (pass == NULL) {
            printf("Failed to init password\n");
            return -1;
        }
        fprintf(pass, "admin");
        fclose(pass);
        printf("Failed to get login password\n");
        pass = fopen("password.txt", "r");
        if (pass == NULL) {
            printf("Failed to reopen password\n");
            return -1;
        }
    }
    char password[20];
    fscanf(pass, "%s", password);
    fclose(pass);

//------------Main state control-------------------------------
    char charbuff[MAX_BUFF]={0};
    Cmd cmdbuff;
    std::string strbuff,str2;
    while(n.ok())
    {   
        switch(state) {
            //-----State 1: Wating for connection--------------
            case STATE_WAIT:
                printf("waiting for connection\n");
                cfd = accept(sfd, (struct sockaddr*)(&caddr), &addrlen);
                if(cfd == -1) {
                    printf("Failed to accept\n");
                    break;
                }
                printf("accept %s\n", inet_ntoa(caddr.sin_addr));
                isconnect = true;
                pthread_create(&send_thread, NULL, &socket_send, (void *)cfd);
                pthread_create(&recv_thread, NULL, &socket_recv, (void *)cfd);
                state = STATE_CONNECT;
            break;
            //-----State 2: Login--------------------------------
            case STATE_CONNECT:
                tb.get_recv(&strbuff);
                if (isconnect == false) {
                    state = STATE_BREAK;
                    break;
                }
                str_to_cmd(strbuff, &cmdbuff);
                if (cmdbuff.type == 1) {
                    printf("pass:%s, input:%s\n",password,cmdbuff.content);
                    if (std::strcmp(password, cmdbuff.content) == 0) {
                        printf("Successful login\n");
                        tb.set_send("0 1");
                        state = STATE_WORK;
                    } else {
                        printf("Wrong Password\n");
                        tb.set_send("0 0");
                    }
                }
            break;
            //-----State 3: Resend cmd to coresponding control node---
            case STATE_WORK:
                tb.get_recv(&strbuff);
                if (isconnect == false) {
                    state = STATE_BREAK;
                    break;
                }
                str_to_cmd(strbuff, &cmdbuff);
                switch (cmdbuff.type) {
                    case 2: {
                        test_topic::MC_msg mcmsg;
                        Cmd_mc *mc;
                        mc = (Cmd_mc *)&(cmdbuff.content);
                        mcmsg.type = mc->type;
                        mc_pub.publish(mcmsg);
                    }
                    break;
                    case 3: {
                        test_topic::Nav_msg navmsg;
                        Cmd_nav *nav;
                        nav = (Cmd_nav *)&(cmdbuff.content);
                        navmsg.pos_x = nav->pos_x;
                        navmsg.pos_y = nav->pos_y;
                        navmsg.agl_z = nav->agl_z;
                        nav_pub.publish(navmsg);
                    }
                    break;
                    case 4: {
                        test_topic::Grab_msg gmsg;
                        Cmd_grab *grab;
                        grab = (Cmd_grab *)&(cmdbuff.content);
                        gmsg.fea_color = grab->fea_color;
                        gmsg.fea_size = grab->fea_size;
                        gmsg.fea_pos = grab->fea_pos;
                    }
                    break;
                    case 5:
                        state = STATE_BREAK;
                    break;
                }
            break;
            case STATE_BREAK:
                isconnect = false;
                pthread_cancel(send_thread);
                pthread_cancel(recv_thread);
                pthread_join(send_thread, NULL);
                pthread_join(recv_thread, NULL);
                tb.clear_buff();
                state = STATE_WAIT;
            break;

        }
        ros::spinOnce();
    }
    printf("node exit\n");
    close (cfd);
    close (sfd);
    return 0;
}

void *socket_send(void *arg) {
    std::cout<<"Socket send thread online"<<std::endl;
    int cfd = (long) arg;
    int r;
    char cbuff[MAX_BUFF];
    std::string sbuff;
    while(1) {
        tb.get_send(&sbuff);
        int i;
        for (i=0; i<sbuff.length(); i++) {
            cbuff[i] = sbuff[i];
        }
        cbuff[i] = '\0';
        r = send(cfd, cbuff, strlen(cbuff), 0);
        if (r == 0 || r == -1) {
            printf("socket_send return %d\n",r);
            isconnect = false;
            tb.set_recv("end");
            break;
        }
        std::cout << "socket send: " << sbuff << std::endl;
    }
}

void *socket_recv(void *arg) {
    std::cout<<"Socket recv thread online"<<std::endl;
    int cfd = (long) arg;
    int r;
    char cbuff[MAX_BUFF];
    std::string sbuff;
    while(1) {
        r = recv(cfd, cbuff, MAX_BUFF, 0);
        if (r == 0 || r == -1) {
            printf("socket_recv return %d\n",r);
            isconnect = false;
            tb.set_recv("end");
            break;
        }
        sbuff = std::string(cbuff);
        tb.set_recv(sbuff);
        std::cout << "socket recv: " << sbuff << std::endl;
    }
}

int rethandler(const test_topic::Ret_msg::ConstPtr& msg) {
    char buff[20];
    std::sprintf(buff, "0 %d %d\n", msg->publisher, msg->type);
    std::string str(buff);
    tb.set_send(str);
    return 0;
}
