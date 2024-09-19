#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
// #include <pcl/search/kdtree.h>
#include <pcl/kdtree/kdtree_flann.h>
#include <pcl_conversions/pcl_conversions.h>
#include <iostream>

#include <geometry_msgs/msg/pose_stamped.hpp>

#include <geometry_msgs/msg/vector3.hpp>
#include <math.h>
#include <nav_msgs/msg/odometry.hpp>

// #include <ros/console.h>
#include "rclcpp/logging.hpp"
#include "rclcpp/rclcpp.hpp"
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <Eigen/Eigen>
#include <random>

using namespace std;

// pcl::search::KdTree<pcl::PointXYZ> kdtreeLocalMap;
pcl::KdTreeFLANN<pcl::PointXYZ> kdtreeLocalMap;
vector<int> pointIdxRadiusSearch;
vector<float> pointRadiusSquaredDistance;

random_device rd;
// default_random_engine eng(4);
default_random_engine eng(rd()); 
uniform_real_distribution<double> rand_x;
uniform_real_distribution<double> rand_y;
uniform_real_distribution<double> rand_w;
uniform_real_distribution<double> rand_h;
uniform_real_distribution<double> rand_inf;

rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr _local_map_pub;
rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr _all_map_pub;
rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr click_map_pub_;
rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr _odom_sub;

vector<double> _state;

int _obs_num;
double _x_size, _y_size, _z_size;
double _x_l, _x_h, _y_l, _y_h, _w_l, _w_h, _h_l, _h_h;
double _z_limit, _sensing_range, _resolution, _sense_rate, _init_x, _init_y;
double _min_dist;

bool _map_ok = false;
bool _has_odom = false;

int circle_num_;
double radius_l_, radius_h_, z_l_, z_h_;
double theta_;
uniform_real_distribution<double> rand_radius_;
uniform_real_distribution<double> rand_radius2_;
uniform_real_distribution<double> rand_theta_;
uniform_real_distribution<double> rand_z_;

sensor_msgs::msg::PointCloud2 globalMap_pcd;
pcl::PointCloud<pcl::PointXYZ> cloudMap;

sensor_msgs::msg::PointCloud2 localMap_pcd;
pcl::PointCloud<pcl::PointXYZ> clicked_cloud_;

