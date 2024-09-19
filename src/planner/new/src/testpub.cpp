#include "rclcpp/rclcpp.hpp"
#include "quadrotor_msgs/msg/position_command.hpp" //包含我们创建的msg文件
 
using namespace std::chrono_literals;
 
class AddressBookPublisher : public rclcpp::Node
{
public: 
  AddressBookPublisher()
  : Node("address_book_publisher")
  {
    address_book_publisher_ =
      this->create_publisher<quadrotor_msgs::msg::PositionCommand>("address_book", 10);
      //生成一个节点address_book_publisher以及话题发布者 AddressBookPublisher
 
    auto publish_msg = [this]() -> void {
        quadrotor_msgs::msg::PositionCommand message ;//创建一个消息 AddressBook稍后发送
        message.header.frame_id=11;
        message.position.x=1;
        // message.first_name = "John";
        // message.last_name = "Doe";
        // message.phone_number = "1234567890";
        // message.phone_type = message.PHONE_TYPE_MOBILE;
          //定期发送消息
        // std::cout << "Publishing Contact\nFirst:" << message.first_name <<
        //   "  Last:" << message.last_name << std::endl;
 
        this->address_book_publisher_->publish(message);//发布消息到话题
      };
    timer_ = this->create_wall_timer(1s, publish_msg);//创建一个定时器，每秒调用一次publish_msg
  }
 
private:
  rclcpp::Publisher<quadrotor_msgs::msg::PositionCommand>::SharedPtr address_book_publisher_;
  //声明了一个指向 rclcpp::Publisher 类模板实例的共享指针 ，用于将消息发布到主题。
  //more_interfaces::msg::AddressBook 模板参数指定将要发布的消息类型。SharedPtr 类型是一个智能指针，提供了对象的共享所有权。
  rclcpp::TimerBase::SharedPtr timer_;
  //声明一个指向rclcpp::TimerBase类的共享指针，用于创建定时器，指定时间间隔后调用回调函数。
};
 
 
int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);//初始化
  rclcpp::spin(std::make_shared<AddressBookPublisher>());//运行
  rclcpp::shutdown();//清理ros2客户端库以及中间件资源
 
  return 0;
}