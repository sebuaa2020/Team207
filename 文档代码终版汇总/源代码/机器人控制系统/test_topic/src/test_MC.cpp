#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <stdio.h>
#include <test_topic/MC_msg.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv)
{
    ros::init(argc, argv, "test_MC");
    ros::NodeHandle n;
    ros::Publisher test_pub = n.advertise<test_topic::MC_msg>("/MC_msg", 10);
    test_topic::MC_msg msg;
    int type;

    while(n.ok())
    {   
        scanf("%d", &type);
        printf("command send %d\n", type);
        msg.type = type;
        test_pub.publish(msg);
    }
    return 0;
}