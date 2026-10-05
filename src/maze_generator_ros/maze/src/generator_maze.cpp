
#include <sstream>

//files libraries
#include <iostream>
#include <fstream>

//looking for home directory
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <string> 

//random number
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "ros/ros.h"
//#include <opencv2/core.hpp>
//#include <opencv2/imgcodecs.hpp>
#include "opencv4/opencv2/core.hpp"
#include "opencv4/opencv2/imgcodecs.hpp"
#include "opencv4/opencv2/highgui.hpp"
//#include <opencv2/highgui.hpp>

double WALL_LENGTH = 0.4;
double WALL_HEIGHT = 0.2;
double WALL_THICKNESS = 0.05;

void put_wall(int x_maze, int y_maze, int s_wall, int n_wall, int x_maze_const, int y_maze_const, std::ofstream& create_world, std::ofstream& create_world_rviz);
std::string get_directory(std::string file_name, std::string file_num, std::string file_type);

int main(int argc, char **argv)
{
    ros::init(argc, argv, "generator_maze");
    ros::NodeHandle n;

    //START defining constants for maze
    unsigned int x_maze = 10; //3 number of x squares
    unsigned int y_maze = 10;	//3 number of y squares
    const unsigned int n_walls = 4; //4 number of walls
    const unsigned int wall_rand = 80; //constant for randomizing where to put walls 0 - 100, 100 is least probable
    //END defining constants for maze

    // read config parameters
    std::string maze_desc_file_name;
    n.getParam("maze_name", maze_desc_file_name);
    n.getParam("wall_length", WALL_LENGTH);
    n.getParam("wall_height", WALL_HEIGHT);
    n.getParam("wall_thisckness", WALL_THICKNESS);

    //parameter server
    //vypocet startu a ciela


    // open maze description
    std::string image_path = get_directory("maze/" + maze_desc_file_name, "", "png");
    cv::Mat img_maze = cv::imread(image_path, cv::IMREAD_COLOR);
    x_maze = img_maze.cols/2;
    y_maze = img_maze.rows/2;

    // open maze start goal file
    std::ofstream maze_start_goal_file;
    std::string maze_start_goal_path;
    maze_start_goal_path = get_directory("maze/config/maze_start_goal", "", "yaml");
    std::cout << maze_start_goal_path << std::endl;
    remove(maze_start_goal_path.c_str()); //removing the old file
    maze_start_goal_file.open (maze_start_goal_path.c_str());

    // open robot start spawn file
    std::ofstream robot_start_spawn_file;
    std::string robot_start_spawn_path;
    robot_start_spawn_path = get_directory("maze/launch/robot_start", "", "launch");
    std::cout << robot_start_spawn_path << std::endl;
    remove(robot_start_spawn_path.c_str()); //removing the old file
    robot_start_spawn_file.open (robot_start_spawn_path.c_str());

	int i = 0, j = 0, k = 0; //variables for loops
	srand(time(NULL));

	//START opening file for simulation
	std::ofstream create_world;
	std::string model_directory;
	model_directory = get_directory("maze/model", "", "sdf");
	std::cout << model_directory << std::endl;
	remove(model_directory.c_str()); //removing the old file
	create_world.open (model_directory.c_str());
	//END opening file for simulation

	//START opening file for rviz
	std::ofstream create_world_rviz;
	std::string model_directory_rviz;
	model_directory_rviz = get_directory("maze/model", "", "urdf");
    std::cout << model_directory_rviz << std::endl;
	remove(model_directory_rviz.c_str()); //removing the old file
	create_world_rviz.open (model_directory_rviz.c_str());
	//END opening file for rviz

	//START writting head of sdf to file
	create_world << "<?xml version='1.0'?>" << std::endl;
	create_world << "<sdf version='1.4'>" << std::endl;
	create_world << "	<model name=\"my_maze\">" << std::endl;
	create_world << "		<static>true</static>" << std::endl;
	create_world << std::endl;
	//END writting head of sdf to file

	//START writting head of sdf to file
	create_world_rviz << "<?xml version='1.0'?>" << std::endl;
	create_world_rviz << "<robot name=\"my_maze\">" << std::endl;
	create_world_rviz << std::endl;


    create_world_rviz <<  "	<link name=\"odom\">" << std::endl;
	create_world_rviz <<  "	</link>" << std::endl;
    create_world << std::endl;

	create_world_rviz << "	<material name=\"white\">" << std::endl;
	create_world_rviz << "		<color rgba=\"1 1 1 1\"/>" << std::endl;
	create_world_rviz << "	</material>" << std::endl;
	//END writting head of sdf to file

	//START creating maze field
	int my_maze[x_maze][y_maze][n_walls];
	for (i = 0; i < x_maze; i++)
	{
		for (j = 0; j < y_maze; j++)
		{
			for (k = 0; k < n_walls; k++)
			{
				my_maze[i][j][k] = 1;
			}
		}
	}
	//END creating maze field
	
	//START creating map of maze
    //random generating
    for (i = 0; i < x_maze; i++)
    {
        for (j = 0; j < y_maze; j++)
        {
            for (k = 0; k < n_walls; k++)
            {
                if((k == 0)||(k == 1)||((k == 2)&&(i == (x_maze -1)))||((k == 3)&&(j == (y_maze -1))))
                {
                    //if (rand()%100 < wall_rand)
                    //img_maze.at()
                    cv::Vec3b bgrPixel;
                    switch (k)
                    {
                        case 0:
                            bgrPixel = img_maze.at<cv::Vec3b>((i+1)*2-1-1, (j+1)*2-1);
                            break;
                        case 1:
                            bgrPixel = img_maze.at<cv::Vec3b>((i+1)*2-1, (j+1)*2-1-1);
                            break;
                        case 2:
                            bgrPixel = img_maze.at<cv::Vec3b>((i+1)*2-1+1, (j+1)*2-1);
                            break;
                        case 3:
                            bgrPixel = img_maze.at<cv::Vec3b>((i+1)*2-1, (j+1)*2-1+1);
                            break;
                    }
                    if (bgrPixel.val[1])
                    {
                        my_maze[i][j][k] = 0;
                    }
                }
            }
        }
    }
    //inserting start and end. Start is goingto have to be few squares long alley of nothing
    //check feasibility
	//END creating map of maze

	//START drawing walls according to map
	int n_wall = 0; //number of wall
	for (i = 0; i < x_maze; i++)
	{
		for (j = 0; j < y_maze; j++)
		{
			for (k = 0; k < n_walls; k++)
			{
				if (my_maze[i][j][k] == 1)
				{
					if((k == 0)||(k == 1)||((k == 2)&&(i == (x_maze -1)))||((k == 3)&&(j == (y_maze -1))))
					{
						n_wall++;
						put_wall(i, j, k, n_wall, x_maze, y_maze, create_world, create_world_rviz);
					}
				}
			}
		}
	}
	//END drawing wall according to map

	//START writting end of sdf to file
	create_world << "	</model>" << std::endl;
	create_world << "</sdf>" << std::endl;
	//END writting end of sdf to file

	//START writting end of sdf to file
	for(int i=1;i<=n_wall;i++)
	{
		create_world_rviz << "	<joint name=\"walls_to_world"<< i << "\" type=\"fixed\">" << std::endl;
  	create_world_rviz << "		<parent link=\"odom\"/>" << std::endl;
  	create_world_rviz << " 	<child link=\"wall" << i << "\"/>" << std::endl;
		create_world_rviz << " </joint>" << std::endl;
		create_world_rviz << std::endl; 
	}
	create_world_rviz << "</robot>" << std::endl;
	//END writting end of sdf to file



	// compute start and goal area coordinates
	int start_tile_x1 = 0;
    int start_tile_y1 = 0;
    int goal_tile_x1 = 0;
    int goal_tile_y1 = 0;
    int goal_tile_x2 = 0;
    int goal_tile_y2 = 0;
    n.getParam("start_tile/x",start_tile_x1);
    n.getParam("start_tile/y",start_tile_y1);
    n.getParam("goal_tile/x1",goal_tile_x1);
    n.getParam("goal_tile/y1",goal_tile_y1);
    n.getParam("goal_tile/x2",goal_tile_x2);
    n.getParam("goal_tile/y2",goal_tile_y2);

    maze_start_goal_file << "start_area:" << std::endl;
    maze_start_goal_file << "   x1: " << start_tile_x1*WALL_LENGTH - x_maze*WALL_LENGTH/2.0 << std::endl;
    maze_start_goal_file << "   y1: " << start_tile_y1*WALL_LENGTH - WALL_LENGTH/2 - y_maze*WALL_LENGTH/2.0 << std::endl;
    maze_start_goal_file << "   x2: " << start_tile_x1*WALL_LENGTH + WALL_LENGTH - x_maze*WALL_LENGTH/2.0 << std::endl;
    maze_start_goal_file << "   y2: " << start_tile_y1*WALL_LENGTH + WALL_LENGTH/2 - y_maze*WALL_LENGTH/2.0 << std::endl;
    maze_start_goal_file << "goal_area:" << std::endl;
    maze_start_goal_file << "   x1: " << goal_tile_x1*WALL_LENGTH - x_maze*WALL_LENGTH/2.0 << std::endl;
    maze_start_goal_file << "   y1: " << goal_tile_y1*WALL_LENGTH - WALL_LENGTH/2 - y_maze*WALL_LENGTH/2.0 << std::endl;
    maze_start_goal_file << "   x2: " << goal_tile_x2*WALL_LENGTH + WALL_LENGTH - x_maze*WALL_LENGTH/2.0 << std::endl;
    maze_start_goal_file << "   y2: " << goal_tile_y2*WALL_LENGTH + WALL_LENGTH/2 - y_maze*WALL_LENGTH/2.0 << std::endl;

    double robot_spawn_x = 0;
    double robot_spawn_y = 0;
    double st_x1 = start_tile_x1*WALL_LENGTH - x_maze*WALL_LENGTH/2.0;
    double st_y1 = start_tile_y1*WALL_LENGTH - WALL_LENGTH/2 - y_maze*WALL_LENGTH/2.0;
    double st_x2 = start_tile_x1*WALL_LENGTH + WALL_LENGTH - x_maze*WALL_LENGTH/2.0;
    double st_y2 = start_tile_y1*WALL_LENGTH + WALL_LENGTH/2 - y_maze*WALL_LENGTH/2.0;

    robot_spawn_x = st_x1 - (st_x1 - st_x2)/2;
    robot_spawn_y = st_y1 - (st_y1 - st_y2)/2;

    robot_start_spawn_file << "<?xml version='1.0'?>" << std::endl;
    robot_start_spawn_file << "<launch>" << std::endl;
    robot_start_spawn_file << "<node pkg=\"gazebo_ros\" name=\"spawn_model\" type=\"spawn_model\" args=\"-file $(find create)/model-1_4.sdf -sdf -model create -x " << robot_spawn_x << " -y " << robot_spawn_y << " -z 0.1\" />" << std::endl;
    robot_start_spawn_file << "</launch>" << std::endl;

    create_world.close();
    create_world_rviz.close();
    maze_start_goal_file.close();
    robot_start_spawn_file.close();
	return 0;
}

