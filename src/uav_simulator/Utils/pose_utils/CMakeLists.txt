cmake_minimum_required(VERSION 3.5)
project(pose_utils)

set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_FLAGS "-O3 -Wall -g")
ADD_COMPILE_OPTIONS(-std=c++11 )
ADD_COMPILE_OPTIONS(-std=c++14 )

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)

# find_package(catkin REQUIRED COMPONENTS
#   #armadillo
#   roscpp
# )
set(MY_DEPENDENCIES
  rclcpp
  # PCL
)
# catkin_package(
#   INCLUDE_DIRS include
#   LIBRARIES pose_utils
# #  CATKIN_DEPENDS geometry_msgs nav_msgs
# #  DEPENDS system_lib
# )

find_package(Armadillo REQUIRED)

include_directories(
    # ${catkin_INCLUDE_DIRS}
    ${ARMADILLO_INCLUDE_DIRS}
    include
    )

add_library(pose_utils SHARED
   ${ARMADILLO_LIBRARIES}
   src/pose_utils.cpp)
ament_target_dependencies(pose_utils
${MY_DEPENDENCIES} 
)
install(TARGETS
  pose_utils
  DESTINATION lib/${PROJECT_NAME}
)
# install(
#   TARGETS bspline_opt
#   ARCHIVE DESTINATION lib
#   LIBRARY DESTINATION lib
#   RUNTIME DESTINATION bin
# )
# install(DIRECTORY launch
#   DESTINATION share/${PROJECT_NAME}
# )
# install(DIRECTORY config
#   DESTINATION share/${PROJECT_NAME}/#把存yaml文件的文件夹config移到share
# )
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