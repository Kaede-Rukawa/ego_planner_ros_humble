#include "rclcpp/rclcpp.hpp"
#include <visualization_msgs/msg/marker.hpp>

#include <plan_manage/ego_replan_fsm.h>

using namespace ego_planner;

int main(int argc, char **argv)
{

  // ros::init(argc, argv, "ego_planner_node");
  // ros::NodeHandle nh("~");
  rclcpp::init(argc, argv);
  auto nh = std::make_shared<rclcpp::Node>("ego_planner_node");//这个只是说节点的名字叫这个
  EGOReplanFSM rebo_replan;

  rebo_replan.init(nh);

  // ros::Duration(1.0).sleep();
  // ros::spin();
  rclcpp::spin(nh);

  return 0;
}

// #include "rclcpp/rclcpp.hpp"
// #include <csignal>
// #include <visualization_msgs/msg/marker.hpp>

// #include <plan_manage/ego_replan_fsm.h>

// using namespace ego_planner;

// void SignalHandler(int signal) {
//   if(ros::isInitialized() && ros::isStarted() && rclcpp::ok() && !ros::isShuttingDown()){
//     ros::shutdown();
//   }
// }

// int main(int argc, char **argv) {

//   signal(SIGINT, SignalHandler);
//   signal(SIGTERM,SignalHandler);

//   ros::init(argc, argv, "ego_planner_node", ros::init_options::NoSigintHandler);
//   ros::NodeHandle nh("~");

//   EGOReplanFSM rebo_replan;

//   rebo_replan.init(nh);

//   // ros::Duration(1.0).sleep();
//   ros::AsyncSpinner async_spinner(4);
//   async_spinner.start();
//   ros::waitForShutdown();

//   return 0;
// }