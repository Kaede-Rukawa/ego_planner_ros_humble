cmake_minimum_required(VERSION 3.8)
project(map_generator)

# 使用 C++14 标准
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -g")

# 查找依赖包
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(pcl_conversions REQUIRED)
find_package(PCL REQUIRED)
find_package(Eigen3 REQUIRED)

# 设置依赖项列表
set(dependencies
  rclcpp
  std_msgs
  geometry_msgs
  nav_msgs
  pcl_conversions
)

# 包含目录
include_directories(
  ${PCL_INCLUDE_DIRS}
  ${EIGEN3_INCLUDE_DIR}
  /opt/ros/humble/include
  /opt/ros/humble/include/nav_msgs
)

# 添加可执行文件
add_executable(random_forest src/random_forest_sensing.cpp)
ament_target_dependencies(random_forest ${dependencies})
target_link_libraries(random_forest ${PCL_LIBRARIES})

# 安装目标
install(TARGETS
  random_forest
  DESTINATION lib/${PROJECT_NAME}
)

# 可选：安装启动文件和配置文件
# install(DIRECTORY launch DESTINATION share/${PROJECT_NAME})
# install(DIRECTORY config DESTINATION share/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