std::string get_directory(std::string file_name, std::string file_num, std::string file_type)
{
	struct passwd *pw = getpwuid(getuid());
	const char *homedir = pw->pw_dir;
	std::string file_directory;
	file_directory = homedir;
	file_directory = file_directory + "/catkin_ws/src/maze_generator_ros/" + file_name + file_num + "." + file_type;
	return file_directory;
}



void put_wall(int x_maze, int y_maze, int s_wall, int n_wall, int x_maze_const, int y_maze_const, std::ofstream& create_world, std::ofstream& create_world_rviz)
{
	double x_maze_wall = 0; 
	double y_maze_wall = 0;
	double r_maze_wall = 0;
	switch (s_wall)
	{
		case 0:
			r_maze_wall = 0;
			x_maze_wall = x_maze*WALL_LENGTH - x_maze_const*WALL_LENGTH/2.0;
			y_maze_wall = y_maze*WALL_LENGTH - y_maze_const*WALL_LENGTH/2.0;
			break;
		case 1:
			r_maze_wall = 1.57;
			x_maze_wall = x_maze*WALL_LENGTH + WALL_LENGTH/2 - x_maze_const*WALL_LENGTH/2.0;
			y_maze_wall = y_maze*WALL_LENGTH - WALL_LENGTH/2 - y_maze_const*WALL_LENGTH/2.0;
			break;
		case 2:
			r_maze_wall = 0;
			x_maze_wall = x_maze*WALL_LENGTH + WALL_LENGTH - x_maze_const*WALL_LENGTH/2.0;
			y_maze_wall = y_maze*WALL_LENGTH - y_maze_const*WALL_LENGTH/2.0;
			break;
		case 3:
			r_maze_wall = 1.57;
			x_maze_wall = x_maze*WALL_LENGTH + WALL_LENGTH/2 - x_maze_const*WALL_LENGTH/2.0;
			y_maze_wall = y_maze*WALL_LENGTH + WALL_LENGTH/2 - y_maze_const*WALL_LENGTH/2.0;
			break;
	}
		
	create_world << "		<link name='wall" << n_wall << "'>" << std::endl;
	create_world << "			<pose>" << x_maze_wall << " " << y_maze_wall << " " << WALL_HEIGHT/2 << " 0 0 " << r_maze_wall << "</pose>" << std::endl;
	create_world << "			<inertial>" << std::endl;
	create_world << "				<mass>1.0</mass>" << std::endl;
	create_world << "			</inertial>" << std::endl;	
	create_world << "			<collision name='collision'>" << std::endl;
	create_world << "				<geometry>" << std::endl;
	create_world << "					<box>" << std::endl;
	create_world << "						<size>" << WALL_THICKNESS << " " << WALL_LENGTH + WALL_THICKNESS << " " << WALL_HEIGHT << "</size>" << std::endl;
	create_world << "					</box>" << std::endl;
	create_world << "				</geometry>" << std::endl;
	create_world << "			</collision>" << std::endl;
	create_world << "			<visual name='visual'>" << std::endl;
	create_world << "				<geometry>" << std::endl;
	create_world << "					<box>" << std::endl;
	create_world << "						<size>" << WALL_THICKNESS << " " << WALL_LENGTH + WALL_THICKNESS << " " << WALL_HEIGHT << "</size>" << std::endl;
	create_world << "					</box>" << std::endl;
	create_world << "				</geometry>" << std::endl;
	create_world << "			</visual>" << std::endl;
	create_world << "		</link>" << std::endl;
	create_world << std::endl;


	create_world_rviz << "	<link name='wall" << n_wall << "'>" << std::endl;
	create_world_rviz << "		<visual name='visual'>" << std::endl;
	create_world_rviz << "			<geometry>" << std::endl;
	create_world_rviz << "				<box size=\"" << WALL_THICKNESS << " " << WALL_LENGTH + WALL_THICKNESS << " " << WALL_HEIGHT << "\"/>" << std::endl;
	create_world_rviz << "			</geometry>" << std::endl;
	create_world_rviz << "			<origin xyz=\"" << x_maze_wall << " " << y_maze_wall << " " << WALL_HEIGHT/2 << "\" rpy=\"0 0 " << r_maze_wall << "\"/>" << std::endl;
	create_world_rviz << "			<material name=\"white\"/>" << std::endl;
	create_world_rviz << "		</visual>" << std::endl;
	create_world_rviz << "	</link>" << std::endl;
	create_world_rviz << std::endl;

}
