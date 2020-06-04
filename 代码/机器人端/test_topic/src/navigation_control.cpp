/*********************************************************************
* Software License Agreement (BSD License)
* 
*  Copyright (c) 2017-2020, Waterplus http://www.6-robot.com
*  All rights reserved.
* 
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
* 
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of the WaterPlus nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
* 
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  FOOTPRINTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/
/*!******************************************************************
 @author     ZhangWanjie
 ********************************************************************/

#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib_msgs/GoalID.h>
#include <actionlib/client/simple_action_client.h>
#include <tf/tf.h>
#include <tf/transform_listener.h>
#include <visualization_msgs/Marker.h>
#include <test_topic/Nav_msg.h>
#include <test_topic/Ret_msg.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

static std::vector <move_base_msgs::MoveBaseGoal> arWayPoint;
static ros::Publisher marker_pub;
static ros::Publisher ret_pub;
static ros::Publisher cancel_pub;
static visualization_msgs::Marker marker_waypoints;
static visualization_msgs::Marker text_marker;
static pthread_t nav_thread;
static pthread_mutex_t wp_lock;
static sem_t wp_sem;
static int nWPIndex =0;

void add_WayPoints(float pos_x, float pos_y, float agl_z);
void Init_WayPoints();
void Init_Marker();
void DrawTextMarker(std::string inText, int inID, float inScale, float inX, float inY, float inZ, float inR, float inG, float inB);
void PublishWaypointsMarker();
void cmdHandler(const test_topic::Nav_msg::ConstPtr& cmd);
void *nav_control(void *arg);


int main(int argc, char** argv)
{
    ros::init(argc, argv, "navigation_control");

    ros::NodeHandle nh;
    marker_pub = nh.advertise<visualization_msgs::Marker>("waypoints_marker", 100);
    ret_pub = nh.advertise<test_topic::Ret_msg>("/return_msg", 10);
    cancel_pub = nh.advertise<actionlib_msgs::GoalID>("move_base/cancel",1);
    ros::Subscriber cmd_sub = nh.subscribe<test_topic::Nav_msg>("/Nav_msg",10,&cmdHandler);
    
    sem_init(&wp_sem, 0, 0);
    //Init_WayPoints();
    Init_Marker();
    PublishWaypointsMarker();
    //tell the action client that we want to spin a thread by default
    
    pthread_create(&nav_thread, NULL, &nav_control, NULL);
    ros::spin();
    /*
    while (ros::ok()) {
        Init_Marker();
        PublishWaypointsMarker();
        ros::spinOnce();
    }*/
    pthread_cancel(nav_thread);
    pthread_join(nav_thread, NULL);
    return 0;
}

void add_WayPoints(float pos_x, float pos_y, float agl_z)
{
    move_base_msgs::MoveBaseGoal newWayPoint;
    tf::Quaternion q;

    newWayPoint.target_pose.header.frame_id = "map";
    newWayPoint.target_pose.pose.position.x = pos_x;
    newWayPoint.target_pose.pose.position.y = pos_y;
    q.setRPY( 0, 0, agl_z );
    newWayPoint.target_pose.pose.orientation.x = q.x();
    newWayPoint.target_pose.pose.orientation.y = q.y();
    newWayPoint.target_pose.pose.orientation.z = q.z();
    newWayPoint.target_pose.pose.orientation.w = q.w();
    ROS_INFO("Hi, there");
    arWayPoint.push_back(newWayPoint);
    ROS_INFO("What's up");
    sem_post(&wp_sem);
}

void Init_Marker()
{
    marker_waypoints.header.frame_id = "map";
    marker_waypoints.ns = "marker_waypoints";
    marker_waypoints.action = visualization_msgs::Marker::ADD;
    marker_waypoints.id = 1;
    marker_waypoints.type = visualization_msgs::Marker::CUBE_LIST;
    marker_waypoints.scale.x = 0.2;
    marker_waypoints.scale.y = 0.2;
    marker_waypoints.scale.z = 0.3;
    marker_waypoints.color.r = 0;
    marker_waypoints.color.g = 0.5;
    marker_waypoints.color.b = 1.0;
    marker_waypoints.color.a = 1.0;

    geometry_msgs::Point point;
    point.z = 0.15;
    marker_waypoints.points.clear();
    int nNumWP = arWayPoint.size();
    for(int i=0; i<nNumWP ; i++ )
    {
        point.x = arWayPoint[i].target_pose.pose.position.x;
        point.y = arWayPoint[i].target_pose.pose.position.y;
        marker_waypoints.points.push_back(point);
    }
}

