cmake_minimum_required(VERSION 3.8)
project(odom_visualization)

# 使用 C++14 标准
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_FLAGS "-O3 -Wall -g")

# 查找依赖包
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
include_directories(${nav_msgs_INCLUDE_DIRS})
find_package(geometry_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(quadrotor_msgs REQUIRED)
find_package(tf2_msgs REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(Armadillo REQUIRED)

set(dependencies
  rclcpp
  sensor_msgs
  nav_msgs
  geometry_msgs
  visualization_msgs
  quadrotor_msgs
  tf2_msgs
  tf2_ros
)

# 包含目录
get_filename_component(PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}" DIRECTORY)
include_directories(
  include
  ${ARMADILLO_INCLUDE_DIRS}
  ${PROJECT_ROOT}/pose_utils/include
  /opt/ros/humble/include
  /opt/ros/humble/include/nav_msgs
)

# 添加可执行文件
add_executable(odom_visualization src/odom_visualization.cpp)

# 链接依赖
ament_target_dependencies(odom_visualization ${dependencies})

get_filename_component(SOURCE_CMAKELIST_DIR "${CMAKE_SOURCE_DIR}" ABSOLUTE)
get_filename_component(PROJECT_ROOT_DIR "${SOURCE_CMAKELIST_DIR}/../../../.." ABSOLUTE)
list(APPEND CMAKE_PREFIX_PATH ${PROJECT_ROOT_DIR}/install)

target_link_libraries(odom_visualization
  ${ARMADILLO_LIBRARIES}
  ${PROJECT_ROOT_DIR}/install/pose_utils/lib/pose_utils/libpose_utils.so
)

# 安装目标
install(TARGETS
  odom_visualization
  DESTINATION lib/${PROJECT_NAME}
)

# 安装其他文件
install(DIRECTORY meshes
  DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
