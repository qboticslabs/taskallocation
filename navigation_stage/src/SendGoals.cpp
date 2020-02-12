/*
 * SendGoals.cpp
 *
 *  Created on: Dec 7, 2013
 *      Author: roiyeho
 */

#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <tf/transform_datatypes.h>
#include "navi_msgs/GoalsList.h"
#include "navi_msgs/Problem.h"

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <thread>
#include <string>
using namespace std;

int nVehicles;      
int nTasks;       
int nDim; 
int rDim; 

class SendGoals
{
public:
void probCallback(const navi_msgs::Problem::ConstPtr& msg)
    {
        nVehicles = msg->nRobots;
        nTasks= msg->nTasks;
        nDim = msg->nTotalmodels;
        rDim = msg->nModels;
    }
    
void robotsCallback(const navi_msgs::GoalsList::ConstPtr& msg, int a)
    {
        typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;
        std::string name1 = "robot_", name2 = "/move_base";
        std::string robotName= name1+std::to_string(a)+name2;
        std::cout <<robotName <<std::endl;
        MoveBaseClient ac(robotName, true);

        // Wait 60 seconds for the action server to become available
        ROS_INFO("Waiting for the move_base action server");
        ac.waitForServer(ros::Duration(60));
        ROS_INFO("Connected to move base server");
    
        move_base_msgs::MoveBaseGoal goal;
        int numTasks = msg->list[a].tasks;
        
        for (int i=0; i<numTasks; i++)
            {   goal.target_pose.header.frame_id = "map";
                goal.target_pose.header.stamp = ros::Time::now();	
                goal.target_pose.pose.position.x = msg->list[a].pList.poses[i].position.x;
                goal.target_pose.pose.position.y = msg->list[a].pList.poses[i].position.y;

                double radians = msg->list[a].pList.poses[i].position.z * (M_PI/180);
                tf::Quaternion quaternion;
                quaternion = tf::createQuaternionFromYaw(radians);
                geometry_msgs::Quaternion qMsg;
                tf::quaternionTFToMsg(quaternion, qMsg);
                goal.target_pose.pose.orientation = qMsg;

                ROS_INFO("Sending robot_%d to: x = %f, y = %f, theta = %f", a,msg->list[a].pList.poses[i].position.x,msg->list[a].pList.poses[i].position.y,msg->list[a].pList.poses[i].position.z );    
                ac.sendGoal(goal);
                ac.waitForResult();
    
                if (ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
                    {ROS_INFO("The robot_%d REACHED the %d goal!",a,i+1);
                        if (i == (numTasks-1))
                        {   ROS_INFO("FINISHED TASKS FOR ROBOT %d",a);
                            //std::terminate();
                            
                        }
                    }    
                else
                    ROS_INFO("The base failed for some reason");
            } 
        }
};

int main(int argc, char** argv) 
{
    ros::init(argc, argv, "SendGoalsNode");
    ros::NodeHandle nh_;
    SendGoals sg;
    ros::Subscriber probSub = nh_.subscribe<navi_msgs::Problem>("/problem", 100, &SendGoals::probCallback, &sg);
  
    cout << nVehicles << endl;
    ros::AsyncSpinner spinner(6);  // Use multiple threads
    ROS_INFO_STREAM("Main loop in thread:" << boost::this_thread::get_id());
    spinner.start();
    cout << nVehicles << endl;

        std::vector<ros::Subscriber> subGoal;
        for(int i = 0; i < 6; i++) {
            ros::Subscriber sub = nh_.subscribe<navi_msgs::GoalsList>( "/tasks", 1, boost::bind(&SendGoals::robotsCallback, &sg,_1, i) );
            subGoal.push_back(sub);
        }
   
    ros::waitForShutdown();
    return 0;
}
    
