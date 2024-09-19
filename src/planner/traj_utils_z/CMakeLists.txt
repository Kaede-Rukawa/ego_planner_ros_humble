# cmake_minimum_required(VERSION 2.8.3)
# project(traj_utils)

# set(CMAKE_BUILD_TYPE "Release")
# set(CMAKE_CXX_FLAGS "-std=c++11")
# set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -g")

# find_package(catkin REQUIRED COMPONENTS
#   #bspline_opt
#   #path_searching
#   roscpp
#   std_msgs
#   geometry_msgs
#   message_generation
#   #cv_bridge
# )

# find_package(Eigen3 REQUIRED)
# find_package(PCL 1.7 REQUIRED)

# # Generate messages in the 'msg' folder
# add_message_files(
#   FILES
#   Bspline.msg
#   DataDisp.msg
#   MultiBsplines.msg
#   )
  
# # Generate added messages and services with any dependencies listed here
# generate_messages(
#   DEPENDENCIES
#   std_msgs
#   geometry_msgs
# )

# catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES traj_utils 
#  CATKIN_DEPENDS message_runtime
# #  DEPENDS system_lib
# )

# include_directories( 
#     SYSTEM 
#     include 
#     ${catkin_INCLUDE_DIRS}
#     ${Eigen3_INCLUDE_DIRS} 
#     ${PCL_INCLUDE_DIRS}
# )

# link_directories(${PCL_LIBRARY_DIRS})

# add_library( traj_utils
#     src/planning_visualization.cpp 
#     src/polynomial_traj.cpp 
#     )
# target_link_libraries( traj_utils
#     ${catkin_LIBRARIES} 
#     )  

cmake_minimum_required(VERSION 3.8)
project(traj_utils)

set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_FLAGS "-std=c++14")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -g")

# find_package(catkin REQUIRED COMPONENTS
#   #bspline_opt
#   #path_searching
#   roscpp
#   std_msgs
#   geometry_msgs
#   message_generation
#   #cv_bridge
# )
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
# find_package(message_generation REQUIRED)
# find_package(pcl_conversions REQUIRED)

set(MY_DEPENDENCIES
  rclcpp
  std_msgs
  geometry_msgs
  # message_generation
  # pcl_conversions
  # PCL
)
find_package(Eigen3 REQUIRED)
find_package(PCL 1.7 REQUIRED)

# # Generate messages in the 'msg' folder
# add_message_files(
#   FILES
#   Bspline.msg
#   DataDisp.msg
#   MultiBsplines.msg
#   )
  
# # Generate added messages and services with any dependencies listed here
# generate_messages(
#   DEPENDENCIES
#   std_msgs
#   geometry_msgs
# )

# catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES traj_utils 
#  CATKIN_DEPENDS message_runtime
# #  DEPENDS system_lib
# )

include_directories( 
    SYSTEM 
    include 
    # ${catkin_INCLUDE_DIRS}
    ${Eigen3_INCLUDE_DIRS} 
    ${PCL_INCLUDE_DIRS}
)

# link_directories(${PCL_LIBRARY_DIRS})

# add_library( traj_utils
#     src/planning_visualization.cpp 
#     src/polynomial_traj.cpp 
#     )
# ament_target_dependencies(traj_utils
# ${MY_DEPENDENCIES} 
# )
# install(TARGETS
#   traj_utils
#   DESTINATION lib/${PROJECT_NAME}
# )
# target_link_libraries( traj_utils
#     ${catkin_LIBRARIES} 
#     )  
#TAG 这里是msg的点 1.rosidl_default_generators和相关的环境DEPENDENCIES builtin_interfaces
find_package(builtin_interfaces REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(geometry_msgs REQUIRED)
rosidl_generate_interfaces(${PROJECT_NAME} 
"msg/Bspline.msg" 
"msg/DataDisp.msg" 
"msg/MultiBsplines.msg" 
DEPENDENCIES builtin_interfaces geometry_msgs)


if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # uncomment the line when a copyright and license is not present in all source files
  #set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # uncomment the line when this package is not in a git repo
  #set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()
##1.自身工程的.h库
ament_package()