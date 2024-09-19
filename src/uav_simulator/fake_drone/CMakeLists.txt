cmake_minimum_required(VERSION 3.8)
project(poscmd_2_odom)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 查找依赖包
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(quadrotor_msgs REQUIRED)

set(dependencies
  rclcpp
  std_msgs
  sensor_msgs
  nav_msgs
  geometry_msgs
  visualization_msgs
  quadrotor_msgs
)

include_directories(
  include
  ${nav_msgs_INCLUDE_DIRS}
  /opt/ros/humble/include
  /opt/ros/humble/include/nav_msgs
)

add_executable(poscmd_2_odom src/poscmd_2_odom.cpp)
ament_target_dependencies(poscmd_2_odom ${dependencies})

install(TARGETS
  poscmd_2_odom
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
