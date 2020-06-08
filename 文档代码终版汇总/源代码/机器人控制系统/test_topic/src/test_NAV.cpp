#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <stdio.h>
#include <test_topic/Nav_msg.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv)
{
    ros::init(argc, argv, "test_NAV");
    ros::NodeHandle n;
    ros::Publisher test_pub = n.advertise<test_topic::Nav_msg>("/Nav_msg", 10);
    test_topic::Nav_msg msg;
    float x,y,z;

    while(n.ok())
    {   
        scanf("%f %f %f", &x, &y, &z);
        printf("command send %f %f %f\n", x,y,z);
        msg.pos_x = x;
        msg.pos_y = y;
        msg.agl_z = z;
        test_pub.publish(msg);
    }
    return 0;
}