#!/bin/sh
xterm  -e  "roslaunch bot_sim world.launch" &
sleep 5
xterm  -e  "roslaunch bot_control control.launch" &
sleep 5
xterm  -e  "rosrun bot_control butler" &
sleep 5
xterm  -e  "rosservice call /navigate_to_goal "B""