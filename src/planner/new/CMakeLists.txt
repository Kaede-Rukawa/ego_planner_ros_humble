cmake_minimum_required(VERSION 3.5)
project(new)

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
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)
find_package(rclcpp  REQUIRED)
#EXPL 1.那边编译好以后这边只需要find_package即可  
find_package(quadrotor_msgs  REQUIRED)
add_executable(testpub src/testpub.cpp)
ament_target_dependencies(testpub rclcpp quadrotor_msgs)
# rosidl_target_interfaces(testpub
#   ${PROJECT_NAME} "rosidl_typesupport_cpp")
# rosidl_get_typesupport_target(cpp_typesupport_target
# ${PROJECT_NAME} rosidl_typesupport_cpp)

# target_link_libraries(testpub "${cpp_typesupport_target}")

install(TARGETS 
  testpub
  DESTINATION lib/${PROJECT_NAME})






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

ament_package()
