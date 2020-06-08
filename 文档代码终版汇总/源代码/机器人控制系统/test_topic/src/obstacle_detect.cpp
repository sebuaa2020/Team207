
#include <ros/ros.h>
#include <std_msgs/String.h>
#include <sensor_msgs/LaserScan.h>
#include <geometry_msgs/Twist.h>
#include <test_topic/OD_msg.h>

static const float alert_dist = 0.4;
static ros::Publisher OD_pub;

static bool ob_front = false;
static bool ob_back = false;
static bool ob_left = false;
static bool ob_right = false;

void lidarCallback(const sensor_msgs::LaserScan::ConstPtr& scan)
{
    int nNum = scan->ranges.size();
    float min_front = 10, min_back = 10, min_left = 10, min_right = 10;
    int i;
    for (i=0; i<nNum/8; i++) {
        if (scan->ranges[i] < min_back) {
            min_back = scan->ranges[i];
        }
    }
    for (i=nNum/8; i<nNum*3/8; i++) {
        if (scan->ranges[i] < min_right) {
            min_right = scan->ranges[i];
        }
    }
    for (i=nNum*3/8; i<nNum*5/8; i++) {
        if (scan->ranges[i] < min_front) {
            min_front = scan->ranges[i];
        }
    }
    for (i=nNum*5/8; i<nNum*7/8; i++) {
        if (scan->ranges[i] < min_left) {
            min_left = scan->ranges[i];
        }
    }
    for (i=nNum*7/8; i<nNum; i++) {
        if (scan->ranges[i] < min_back) {
            min_back = scan->ranges[i];
        }
    }
    printf("OD: front:%f, back:%f, left:%f, right:%f\n", min_front, min_back,min_left, min_right);
    test_topic::OD_msg newmsg;
    if (min_front < alert_dist)
        newmsg.front = true;
    else 
        newmsg.front = false;
    if (min_back < alert_dist)
        newmsg.back = true;
    else 
        newmsg.back = false;
    if (min_left < alert_dist)
        newmsg.left = true;
    else 
        newmsg.left = false;
    if (min_right < alert_dist)
        newmsg.right = true;
    else 
        newmsg.right = false;
    
    if (newmsg.front != ob_front || newmsg.back != ob_back || newmsg.left != ob_left || newmsg.right != ob_right) {
        ob_front = newmsg.front;
        ob_back = newmsg.back;
        ob_left = newmsg.left;
        ob_right = newmsg.right;
        OD_pub.publish(newmsg);
    }
}

int main(int argc, char** argv)
{
    ros::init(argc,argv,"obstacle_detect");
    
    ROS_INFO("obstacle detect start!");

    ros::NodeHandle nh;
    ros::Subscriber lidar_sub = nh.subscribe("/scan", 10, &lidarCallback);
    OD_pub = nh.advertise<test_topic::OD_msg>("/OD_msg",10);

    ros::spin();
}