void RandomMapGenerate() {
  pcl::PointXYZ pt_random;

  rand_x = uniform_real_distribution<double>(_x_l, _x_h);
  rand_y = uniform_real_distribution<double>(_y_l, _y_h);
  rand_w = uniform_real_distribution<double>(_w_l, _w_h);
  rand_h = uniform_real_distribution<double>(_h_l, _h_h);

  rand_radius_ = uniform_real_distribution<double>(radius_l_, radius_h_);
  rand_radius2_ = uniform_real_distribution<double>(radius_l_, 1.2);
  rand_theta_ = uniform_real_distribution<double>(-theta_, theta_);
  rand_z_ = uniform_real_distribution<double>(z_l_, z_h_);

  // generate polar obs
  for (int i = 0; i < _obs_num; i++) {
    double x, y, w, h;
    x = rand_x(eng);
    y = rand_y(eng);
    w = rand_w(eng);

    x = floor(x / _resolution) * _resolution + _resolution / 2.0;
    y = floor(y / _resolution) * _resolution + _resolution / 2.0;

    int widNum = ceil(w / _resolution);

    for (int r = -widNum / 2.0; r < widNum / 2.0; r++)
      for (int s = -widNum / 2.0; s < widNum / 2.0; s++) {
        h = rand_h(eng);
        int heiNum = ceil(h / _resolution);
        for (int t = -20; t < heiNum; t++) {
          pt_random.x = x + (r + 0.5) * _resolution + 1e-2;
          pt_random.y = y + (s + 0.5) * _resolution + 1e-2;
          pt_random.z = (t + 0.5) * _resolution + 1e-2;
          cloudMap.points.push_back(pt_random);
        }
      }
  }

  // generate circle obs
  for (int i = 0; i < circle_num_; ++i) {
    double x, y, z;
    x = rand_x(eng);
    y = rand_y(eng);
    z = rand_z_(eng);

    x = floor(x / _resolution) * _resolution + _resolution / 2.0;
    y = floor(y / _resolution) * _resolution + _resolution / 2.0;
    z = floor(z / _resolution) * _resolution + _resolution / 2.0;

    Eigen::Vector3d translate(x, y, z);

    double theta = rand_theta_(eng);
    Eigen::Matrix3d rotate;
    rotate << cos(theta), -sin(theta), 0.0, sin(theta), cos(theta), 0.0, 0, 0,
        1;

    double radius1 = rand_radius_(eng);
    double radius2 = rand_radius2_(eng);

    // draw a circle centered at (x,y,z)
    Eigen::Vector3d cpt;
    for (double angle = 0.0; angle < 6.282; angle += _resolution / 2) {
      cpt(0) = 0.0;
      cpt(1) = radius1 * cos(angle);
      cpt(2) = radius2 * sin(angle);

      // inflate
      Eigen::Vector3d cpt_if;
      for (int ifx = -0; ifx <= 0; ++ifx)
        for (int ify = -0; ify <= 0; ++ify)
          for (int ifz = -0; ifz <= 0; ++ifz) {
            cpt_if = cpt + Eigen::Vector3d(ifx * _resolution, ify * _resolution,
                                           ifz * _resolution);
            cpt_if = rotate * cpt_if + Eigen::Vector3d(x, y, z);
            pt_random.x = cpt_if(0);
            pt_random.y = cpt_if(1);
            pt_random.z = cpt_if(2);
            cloudMap.push_back(pt_random);
          }
    }
  }

  cloudMap.width = cloudMap.points.size();
  cloudMap.height = 1;
  cloudMap.is_dense = true;

  RCLCPP_WARN(rclcpp::get_logger("random_map_sensing"), "Finished generate random map ");

  kdtreeLocalMap.setInputCloud(cloudMap.makeShared());

  _map_ok = true;
}

