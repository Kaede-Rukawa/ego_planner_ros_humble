#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "quadrotor_msgs/msg/position_command.hpp"
using std::placeholders::_1;

class MinimalSubscriber : public rclcpp::Node
{
  public:
    MinimalSubscriber()
    : Node("minimal_subscriber")
    {
      subscription_ = this->create_subscription<quadrotor_msgs::msg::PositionCommand>(
      "topic", 10, std::bind(&MinimalSubscriber::topic_callback, this, _1));
    }

  private:
    void topic_callback(const quadrotor_msgs::msg::PositionCommand::SharedPtr msg) const
    {
      // RCLCPP_INFO(this->get_logger(), "I heard custom_msgs::msg::TestFoxy : '%d'", msg->position.x);
      std::cout<<msg->position.x<<std::endl;
    }
    rclcpp::Subscription<quadrotor_msgs::msg::PositionCommand>::SharedPtr subscription_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MinimalSubscriber>());
  rclcpp::shutdown();
  return 0;
}
