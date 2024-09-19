#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "traj_utils/msg/bspline.hpp"
using std::placeholders::_1;

class MinimalSubscriber : public rclcpp::Node
{
  public:
    MinimalSubscriber()
    : Node("minimal_subscriber")
    {
      subscription_ = this->create_subscription<traj_utils::msg::Bspline>(
      "topic", 10, std::bind(&MinimalSubscriber::topic_callback, this, _1));
    }

  private:
    void topic_callback(const traj_utils::msg::Bspline::SharedPtr msg) const
    {
      // RCLCPP_INFO(this->get_logger(), "I heard custom_msgs::msg::TestFoxy : '%d'", msg->position.x);
      std::cout<<msg->drone_id<<std::endl;
    }
    rclcpp::Subscription<traj_utils::msg::Bspline>::SharedPtr subscription_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MinimalSubscriber>());
  rclcpp::shutdown();
  return 0;
}