void RandomMapGenerateCylinder() {
  pcl::PointXYZ pt_random;

  vector<Eigen::Vector2d> obs_position;

  rand_x = uniform_real_distribution<double>(_x_l, _x_h);
  rand_y = uniform_real_distribution<double>(_y_l, _y_h);
  rand_w = uniform_real_distribution<double>(_w_l, _w_h);
  rand_h = uniform_real_distribution<double>(_h_l, _h_h);
  rand_inf = uniform_real_distribution<double>(0.5, 1.5);

  rand_radius_ = uniform_real_distribution<double>(radius_l_, radius_h_);
  rand_radius2_ = uniform_real_distribution<double>(radius_l_, 1.2);
  rand_theta_ = uniform_real_distribution<double>(-theta_, theta_);
  rand_z_ = uniform_real_distribution<double>(z_l_, z_h_);

  // generate polar obs
  for (int i = 0; i < _obs_num && rclcpp::ok(); i++) {
    double x, y, w, h, inf;
    x = rand_x(eng);
    y = rand_y(eng);
    w = rand_w(eng);
    inf = rand_inf(eng);
    
    bool flag_continue = false;
    for ( auto p : obs_position )
      if ( (Eigen::Vector2d(x,y) - p).norm() < _min_dist /*metres*/ )
      {
        i--;
        flag_continue = true;
        break;
      }
    if ( flag_continue ) continue;

    obs_position.push_back( Eigen::Vector2d(x,y) );
    

    x = floor(x / _resolution) * _resolution + _resolution / 2.0;
    y = floor(y / _resolution) * _resolution + _resolution / 2.0;

    int widNum = ceil((w*inf) / _resolution);
    double radius = (w*inf) / 2;

    for (int r = -widNum / 2.0; r < widNum / 2.0; r++)
      for (int s = -widNum / 2.0; s < widNum / 2.0; s++) {
        h = rand_h(eng);
        int heiNum = ceil(h / _resolution);
        for (int t = -20; t < heiNum; t++) {
          double temp_x = x + (r + 0.5) * _resolution + 1e-2;
          double temp_y = y + (s + 0.5) * _resolution + 1e-2;
          double temp_z = (t + 0.5) * _resolution + 1e-2;
          if ( (Eigen::Vector2d(temp_x,temp_y) - Eigen::Vector2d(x,y)).norm() <= radius )
          {
            pt_random.x = temp_x;
            pt_random.y = temp_y;
            pt_random.z = temp_z;
            cloudMap.points.push_back(pt_random);
          }
        }
      }
  }

  // generate circle obs
  for (int i = 0; i < circle_num_; ++i) {
    double x, y, z;
    x = rand_x(eng);
    y = rand_y(eng);
    z = rand_z_(eng);

    x = floor(x / _resolution) * _resolution + _resolution / 2.0;
    y = floor(y / _resolution) * _resolution + _resolution / 2.0;
    z = floor(z / _resolution) * _resolution + _resolution / 2.0;

    Eigen::Vector3d translate(x, y, z);

    double theta = rand_theta_(eng);
    Eigen::Matrix3d rotate;
    rotate << cos(theta), -sin(theta), 0.0, sin(theta), cos(theta), 0.0, 0, 0,
        1;

    double radius1 = rand_radius_(eng);
    double radius2 = rand_radius2_(eng);

    // draw a circle centered at (x,y,z)
    Eigen::Vector3d cpt;
    for (double angle = 0.0; angle < 6.282; angle += _resolution / 2) {
      cpt(0) = 0.0;
      cpt(1) = radius1 * cos(angle);
      cpt(2) = radius2 * sin(angle);

      // inflate
      Eigen::Vector3d cpt_if;
      for (int ifx = -0; ifx <= 0; ++ifx)
        for (int ify = -0; ify <= 0; ++ify)
          for (int ifz = -0; ifz <= 0; ++ifz) {
            cpt_if = cpt + Eigen::Vector3d(ifx * _resolution, ify * _resolution,
                                           ifz * _resolution);
            cpt_if = rotate * cpt_if + Eigen::Vector3d(x, y, z);
            pt_random.x = cpt_if(0);
            pt_random.y = cpt_if(1);
            pt_random.z = cpt_if(2);
            cloudMap.push_back(pt_random);
          }
    }
  }

  cloudMap.width = cloudMap.points.size();
  cloudMap.height = 1;
  cloudMap.is_dense = true;


  RCLCPP_WARN(rclcpp::get_logger("random_map_sensing"), "Finished generate random map ");
  kdtreeLocalMap.setInputCloud(cloudMap.makeShared());

  _map_ok = true;
}

void rcvOdometryCallbck(const nav_msgs::msg::Odometry::SharedPtr odom) {
  if (odom->child_frame_id == "X" || odom->child_frame_id == "O") return;
  _has_odom = true;

  _state = {odom->pose.pose.position.x,
            odom->pose.pose.position.y,
            odom->pose.pose.position.z,
            odom->twist.twist.linear.x,
            odom->twist.twist.linear.y,
            odom->twist.twist.linear.z,
            0.0,
            0.0,
            0.0};
}

int i = 0;
void pubSensedPoints() {
  // if (i < 10) {
  pcl::toROSMsg(cloudMap, globalMap_pcd);
  globalMap_pcd.header.frame_id = "world";
  _all_map_pub->publish(globalMap_pcd);
  // }

  return;

  /* ---------- only publish points around current position ---------- */
  if (!_map_ok || !_has_odom) return;

  pcl::PointCloud<pcl::PointXYZ> localMap;

  pcl::PointXYZ searchPoint(_state[0], _state[1], _state[2]);
  pointIdxRadiusSearch.clear();
  pointRadiusSquaredDistance.clear();

  pcl::PointXYZ pt;

  if (isnan(searchPoint.x) || isnan(searchPoint.y) || isnan(searchPoint.z))
    return;

  if (kdtreeLocalMap.radiusSearch(searchPoint, _sensing_range,
                                  pointIdxRadiusSearch,
                                  pointRadiusSquaredDistance) > 0) {
    for (size_t i = 0; i < pointIdxRadiusSearch.size(); ++i) {
      pt = cloudMap.points[pointIdxRadiusSearch[i]];
      localMap.points.push_back(pt);
    }
  } else {

    RCLCPP_ERROR(rclcpp::get_logger("random_map_sensing"), "[Map server] No obstacles .");
    return;
  }

  localMap.width = localMap.points.size();
  localMap.height = 1;
  localMap.is_dense = true;

  pcl::toROSMsg(localMap, localMap_pcd);
  localMap_pcd.header.frame_id = "world";
  _local_map_pub->publish(localMap_pcd);
}

