#include<ros/ros.h>
#include"imu_flag_node/reg65.h"
#include"imu_flag_node/reg37.h"
#include"sensor_msgs/Imu.h"
#include"std_msgs/Bool.h"
template <class T> T String_To_Num(std::string str);

class imu_flag_node_class
{
public:
imu_flag_node_class(){
	imu_triggered_param = false;
	first_reg37 = true;
	greater_than_4_mins = false;
	speed_limit_reached = false;
	first_speed_limit_reached = false;
	stopped_for_10_seconds = false;

	R37_sub = nh.subscribe("/PLC_R37Data",1000, &imu_flag_node_class::Reg37_Callback, this);
	imu_triggered_sub = nh.subscribe("/imu/data", 1000, &imu_flag_node_class::imu_callback, this);
	start_GBE_pub = nh.advertise<std_msgs::Bool>("/StartGBE",1000);
}

void Reg37_Callback(const imu_flag_node::reg37::ConstPtr msg)
{
	if(imu_triggered_param == true){

		if(first_reg37 == true)
		{
			ROS_INFO("first_reg37 is actually set");
			initial_time = msg->header.stamp.toSec();
			first_reg37 = false;
		}
		else
		{
			current_time = msg->header.stamp.toSec();
			time_diff = current_time - initial_time;
			ROS_INFO("time_diff is %f", time_diff);
			if (time_diff > 240)
			{
				ROS_INFO("time > 240 %f", time_diff);
				greater_than_4_mins == true;
			}
		}

	double current_speed = String_To_Num<float>(msg->Vechical_Speed_Feedback)/100;
	if( current_speed > 25)
	{
		speed_limit_reached == true;
	}

	if(speed_limit_reached == true && first_speed_limit_reached == true)
	{
		initial_time_stopped = msg->header.stamp.toSec();
	}
	else if(speed_limit_reached == true && first_speed_limit_reached == false)
	{
		current_time_stopped = msg->header.stamp.toSec();
		if(current_time_stopped - initial_time_stopped > 10)
		{
			stopped_for_10_seconds = true;
		}
	}

	if(	current_speed == 0 &&
		speed_limit_reached == true &&
		greater_than_4_mins == true &&
		stopped_for_10_seconds == true &&
		flag_00 == false)
	{
		start_GBE_pub.publish(true);
		ros::Duration(2).sleep();
		start_GBE_pub.publish(false);
	}
	}
}

void imu_callback(const sensor_msgs::Imu::ConstPtr)
{
	imu_triggered_param = true;
}

// void dummy (const data::ConstPtr msg)
// {
// 	if (msg->value[28] == 0 && msg->value[29] ==  0)
// 		flag_00 = true;
// 	if 

// }
private:
	ros::NodeHandle nh;
	bool imu_triggered_param;
	bool first_reg37;
	bool greater_than_4_mins;
	bool speed_limit_reached;
	bool stopped_for_10_seconds;
	bool first_speed_limit_reached;
	bool flag_00;
	double current_time;
	double initial_time;

	double initial_time_stopped;
	double current_time_stopped;
	double time_diff;
	ros::Subscriber R37_sub;
	ros::Subscriber status_sub;
	ros::Subscriber imu_triggered_sub;
	ros::Publisher start_GBE_pub;
};

int main(int argc, char** argv)
{
	ros::init(argc, argv, "iaodfnioaen");

	imu_flag_node_class imu_flag_node_object;

	while(ros::ok())
	{
		ros::spinOnce();
	}
	return 0;
}

template <class T>
T String_To_Num(std::string str)
{
	std::stringstream ss(str);
	T converted_int;
	ss >> converted_int;
	return converted_int;
}