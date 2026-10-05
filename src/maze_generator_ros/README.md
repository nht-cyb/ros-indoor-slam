# maze_generator_ros
A ros package to generate URDF and SDF model of a maze which can be used for gazebo or Rviz.

To generate a new maze you have to load parameters from maze_config.yaml and run node generator_maze, which generates four files:
<br>model.urdf
<br>model.sdf
<br>maze_start_goal.yaml - 
<br>robot_start.launch - spawning of iRobot create in starting position

To change parameters of the maze such as wall thickness, height and length, starting area and goal area change maze_config.yaml
<br>To generate maze from picture.png you have to draw it as png., put it in maze_desc directory and change the name in maze_config.yaml
<br>To see example please see files inside of the repository.

Other related repositories:
<br>Smadas/irobot_create_ros - irobot create urdf, sdf description
<br>Smadas/create_control_ros - ROS package prepared to insert your algorithm for controling simulate iRobot create

Just make sure that your workspace is called catknin_ws otherwise change it in the code.

You can use pikoPixel to draw a new maze.

Please be aware of the different naming of repo and actual ROS package.
