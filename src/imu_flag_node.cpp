#include<ros/ros.h>
#include"imu_flag_node/reg65.h"

void Reg65_Callback(const imu_flag_node::reg65::ConstPtr msg)
{
	//Todo: Monitor the speed
}

int main(int argc, char** argv)
{
	ros::init(argc, argv, "imu_flag_node");
	ros::NodeHandle nh;
	ros::Subscriber R65_sub = nh.subscribe("/PLC_R65Data", 1000, Reg65_Callback);
	while(ros::ok())
	{
		ros::spinOnce();
	}
	return 0;
}