void clickCallback(const geometry_msgs::msg::PoseStamped& msg) {
  double x = msg.pose.position.x;
  double y = msg.pose.position.y;
  double w = rand_w(eng);
  double h;
  pcl::PointXYZ pt_random;

  x = floor(x / _resolution) * _resolution + _resolution / 2.0;
  y = floor(y / _resolution) * _resolution + _resolution / 2.0;

  int widNum = ceil(w / _resolution);

  for (int r = -widNum / 2.0; r < widNum / 2.0; r++)
    for (int s = -widNum / 2.0; s < widNum / 2.0; s++) {
      h = rand_h(eng);
      int heiNum = ceil(h / _resolution);
      for (int t = -1; t < heiNum; t++) {
        pt_random.x = x + (r + 0.5) * _resolution + 1e-2;
        pt_random.y = y + (s + 0.5) * _resolution + 1e-2;
        pt_random.z = (t + 0.5) * _resolution + 1e-2;
        clicked_cloud_.points.push_back(pt_random);
        cloudMap.points.push_back(pt_random);
      }
    }
  clicked_cloud_.width = clicked_cloud_.points.size();
  clicked_cloud_.height = 1;
  clicked_cloud_.is_dense = true;

  pcl::toROSMsg(clicked_cloud_, localMap_pcd);
  localMap_pcd.header.frame_id = "world";
  click_map_pub_->publish(localMap_pcd);

  cloudMap.width = cloudMap.points.size();

  return;
}

