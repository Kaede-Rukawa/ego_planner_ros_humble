cmake_minimum_required(VERSION 3.5)
project(bridge_test)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(traj_utils REQUIRED)
find_package(quadrotor_msgs REQUIRED)

add_executable(subscriber src/test.cpp)
ament_target_dependencies(subscriber rclcpp traj_utils quadrotor_msgs)
add_executable(subscriber_bspline src/test1.cpp)
ament_target_dependencies(subscriber_bspline rclcpp traj_utils )
install(TARGETS
  subscriber
  subscriber_bspline
  DESTINATION lib/${PROJECT_NAME})

ament_package()
