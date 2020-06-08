#include <c++/5/iostream>
#include <c++/5/sstream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct cmd {
    long type;
    char content[16];
} Cmd;

typedef struct cmd_login {
    char password[16];
} Cmd_login;

typedef struct cmd_mc {
    long type;
    char zero[12];
} Cmd_mc;

typedef struct cmd_nav {
    float pos_x;
    float pos_y;
    float agl_z;
    char zero[4];
} Cmd_nav;

typedef struct cmd_grab {
    long fea_color;
    long fea_size;
    long fea_pos;
    char zero[4];
} Cmd_grab;

void str_to_cmd(std::string str, Cmd *buff) {
    memset(buff, 0, sizeof(Cmd));
    std::cout << "cmd handler: string:"<<str<<",";
    std::stringstream ss;
    ss << str;
    long type;
    char c;
    ss >> type ;
    buff->type = type;
    printf("type:%ld, ",type);
    switch (type) {
        case 1://-----Login cmd-----
            ss >> buff->content;
            printf("pass:%s\n", buff->content);
        break;
        case 2://-----Manual control cmd-----
            Cmd_mc *mc;
            mc = (Cmd_mc *)&(buff->content);
            ss >> mc->type;
            printf("type2:%ld\n", mc->type);
        break;
        case 3://-----Navigation cmd-----
            Cmd_nav *nav;
            nav = (Cmd_nav *)&(buff->content);
            ss >> nav->pos_x >> nav->pos_y  >> nav->agl_z;
            printf("args:%f, %f, %f\n", nav->pos_x, nav->pos_y, nav->agl_z);
        break;
        case 4://-----Grab cmd-----
            Cmd_grab *grab;
            grab = (Cmd_grab *)&(buff->content);
            ss >> grab->fea_color  >> grab->fea_size >> grab->fea_pos;
        break;
        case 5://-----End service cmd-----
            //do nothing
        break;
    }
}