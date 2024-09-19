set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
cmake_minimum_required(VERSION 3.8)
project(bspline_opt)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 查找依赖包
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(pcl_conversions REQUIRED)
find_package(pcl_ros REQUIRED)
find_package(plan_env REQUIRED)
find_package(path_searching REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(traj_utils_so REQUIRED)

set(PLAN_ENV_INCLUDE_DIR "/home/<username>/ego_planner_ros2/install/plan_env/include")
set(PLAN_ENV_LIB_DIR "/home/<username>/ego_planner_ros2/install/plan_env/lib")


set(dependencies
  rclcpp
  std_msgs
  geometry_msgs
  nav_msgs
  visualization_msgs
  pcl_conversions
  pcl_ros
  plan_env
  path_searching
  cv_bridge
  traj_utils_so
)

if(TARGET plan_env::plan_env)
  get_target_property(PLAN_ENV_INCLUDE_DIRS plan_env::plan_env INTERFACE_INCLUDE_DIRECTORIES)
else()
  set(PLAN_ENV_INCLUDE_DIRS "${plan_env_DIR}/../../../include")
endif()

include_directories(
  include
  ${pcl_conversions_INCLUDE_DIRS}
  ${pcl_ros_INCLUDE_DIRS}
  ${path_searching_INCLUDE_DIRS}
  ${cv_bridge_INCLUDE_DIRS}
  /opt/ros/humble/include
  /opt/ros/humble/include/pcl_conversions
  ${traj_utils_so_INCLUDE_DIRS}
  ${PLAN_ENV_INCLUDE_DIR}
)

add_library(${PROJECT_NAME} SHARED
  src/bspline_optimizer.cpp
  src/uniform_bspline.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

ament_target_dependencies(${PROJECT_NAME} 
  ${dependencies}
)

target_link_libraries(${PROJECT_NAME}
  ${traj_utils_so_LIBRARIES}
  ${plan_env_LIBRARIES}
  ${path_searching_LIBRARIES}
  ${PCL_LIBRARIES}
  plan_env::plan_env
)

install(TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}Targets
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)

install(DIRECTORY include/${PROJECT_NAME}/
  DESTINATION include/${PROJECT_NAME}
)

ament_export_targets(${PROJECT_NAME}Targets HAS_LIBRARY_TARGET)
ament_export_include_directories(include)
ament_export_libraries(${PROJECT_NAME})
ament_export_dependencies(${dependencies})

ament_package()
