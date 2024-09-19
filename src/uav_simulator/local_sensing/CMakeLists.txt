cmake_minimum_required(VERSION 3.8)
project(local_sensing_node)

# 使用 C++14 标准
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_BUILD_TYPE Release)

set(ENABLE_CUDA false)

if(ENABLE_CUDA)
  # CUDA 相关配置（如果需要）
else(ENABLE_CUDA)
  # 查找依赖包
  find_package(ament_cmake REQUIRED)
  find_package(rclcpp REQUIRED)
  find_package(pcl_ros REQUIRED)
  find_package(sensor_msgs REQUIRED)
  find_package(geometry_msgs REQUIRED)
  find_package(nav_msgs REQUIRED)
  find_package(quadrotor_msgs REQUIRED)
  find_package(Eigen3 REQUIRED)
  find_package(PCL REQUIRED)

  set(dependencies
    rclcpp
    pcl_ros
    sensor_msgs
    geometry_msgs
    nav_msgs
    quadrotor_msgs
  )

  include_directories(
    ${EIGEN3_INCLUDE_DIR}
    ${PCL_INCLUDE_DIRS}
    ${nav_msgs_INCLUDE_DIRS}
    ${rclcpp_INCLUDE_DIRS}
    ${sensor_msgs_INCLUDE_DIRS}
    ${geometry_msgs_INCLUDE_DIRS}
    ${quadrotor_msgs_INCLUDE_DIRS}
  )

  # 添加 pkg-config 查找的路径
  include_directories(
  ${nav_msgs_INCLUDE_DIRS}
  /opt/ros/humble/include
  /opt/ros/humble/include/nav_msgs 
)

  add_executable(pcl_render_node src/pointcloud_render_node.cpp)
  ament_target_dependencies(pcl_render_node ${dependencies})
  target_link_libraries(pcl_render_node ${PCL_LIBRARIES})

  install(TARGETS
    pcl_render_node
    DESTINATION lib/${PROJECT_NAME}
  )

  # 添加头文件安装
  install(DIRECTORY include/${PROJECT_NAME}/
    DESTINATION include/${PROJECT_NAME}
    FILES_MATCHING PATTERN "*.h"
    PATTERN ".svn" EXCLUDE
  )

  if(BUILD_TESTING)
    find_package(ament_lint_auto REQUIRED)
    ament_lint_auto_find_test_dependencies()
  endif()

  ament_package()
endif(ENABLE_CUDA)
