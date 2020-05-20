#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <stdio.h>
#include <test_topic/test_msg.h>

int receive_int(const test_topic::test_msg::ConstPtr& rec) {
    printf("Subscriber recieve %d\n", rec->data);
}


int main(int argc, char** argv)
{
    ros::init(argc, argv, "test_receiver");
    ros::NodeHandle n;
    ros::Subscriber test_sub = n.subscribe<test_topic::test_msg>("/test_topic", 10, &receive_int);
    while(ros::ok())
    {
        ros::spinOnce();
    }
    return 0;
}