int main(int argc, char** argv) {


  rclcpp::init(argc, argv);
  auto n = std::make_shared<rclcpp::Node>("random_map_sensing");//这个只是说节点的名字叫这个

  _local_map_pub = n->create_publisher<sensor_msgs::msg::PointCloud2>("/map_generator/local_cloud", 1);
  _all_map_pub = n->create_publisher<sensor_msgs::msg::PointCloud2>("/map_generator/global_cloud", 1);

  _odom_sub = n->create_subscription<nav_msgs::msg::Odometry>("/odometry",50,rcvOdometryCallbck);

  click_map_pub_ =
      n->create_publisher<sensor_msgs::msg::PointCloud2>("/pcl_render_node/local_map", 1);
  // ros::Subscriber click_sub = n.subscribe("/goal", 10, clickCallback);
  n->declare_parameter<double>("init_state_x",0.0);
  n->declare_parameter<double>("init_state_y",0.0);
  n->declare_parameter<double>("map/x_size",50.0);
  n->declare_parameter<double>("map/y_size",50.0);
  n->declare_parameter<double>("map/z_size",5.0);
  n->declare_parameter<int>("map/obs_num",30);
  n->declare_parameter<double>("map/resolution",0.1);
  n->declare_parameter<int>("map/circle_num",30);
  n->declare_parameter<double>("ObstacleShape/lower_rad",0.3);
  n->declare_parameter<double>("ObstacleShape/upper_rad",0.8);
  n->declare_parameter<double>("ObstacleShape/lower_hei",3.0);
  n->declare_parameter<double>("ObstacleShape/upper_hei",7.0);
  n->declare_parameter<double>("ObstacleShape/radius_l",7.0);
  n->declare_parameter<double>("ObstacleShape/radius_h",7.0);
  n->declare_parameter<double>("ObstacleShape/z_l",7.0);
  n->declare_parameter<double>("ObstacleShape/z_h",7.0);
  n->declare_parameter<double>("ObstacleShape/theta",7.0);
  n->declare_parameter<double>("sensing/radius",10.0);
  n->declare_parameter<double>("sensing/rate",10.0);
  n->declare_parameter<double>("min_distance",1.0);

  n->get_parameter("init_state_x",_init_x);
  n->get_parameter("init_state_y",_init_y);
  n->get_parameter("map/x_size",_x_size);
  n->get_parameter("map/y_size",_y_size);
  n->get_parameter("map/z_size",_z_size);
  n->get_parameter("map/obs_num",_obs_num);
  n->get_parameter("map/resolution",_resolution);
  n->get_parameter("map/circle_num",circle_num_);
  n->get_parameter("ObstacleShape/lower_rad",_w_l);
  n->get_parameter("ObstacleShape/upper_rad",_w_h);
  n->get_parameter("ObstacleShape/lower_hei",_h_l);
  n->get_parameter("ObstacleShape/upper_hei",_h_h);
  n->get_parameter("ObstacleShape/radius_l",radius_l_);
  n->get_parameter("ObstacleShape/radius_h",radius_h_);
  n->get_parameter("ObstacleShape/z_l",z_l_);
  n->get_parameter("ObstacleShape/z_h",z_h_);
  n->get_parameter("ObstacleShape/theta",theta_);
  n->get_parameter("sensing/radius",_sensing_range);
  n->get_parameter("sensing/rate",_sense_rate);
  n->get_parameter("min_distance",_min_dist);
  bool test_param=0;
  n->declare_parameter<bool>("test_param",0);
  n->get_parameter("test_param",test_param);
  if(!test_param)
  std::cout<<"localPlanner获取参数失败！ "<<std::endl;

  
  // n.param("init_state_x", _init_x, 0.0);
  // n.param("init_state_y", _init_y, 0.0);
  // n.param("map/x_size", _x_size, 50.0);
  // n.param("map/y_size", _y_size, 50.0);
  // n.param("map/z_size", _z_size, 5.0);
  // n.param("map/obs_num", _obs_num, 30);
  // n.param("map/resolution", _resolution, 0.1);
  // n.param("map/circle_num", circle_num_, 30);
  // n.param("ObstacleShape/lower_rad", _w_l, 0.3);
  // n.param("ObstacleShape/upper_rad", _w_h, 0.8);
  // n.param("ObstacleShape/lower_hei", _h_l, 3.0);
  // n.param("ObstacleShape/upper_hei", _h_h, 7.0);
  // n.param("ObstacleShape/radius_l", radius_l_, 7.0);
  // n.param("ObstacleShape/radius_h", radius_h_, 7.0);
  // n.param("ObstacleShape/z_l", z_l_, 7.0);
  // n.param("ObstacleShape/z_h", z_h_, 7.0);
  // n.param("ObstacleShape/theta", theta_, 7.0);
  // n.param("sensing/radius", _sensing_range, 10.0);
  // n.param("sensing/rate", _sense_rate, 10.0);
  // n.param("min_distance", _min_dist, 1.0);

  _x_l = -_x_size / 2.0;
  _x_h = +_x_size / 2.0;

  _y_l = -_y_size / 2.0;
  _y_h = +_y_size / 2.0;

  _obs_num = min(_obs_num, (int)_x_size * 10);
  _z_limit = _z_size;

  // ros::Duration(0.5).sleep();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  unsigned int seed = rd();
  // unsigned int seed = 2433201515;
  cout << "seed=" << seed << endl;
  eng.seed(seed);

  // RandomMapGenerate();
  RandomMapGenerateCylinder();

  // ros::Rate loop_rate(_sense_rate);
  rclcpp::Rate loop_rate(10);

  while (rclcpp::ok()) {
    pubSensedPoints();
    rclcpp::spin_some(n);
    loop_rate.sleep();
  }
}