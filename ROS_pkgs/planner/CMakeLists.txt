cmake_minimum_required(VERSION 2.8.3)
project(planner)

## Add support for C++11, supported in ROS Kinetic and newer
set (CMAKE_CXX_STANDARD 11)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  roscpp
  roslib
  caros_common_msgs
  caros_control_msgs
  caros_common
  caros_control
  std_msgs
  caros_common_robwork
  geometry_msgs
  message_generation
  rosconsole
  roslint
)

###################################
##         RobWork setup         ##
###################################
# Set the RobWork root (edit in .bashrc if necessary)
SET(RW_ROOT $ENV{RW_ROOT})

# Use RobWork
MESSAGE("-- RobWork root: " ${RW_ROOT})
SET(RobWork_DIR ${RW_ROOT}/cmake)
FIND_PACKAGE(RobWork REQUIRED)
INCLUDE_DIRECTORIES( ${ROBWORK_INCLUDE_DIRS} ${catkin_INCLUDE_DIRS} include)
LINK_DIRECTORIES( ${ROBWORK_LIBRARY_DIRS} )

###################################
## catkin specific configuration ##
###################################
catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES rovi2_rob
#  CATKIN_DEPENDS roscpp
#  DEPENDS system_lib
)

###########
## Build ##
###########
#include_directories( ${catkin_INCLUDE_DIRS} )

add_library(AnytimePlanning SHARED src/AnytimePlanning.cpp)
add_library(URRobot SHARED src/URRobot.cpp)

add_executable(planner src/main.cpp)
target_link_libraries(planner AnytimePlanning URRobot ${catkin_LIBRARIES} ${ROBWORK_LIBRARIES} ${ROS_LIBRARIES})
add_dependencies(planner caros_common_msgs caros_control_msgs caros_control)

MESSAGE(${PROJECT_NAME} " done!")