void DrawTextMarker(std::string inText, int inID, float inScale, float inX, float inY, float inZ, float inR, float inG, float inB)
{
    text_marker.header.frame_id = "map";
    text_marker.ns = "line_obj";
    text_marker.action = visualization_msgs::Marker::ADD;
    text_marker.id = inID;
    text_marker.type = visualization_msgs::Marker::TEXT_VIEW_FACING;
    text_marker.scale.z = inScale;
    text_marker.color.r = inR;
    text_marker.color.g = inG;
    text_marker.color.b = inB;
    text_marker.color.a = 1.0;

    text_marker.pose.position.x = inX;
    text_marker.pose.position.y = inY;
    text_marker.pose.position.z = inZ;
    
    text_marker.pose.orientation=tf::createQuaternionMsgFromYaw(1.0);

    text_marker.text = inText;

    marker_pub.publish(text_marker);
}

void PublishWaypointsMarker()
{
    int nNumWP = arWayPoint.size();
    for(int i=0; i<nNumWP ; i++ )
    {
        float wp_x = arWayPoint[i].target_pose.pose.position.x;
        float wp_y = arWayPoint[i].target_pose.pose.position.y;

        std::ostringstream stringStream;
        stringStream << "wp_" << i+nWPIndex;
        std::string face_id = stringStream.str();
        DrawTextMarker(face_id,i,0.2,wp_x,wp_y,marker_waypoints.scale.z+0.2,0,0.5,1.0);
    }
    marker_pub.publish(marker_waypoints);
    ros::spinOnce();
}



void cmdHandler(const test_topic::Nav_msg::ConstPtr& cmd) {
    printf("NAV control recv cmd: %f, %f, %f\n", cmd->pos_x, cmd->pos_y, cmd->agl_z);
    if (cmd->agl_z == -1) {
        pthread_mutex_lock(&wp_lock);
        actionlib_msgs::GoalID cancel;
        cancel_pub.publish(cancel);
        arWayPoint.clear();
        sem_init(&wp_sem, 0, 0);
        Init_Marker();
        PublishWaypointsMarker();
        pthread_mutex_unlock(&wp_lock);
    } else {
        pthread_mutex_lock(&wp_lock);
        add_WayPoints(cmd->pos_x, cmd->pos_y, cmd->agl_z);
        Init_Marker();
            PublishWaypointsMarker();
        pthread_mutex_unlock(&wp_lock);
    }
}

void *nav_control(void *arg) {
    
    MoveBaseClient ac("move_base", true);

    //wait for the action server to come up
    while(!ac.waitForServer(ros::Duration(5.0)))
    {
        if(!ros::ok())
            break;
        ROS_INFO("Waiting for the move_base action server to come up");
    }
    while(1)
    {   
        std::cout << "1size of arWayPoint" << arWayPoint.size() <<std::endl;
        sem_wait(&wp_sem);
        
        pthread_mutex_lock(&wp_lock);
        std::cout << "2size of arWayPoint" << arWayPoint.size() <<std::endl;
        ROS_INFO("Go to the WayPoint[%d]",nWPIndex);
        Init_Marker();
        PublishWaypointsMarker();
        ac.sendGoal(arWayPoint[0]);
        pthread_mutex_unlock(&wp_lock);

        ac.waitForResult();

        if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
        {   
            pthread_mutex_lock(&wp_lock);
            ROS_INFO("Arrived at WayPoint[%d] !",nWPIndex);
            nWPIndex ++;
            arWayPoint.erase(arWayPoint.begin());
            Init_Marker();
            PublishWaypointsMarker();
            pthread_mutex_unlock(&wp_lock);
        }
        else
            ROS_INFO("Failed to get to WayPoint[%d] ...",nWPIndex );
        
    }
}