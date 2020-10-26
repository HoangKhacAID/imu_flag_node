cmake_minimum_required(VERSION 3.0.2)
project(imu_flag_node)

find_package(	catkin REQUIRED
		roscpp
		std_msgs
		message_generation)


include_directories(
${catkin_INCLUDE_DIRS}
)

add_message_files(
   		FILES
		reg65.msg
)

generate_messages(
	   	DEPENDENCIES
   		std_msgs
)


add_executable(imu_flag_node src/imu_flag_node.cpp)
add_dependencies(imu_flag_node ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(imu_flag_node
   ${catkin_LIBRARIES})

