#include <ros/ros.h>
#include <tf2_ros/transform_listener.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#include <geometry_msgs/PoseStamped.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

bool transformGoalPose(const geometry_msgs::PoseStamped& input_pose, geometry_msgs::PoseStamped& output_pose) {
    static tf2_ros::Buffer tf_buffer;
    static tf2_ros::TransformListener tf_listener(tf_buffer);

    try {
        geometry_msgs::TransformStamped transform_stamped;
        transform_stamped = tf_buffer.lookupTransform("map", "odom", ros::Time(0), ros::Duration(1.0));

        tf2::doTransform(input_pose, output_pose, transform_stamped);
        output_pose.header.frame_id = "map"; // Ensure it's in map frame
        return true;
    } catch (tf2::TransformException &ex) {
        ROS_WARN("TF2 Transform Error: %s", ex.what());
        return false;
    }
}

int sendGoal(double x, double y, double yaw) {
    MoveBaseClient ac("move_base", true);

    // Wait for move_base action server
    ROS_INFO("Waiting for the move_base action server...");
    ac.waitForServer();
    ROS_INFO("Connected to move_base server");

    // Define goal
    move_base_msgs::MoveBaseGoal goal;
    goal.target_pose.header.frame_id = "map"; 
    goal.target_pose.header.stamp = ros::Time::now();

    geometry_msgs::PoseStamped goal_pose;
    
    goal_pose.pose.position.x = x;
    goal_pose.pose.position.y = y;
    goal_pose.pose.orientation.z = sin(yaw / 2.0);
    goal_pose.pose.orientation.w = cos(yaw / 2.0);

    ROS_INFO("Sending goal: x=%.2f, y=%.2f, yaw=%.2f", x, y, yaw);

    // geometry_msgs::PoseStamped transformed_pose;
    // if (!transformGoalPose(goal_pose, transformed_pose)) {
    //     ROS_ERROR("Failed to transform goal pose!");
    //     return -1;
    // }

    goal.target_pose.pose = goal_pose.pose;
    ac.sendGoal(goal);
    
    ac.waitForResult();
    
    if (ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
        ROS_INFO("Goal reached!");
    else
        ROS_WARN("Failed to reach goal");
    return 0;
}

int main(int argc, char** argv) {
    ros::init(argc, argv, "auto_navigation");

    // Define multiple goals
    std::vector<std::tuple<double, double, double>> goals = {

    { 2.2, -3.60, 1.57 },
    { -0.4, -3.60, 1.57 },
    { 0.5, -2.50, 0 },
    { 5.65, -4.34, 1.57 },
    { 5.63, -2.41, 1.57 }
    
    };

    for (const auto& goal : goals) {
        sendGoal(std::get<0>(goal), std::get<1>(goal), std::get<2>(goal));
        ros::Duration(2.0).sleep();  // Pause before next goal
    }

    return 0;
}