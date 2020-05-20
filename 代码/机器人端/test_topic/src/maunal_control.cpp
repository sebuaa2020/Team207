#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <stdio.h>
#include <test_topic/MC_msg.h>
#include <test_topic/OD_msg.h>

static const float linear_vel = 0.1;
static const float angular_vel = 0.1;
static geometry_msgs::Twist base_cmd;
static ros::Publisher cmd_vel_pub;
static bool ob_front = 0;
static bool ob_back = 0;
static bool ob_left = 0;
static bool ob_right = 0;
static int cur_ins = 0;

void cmdPublish() {
    base_cmd.linear.x = 0;
    base_cmd.linear.y = 0;
    base_cmd.angular.z = 0;
    switch (cur_ins) {
        case 0:
            
        break;
        case 1:
            if (!ob_front) {
                base_cmd.linear.x = linear_vel;
            }
        break;
        case 2:
            if (!ob_back) {
                base_cmd.linear.x = -linear_vel;
            }
        break;
        case 3:
            if (!ob_left) {
                base_cmd.linear.y = linear_vel;
            }
        break;
        case 4:
            if (!ob_right) {
                base_cmd.linear.y = -linear_vel;
            }
        break;
        case 5:
            base_cmd.angular.z = angular_vel;
        break;
        case 6:
            base_cmd.angular.z = -angular_vel;
        break;
    }
    cmd_vel_pub.publish(base_cmd);
}

int insHandler(const test_topic::MC_msg::ConstPtr& ins) {
    printf("MCC: get cmd\n");
    cur_ins = ins->type;
    cmdPublish();
}

int obsHandler(const test_topic::OD_msg::ConstPtr& msg) {
    printf("MCC: get OD msg\n");
    ob_front = msg->front;
    ob_back = msg->back;
    ob_left = msg->left;
    ob_right = msg->right;
    cmdPublish();
}


int main(int argc, char** argv)
{
    ros::init(argc, argv, "test_receiver");
    ros::NodeHandle n;
    cmd_vel_pub = n.advertise<geometry_msgs::Twist>("/cmd_vel", 10);
    ros::Subscriber ins_sub = n.subscribe<test_topic::MC_msg>("/MC_msg", 10, &insHandler);
    ros::Subscriber OD_sub = n.subscribe<test_topic::OD_msg>("/OD_msg", 10, &obsHandler);
    while(ros::ok())
    {
        ros::spinOnce();
    }
    return 0;
}