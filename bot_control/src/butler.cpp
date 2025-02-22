#include <ros/ros.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <bot_control/goToGoal.h>
#include <queue>  // Missing queue include

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

struct GoalPoint {
    double x, y, yaw;
};

MoveBaseClient* ac;
std::queue<GoalPoint> goal_queue;  // Queue to store goals

std::map<std::string, GoalPoint> locations = {
    {"kitchen", {5.63, -2.41, 1.57}},
    {"A", {2.2, -3.60, 1.57}},
    {"B", {-0.4, -3.60, 1.57}},
    {"C", {0.5, -2.50, 0.0}},
    {"home", {5.65, -4.34, 1.57}}
};

// Function to send the next goal
void sendNextGoal() {
    if (goal_queue.empty()) return;

    GoalPoint next_goal = goal_queue.front();
    goal_queue.pop();

    move_base_msgs::MoveBaseGoal goal;
    goal.target_pose.header.frame_id = "map";
    goal.target_pose.header.stamp = ros::Time::now();
    goal.target_pose.pose.position.x = next_goal.x;
    goal.target_pose.pose.position.y = next_goal.y;
    goal.target_pose.pose.orientation.z = sin(next_goal.yaw / 2.0);
    goal.target_pose.pose.orientation.w = cos(next_goal.yaw / 2.0);

    ROS_INFO("Sending goal: (%.2f, %.2f)", next_goal.x, next_goal.y);

    // Set callback correctly
    ac->sendGoal(goal, actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction>::SimpleDoneCallback(
        [] (const actionlib::SimpleClientGoalState& state, const move_base_msgs::MoveBaseResultConstPtr& result) {
            sendNextGoal(); // Call next goal after reaching the current one
        }
    ));
}

// Service callback (Non-blocking)
bool navigateCallback(bot_control::goToGoal::Request &req, bot_control::goToGoal::Response &res) {
    if (locations.find(req.location) == locations.end()) {
        ROS_WARN("Invalid location: %s", req.location.c_str());
        res.success = false;
        return true;
    }

    // Cancel any ongoing goal and clear the queue
    ac->cancelAllGoals();
    std::queue<GoalPoint>().swap(goal_queue);  // Efficient way to clear queue

    // Queue goals: kitchen → requested → home
    goal_queue.push(locations["kitchen"]);
    goal_queue.push(locations[req.location]);
    goal_queue.push(locations["home"]);

    // Start execution
    sendNextGoal();

    res.success = true;  // Service call returns immediately
    return true;
}

int main(int argc, char** argv) {
    ros::init(argc, argv, "navigation_service");
    ros::NodeHandle nh;

    ac = new MoveBaseClient("move_base", true);
    ROS_INFO("Waiting for move_base server...");
    ac->waitForServer();
    ROS_INFO("Connected to move_base server");

    ros::ServiceServer service = nh.advertiseService("navigate_to_goal", navigateCallback);
    ROS_INFO("Navigation service ready.");
    ros::spin();

    delete ac;
    return 0;
}