    /*//poses for third robot
    double xx2 [] = {30,38,40,47,55};
    double yy2 [] = {28,24,31,37,48};
    double tt2 [] = {0,0,0,0,0};
    std::vector <double> x2;
    std::vector <double> y2;
    std::vector <double> theta2;
    for (int i = 0; i<5; i++)
    {x2.push_back(xx2[i]);
    y2.push_back(yy2[i]);
    theta2.push_back(tt2[i]);}
     
    //poses for fourth robot
    double xx3 [] = {26,30,32};
    double yy3 [] = {39,50,40};
    double tt3 [] ={0,0,0};
    std::vector <double> x3;
    std::vector <double> y3;
    std::vector <double> theta3;
    for (int i = 0; i<3; i++)
    {x3.push_back(xx3[i]);
    y3.push_back(yy3[i]);
    theta3.push_back(tt3[i]);}
     
    //poses for fifth robot
    double xx4 [] = {38,45};
    double yy4 [] = {18,18};
    double tt4 [] = {0,0};
    std::vector <double> x4;
    std::vector <double> y4;
    std::vector <double> theta4;
    for (int i = 0; i<2; i++)
    {x4.push_back(xx4[i]);
    y4.push_back(yy4[i]);
    theta4.push_back(tt4[i]);}
     
    //poses for sixth robot
    double xx5 [] = {5,7,10};
    double yy5 [] = {24,5,10};
    double tt5 [] = {0,0,0};
    std::vector <double> x5;
    std::vector <double> y5;
    std::vector <double> theta5;
    for (int i = 0; i<3; i++)
    {x5.push_back(xx5[i]);
    y5.push_back(yy5[i]);
    theta5.push_back(tt5[i]);}
     
    //poses for seventh robot
    double xx6 [] = {23,29,24,22};
    double yy6 [] = {22,12,2,12};
    double tt6 [] = {0,0,0,0};
    std::vector <double> x6;
    std::vector <double> y6;
    std::vector <double> theta6;
    for (int i = 0; i<4; i++)
    {x6.push_back(xx6[i]);
     y6.push_back(yy6[i]);
     theta6.push_back(tt6[i]);}
    
    // create the action client	
    MoveBaseClient ac0("robot_0/move_base", true);
    MoveBaseClient ac1("robot_1/move_base", true);
    MoveBaseClient ac2("robot_2/move_base", true);
    MoveBaseClient ac3("robot_3/move_base", true);
    MoveBaseClient ac4("robot_4/move_base", true);
    MoveBaseClient ac5("robot_5/move_base", true);
    MoveBaseClient ac6("robot_6/move_base", true);

    // Wait 60 seconds for the action server to become available
    ROS_INFO("Waiting for the move_base action server");
    ac0.waitForServer(ros::Duration(60));
    ac1.waitForServer(ros::Duration(60));
    ac2.waitForServer(ros::Duration(60));
    ac3.waitForServer(ros::Duration(60));
    ac4.waitForServer(ros::Duration(60));
    ac5.waitForServer(ros::Duration(60));
    ac6.waitForServer(ros::Duration(60));

    ROS_INFO("Connected to move base server");

    // Send a goal to move_base
    move_base_msgs::MoveBaseGoal goal0;
    move_base_msgs::MoveBaseGoal goal1;
    move_base_msgs::MoveBaseGoal goal2;
    move_base_msgs::MoveBaseGoal goal3;
    move_base_msgs::MoveBaseGoal goal4;
    move_base_msgs::MoveBaseGoal goal5;
    move_base_msgs::MoveBaseGoal goal6;

    for (int i = 0; i<5; i++)
    {
        goal0.target_pose.header.frame_id = "map";
        goal0.target_pose.header.stamp = ros::Time::now();	
        goal0.target_pose.pose.position.x = x0[i];
        goal0.target_pose.pose.position.y = y0[i];
    
        goal1.target_pose.header.frame_id = "map";
        goal1.target_pose.header.stamp = ros::Time::now();	
        goal1.target_pose.pose.position.x = x1[i];
        goal1.target_pose.pose.position.y = y1[i];

        goal2.target_pose.header.frame_id = "map";
        goal2.target_pose.header.stamp = ros::Time::now();	
        goal2.target_pose.pose.position.x = x2[i];
        goal2.target_pose.pose.position.y = y2[i];
        
        goal3.target_pose.header.frame_id = "map";
        goal3.target_pose.header.stamp = ros::Time::now();	
        goal3.target_pose.pose.position.x = x3[i];
        goal3.target_pose.pose.position.y = y3[i];
        
        goal4.target_pose.header.frame_id = "map";
        goal4.target_pose.header.stamp = ros::Time::now();	
        goal4.target_pose.pose.position.x = x4[i];
        goal4.target_pose.pose.position.y = y4[i];
        
        goal5.target_pose.header.frame_id = "map";
        goal5.target_pose.header.stamp = ros::Time::now();	
        goal5.target_pose.pose.position.x = x5[i];
        goal5.target_pose.pose.position.y = y5[i];
        
        goal6.target_pose.header.frame_id = "map";
        goal6.target_pose.header.stamp = ros::Time::now();	
        goal6.target_pose.pose.position.x = x6[i];
        goal6.target_pose.pose.position.y = y6[i];
        
        // Convert the Euler angle to quaternion
        double radians0 = theta0[i] * (M_PI/180);
        tf::Quaternion quaternion0;
        quaternion0 = tf::createQuaternionFromYaw(radians0);
        geometry_msgs::Quaternion qMsg0;
        tf::quaternionTFToMsg(quaternion0, qMsg0);
        goal0.target_pose.pose.orientation = qMsg0;
    
        double radians1 = theta1[i] * (M_PI/180);
        tf::Quaternion quaternion1;
        quaternion1 = tf::createQuaternionFromYaw(radians1);
        geometry_msgs::Quaternion qMsg1;
        tf::quaternionTFToMsg(quaternion1, qMsg1);
        goal1.target_pose.pose.orientation = qMsg1;
        
        double radians2 = theta2[i] * (M_PI/180);
        tf::Quaternion quaternion2;
        quaternion2 = tf::createQuaternionFromYaw(radians2);
        geometry_msgs::Quaternion qMsg2;
        tf::quaternionTFToMsg(quaternion2, qMsg2);
        goal2.target_pose.pose.orientation = qMsg2;
        
        double radians3 = theta3[i] * (M_PI/180);
        tf::Quaternion quaternion3;
        quaternion3 = tf::createQuaternionFromYaw(radians3);
        geometry_msgs::Quaternion qMsg3;
        tf::quaternionTFToMsg(quaternion3, qMsg3);
        goal3.target_pose.pose.orientation = qMsg3;
        
        double radians4 = theta4[i] * (M_PI/180);
        tf::Quaternion quaternion4;
        quaternion4 = tf::createQuaternionFromYaw(radians4);
        geometry_msgs::Quaternion qMsg4;
        tf::quaternionTFToMsg(quaternion4, qMsg4);
        goal4.target_pose.pose.orientation = qMsg4;
        
        double radians5 = theta5[i] * (M_PI/180);
        tf::Quaternion quaternion5;
        quaternion5 = tf::createQuaternionFromYaw(radians5);
        geometry_msgs::Quaternion qMsg5;
        tf::quaternionTFToMsg(quaternion5, qMsg5);
        goal5.target_pose.pose.orientation = qMsg5;
        
        double radians6 = theta6[i] * (M_PI/180);
        tf::Quaternion quaternion6;
        quaternion6 = tf::createQuaternionFromYaw(radians6);
        geometry_msgs::Quaternion qMsg6;
        tf::quaternionTFToMsg(quaternion6, qMsg6);
        goal6.target_pose.pose.orientation = qMsg6;

        // Send the goal command
        ROS_INFO("Sending robot_0 to: x = %f, y = %f, theta = %f", x0[i], y0[i], theta0[i]);
        ac0.sendGoal(goal0);
    
        ROS_INFO("Sending robot_1 to: x = %f, y = %f, theta = %f", x1[i], y1[i], theta1[i]);
        ac1.sendGoal(goal1);
        
        ROS_INFO("Sending robot_2 to: x = %f, y = %f, theta = %f", x2[i], y2[i], theta2[i]);
        ac2.sendGoal(goal2);
        
        ROS_INFO("Sending robot_3 to: x = %f, y = %f, theta = %f", x3[i], y3[i], theta3[i]);
        ac3.sendGoal(goal3);
        
        ROS_INFO("Sending robot_4 to: x = %f, y = %f, theta = %f", x4[i], y4[i], theta4[i]);
        ac4.sendGoal(goal4);
        
        ROS_INFO("Sending robot_5 to: x = %f, y = %f, theta = %f", x5[i], y5[i], theta5[i]);
        ac5.sendGoal(goal5);
        
        ROS_INFO("Sending robot_6 to: x = %f, y = %f, theta = %f", x6[i], y6[i], theta6[i]);
        ac6.sendGoal(goal6);

        // Wait for the action to return
        ac0.waitForResult();
        if (ac0.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
            ROS_INFO("Robot reached the goal!");
        else
            ROS_INFO("The base failed for some reason");
        
        ac1.waitForResult();
        if (ac1.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
            ROS_INFO("Robot reached the goal!");
        else
            ROS_INFO("The base failed for some reason");
    
        ac2.waitForResult();
        if (ac2.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
            ROS_INFO("Robot reached the goal!");
        else
            ROS_INFO("The base failed for some reason");
        
        ac3.waitForResult();
        if (ac3.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
            ROS_INFO("Robot reached the goal!");
        else
            ROS_INFO("The base failed for some reason");
              
        ac4.waitForResult();
        if (ac4.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
            ROS_INFO("Robot reached the goal!");
        else
            ROS_INFO("The base failed for some reason");
              
        ac5.waitForResult();
        if (ac5.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
            ROS_INFO("Robot reached the goal!");
        else
            ROS_INFO("The base failed for some reason");
        
        ac6.waitForResult();
        if (ac6.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
            ROS_INFO("Robot reached the goal!");
        else
            ROS_INFO("The base failed for some reason");
}*/  
   

































