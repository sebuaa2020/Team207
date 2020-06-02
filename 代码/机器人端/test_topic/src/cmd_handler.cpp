#include <c++/5/iostream>
#include <c++/5/sstream>
#include <stdlib.h>
#include <string.h>

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
    std::stringstream ss;
    ss << str;
    int type;
    char c;
    ss >> type >> c;
    buff->type = type;
    switch (type) {
        case 1://-----Login cmd-----
            ss >> buff->content;
        break;
        case 2://-----Manual control cmd-----
            Cmd_mc *mc;
            mc = (Cmd_mc *)&(buff->content);
            ss >> mc->type;
        break;
        case 3://-----Navigation cmd-----
            Cmd_nav *nav;
            nav = (Cmd_nav *)&(buff->content);
            ss >> nav->pos_x >> c >> nav->pos_y >> c >> nav->agl_z;
        break;
        case 4://-----Grab cmd-----
            Cmd_grab *grab;
            grab = (Cmd_grab *)&(buff->content);
            ss >> grab->fea_color >> c >> grab->fea_size >> c >> grab->fea_pos;
        break;
        case 5://-----End service cmd-----
            //do nothing
        break;
    }
}