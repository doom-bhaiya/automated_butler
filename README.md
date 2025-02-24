This project is for the first round interview for Goat Robotics

Problem STatement:

The French door café has asked us to build a robot that will be used as a butler. The orders are taken
from the customers by the host of the restaurant and will be passed on to the kitchen to make them
ready. Once the food is ready the butler will collect the food and give it to the customer who has
ordered the food. Now the restaurant becomes busy and the cost of the employees are shooting up.
To find a solution for it they have approached us.
We are going to give a solution for the same with our robot. We will be able to handle a busy day
by replacing the butler with a robot. The robot will run the errands faster and in a more efficient
way.
Our robot’s general workflow will be as follows, our robot will be in the home position initially.
When the order is received by the robot, it will travel to the kitchen to collect the food that has to be
delivered and move to the customers table for the delivery of the food. Once the order is completed
the robot will move to its initial home position.
This café expects us to manage the delivery for 3 tables (table1, table2, table3).

Solution:

Step 1 : 
Creating the environment

I designed a basic world with a place for home position and a kitchen like place and three tables to mimic the given scenario
I made a basic two wheeled robot with a castor wheel for support. The bot is provided with a depth camera, lidar, imu sensors to allow it to do SLAM and navigate itself. In the URDF I have added the diff drive plugin to move the bot easily.

Now the bot publishes the following topics joint_states, odom, lidar_scan, depth_camera_data etc.

Step 2 :
Simulataneous localization and Mapping

Our Robot can perceieve the world using the sensors we gave, but it still could not see through walls, so it could not plan for anything which is out of its sight. Along with that It needs to know where it is so that it can move to a table of choice rather than some random table and it should know where the home position is and the kitchen is. To tackle all these problems, a MAP is neccesary. 
To map the environment we use Gmapping algorithm and teleoperate the robot across the entire environment in Gazebo, then we save the map to reuse it later. 

Step 3 :
Make it move 

In a new launch file we write code for localization and navigation. For localization we can use AMCL localization package. It works well for these kinds of tasks and its simple and yet effective. For Navigation we are using the ROS move base package and we have tuned the parameters according to our requirements. Then we make a custom rviz and save it so that we can load it automatically. 
As of now our robot can move if it is manually given goal position and orientation and we can see the movement and the planning in rviz.

Step 4 :

Making a service to listen to what table we need to serve, 
The service then calls the move base action to go to kitchen first
Then it calls the move base action to go to the table
Then it calls the move base action to go to the home position

The service returns true once it verifies whether the given string is in the list of table or not. This is done so as to make our robot interrupt to make it more to a more prioritized order.

This is implemented by passing the goal to a new thread and the main thread just returns true so as to close the service request immediately.








Algorithm configurations:

For the localization :
AMCL algorithm is chosen, as it is simple and effective

For SLAM :
Gmapping is chosen, as we usually have lidars for these types of actions and gmapping is the one of the simplest SLAM algorithm
And it is having good support in ROS as well

For Navigation :
ROS Navigation package move base is used for Navigation
The tuning for the move base was done accordingly but since the environment is designed with very small gaps, the robot finds it difficult to properly move to the goals
