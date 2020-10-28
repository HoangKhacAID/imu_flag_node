#include<ros/ros.h>
#include"imu_flag_node/reg65.h"
#include"imu_flag_node/reg37.h"
#include"sensor_msgs/Imu.h"
#include"std_msgs/Bool.h"
#include"diagnostic_msgs/KeyValue.h"
#include"imu_flag_node/imu_flag_custom_msgs.h"
template <class T> T String_To_Num(std::string str);

class imu_flag_node_class
{
public:
imu_flag_node_class(){

	imu_flag_msgs.speed_reached_25_before_GBE = false;
	imu_flag_msgs.speed_reached_25_after_GBE = false;
	imu_flag_msgs.bias_estimation_complete = false;
	imu_flag_msgs.wait_4_minutes_before_GBE = false;
	flag_start_GBE = false;
	imu_ready_flag = false;
	GBE_success_flag = false;
	greater_than_4_mins = false;
	first_imu = true;
	greater_than_4_mins_print = true;

	R37_sub = nh.subscribe("/PLC_R37Data",1000, &imu_flag_node_class::Reg37_Callback, this);
	imu_triggered_sub = nh.subscribe("/imu/data", 1000, &imu_flag_node_class::imu_callback, this);
	status_sub = nh.subscribe("/status",1000, &imu_flag_node_class::status_callback,this);
}

void Reg37_Callback(const imu_flag_node::reg37::ConstPtr msg)
{
	current_speed = String_To_Num<double>(msg->Vechical_Speed_Feedback)/100;

	if(	current_speed > 25 && flag_start_GBE == false)
		{
		ROS_INFO("speed reached 25 kmph before starting GBE");
		imu_flag_msgs.speed_reached_25_before_GBE = true;
		}
	else if(current_speed > 25 && GBE_success_flag == true)
		{
		imu_flag_msgs.speed_reached_25_after_GBE = true;
		ROS_INFO("speed reached 25 kmph after finishing GBE");
		ROS_INFO("IMU bias estimation complete");
		}
}	

void imu_callback(const sensor_msgs::Imu::ConstPtr msg)
{
	if(first_imu == true)
	{
		ROS_INFO("imu started");
		initial_time = msg->header.stamp.toSec();
		first_imu = false;
	}
	else
	{
		current_time = msg->header.stamp.toSec();
		time_diff = current_time - initial_time;
		if (time_diff > 240)
		{
			if( imu_flag_msgs.wait_4_minutes_before_GBE == false)
			{
				ROS_INFO("4 minute has passsed");
			}
			imu_flag_msgs.wait_4_minutes_before_GBE == true;
		}
	}
}

void status_callback(const diagnostic_msgs::KeyValue::ConstPtr msg)
{
	if(msg->value[27] == 1 && msg->value[28] == 1)
	{
		ROS_INFO("GBE started");
		flag_start_GBE = true;
	}
	if(flag_start_GBE == true && msg->value[27] == 0 && msg->value[28] == 0)
	{
		ROS_INFO("GBE finished and succeeded");
		GBE_success_flag = true;
	}
	
}

imu_flag_node::imu_flag_custom_msgs get_imu_flag_msgs()
{
	return imu_flag_msgs;
}

private:
	ros::NodeHandle nh;
	imu_flag_node::imu_flag_custom_msgs imu_flag_msgs;
	double current_speed;
	bool flag_start_GBE;
	bool first_imu;
	bool greater_than_4_mins;
	bool GBE_success_flag;
	bool greater_than_4_mins_print;
	bool imu_ready_flag;
	double current_time;
	double initial_time;

	double time_diff;
	ros::Subscriber R37_sub;
	ros::Subscriber status_sub;
	ros::Subscriber imu_triggered_sub;
	ros::Publisher start_GBE_pub;
};

int main(int argc, char** argv)
{
	ros::init(argc, argv, "iaodfnioaen");
	ros::NodeHandle main_nh;
	imu_flag_node_class imu_flag_node_object;
	ros::Publisher imu_ready_flag_pub = main_nh.advertise<std_msgs::Bool>("/imu_ready", 1000);
	imu_flag_node::imu_flag_custom_msgs imu_ready_flag_msgs;
	while(ros::ok())
	{	
		imu_ready_flag_msgs = imu_flag_node_object.get_imu_flag_msgs();
		imu_ready_flag_pub.publish(imu_ready_flag_msgs);
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