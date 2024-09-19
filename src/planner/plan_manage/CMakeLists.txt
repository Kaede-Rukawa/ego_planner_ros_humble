cmake_minimum_required(VERSION 3.5)
project(ego_planner)

set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -g")

get_filename_component(PROJECT_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../.." ABSOLUTE)
list(APPEND CMAKE_PREFIX_PATH ${PROJECT_ROOT_DIR}/install)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(quadrotor_msgs REQUIRED)
find_package(traj_utils REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(PCL 1.7 REQUIRED)
find_package(message_filters REQUIRED)
find_package(pcl_msgs REQUIRED)
find_package(bspline_opt REQUIRED)
find_package(traj_utils_so REQUIRED)
find_package(plan_env REQUIRED)
set(PLAN_ENV_INCLUDE_DIR "/home/<username>/ego_planner_ros2/install/plan_env/include")
set(PLAN_ENV_LIB_DIR "/home/<username>/ego_planner_ros2/install/plan_env/lib")
set(BSPLINE_OPT_INCLUDE_DIR "/home/<username>/ego_planner_ros2/install/bspline_opt/include")
set(BSPLINE_OPT_LIB_DIR "/home/<username>/ego_planner_ros2/install/bspline_opt/lib")

set(dependencies
  rclcpp
  std_msgs
  geometry_msgs
  quadrotor_msgs
  traj_utils
  traj_utils_so
  cv_bridge
  nav_msgs
  visualization_msgs
  Eigen3
  PCL
  message_filters
  pcl_msgs
  bspline_opt
  plan_env
)

include_directories(
  include
  ${PROJECT_SOURCE_DIR}/include
  ${EIGEN3_INCLUDE_DIR}
  ${PCL_INCLUDE_DIRS}
  ${PROJECT_ROOT_DIR}/src/planner/bspline_opt/include
  ${PROJECT_ROOT_DIR}/src/planner/path_searching/include
  ${PROJECT_ROOT_DIR}/src/planner/plan_env/include
  /opt/ros/humble/include
  /opt/ros/humble/include/pcl_conversions
  ${traj_utils_LIBRARIES}
  ${traj_utils_so_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/../traj_utils_so/include
  ${CMAKE_CURRENT_SOURCE_DIR}/../traj_utils_z/include
  ${PLAN_ENV_INCLUDE_DIR}
  ${BSPLINE_OPT_INCLUDE_DIR}
)

link_directories(
  ${PROJECT_ROOT_DIR}/install/lib
  ${traj_utils_LIBRARY_DIRS}
  ${traj_utils_so_LIBRARY_DIRS}
  ${bspline_opt_LIBRARY_DIRS}
  ${plan_env_LIBRARY_DIRS}
)

add_executable(ego_planner_node
  src/ego_planner_node.cpp 
  src/ego_replan_fsm.cpp
  src/planner_manager.cpp
)

ament_target_dependencies(ego_planner_node ${dependencies})

target_link_libraries(ego_planner_node
  ${traj_utils_LIBRARIES}
  ${traj_utils_so_LIBRARIES}
  ${bspline_opt_LIBRARIES}
  ${PCL_LIBRARIES}
  ${plan_env_LIBRARIES}
)

add_executable(traj_server src/traj_server.cpp)

ament_target_dependencies(traj_server ${dependencies})

target_link_libraries(traj_server
  ${traj_utils_LIBRARIES}
  ${traj_utils_so_LIBRARIES}
  ${bspline_opt_LIBRARIES}
  ${PCL_LIBRARIES}
  ${plan_env_LIBRARIES}
)

install(TARGETS
  traj_server
  ego_planner_node
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY launch config
  DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
