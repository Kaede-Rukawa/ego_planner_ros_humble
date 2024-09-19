#include <nav_msgs/msg/odometry.hpp>
#include <nav_msgs/msg/path.hpp>
#include <pcl/filters/voxel_grid.h>
#include <pcl/kdtree/kdtree_flann.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/search/kdtree.h>
#include <pcl_conversions/pcl_conversions.h>
#include "rclcpp/rclcpp.hpp"
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <Eigen/Dense>
#include <fstream>
#include <iostream>
#include <pcl/search/impl/kdtree.hpp>
#include <vector>

using namespace std;
using namespace Eigen;

// ros::Publisher pub_cloud;
rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr pub_cloud;
sensor_msgs::msg::PointCloud2 local_map_pcl;
sensor_msgs::msg::PointCloud2 local_depth_pcl;

// ros::Subscriber odom_sub;
rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_sub;
// ros::Subscriber global_map_sub, local_map_sub;
rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr global_map_sub;
rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr local_map_sub;
rclcpp::TimerBase::SharedPtr local_sensing_timer;

bool has_global_map(false);
bool has_local_map(false);
bool has_odom(false);

nav_msgs::msg::Odometry::SharedPtr _odom;

double sensing_horizon, sensing_rate, estimation_rate;
double _x_size, _y_size, _z_size;
double _gl_xl, _gl_yl, _gl_zl;
double _resolution, _inv_resolution;
int _GLX_SIZE, _GLY_SIZE, _GLZ_SIZE;

rclcpp::Time last_odom_stamp = rclcpp::Time::max();

inline Eigen::Vector3d gridIndex2coord(const Eigen::Vector3i& index) {
  Eigen::Vector3d pt;
  pt(0) = ((double)index(0) + 0.5) * _resolution + _gl_xl;
  pt(1) = ((double)index(1) + 0.5) * _resolution + _gl_yl;
  pt(2) = ((double)index(2) + 0.5) * _resolution + _gl_zl;

  return pt;
};

inline Eigen::Vector3i coord2gridIndex(const Eigen::Vector3d& pt) {
  Eigen::Vector3i idx;
  idx(0) = std::min(std::max(int((pt(0) - _gl_xl) * _inv_resolution), 0),
                    _GLX_SIZE - 1);
  idx(1) = std::min(std::max(int((pt(1) - _gl_yl) * _inv_resolution), 0),
                    _GLY_SIZE - 1);
  idx(2) = std::min(std::max(int((pt(2) - _gl_zl) * _inv_resolution), 0),
                    _GLZ_SIZE - 1);

  return idx;
};

void rcvOdometryCallbck(const nav_msgs::msg::Odometry::SharedPtr odom) {
  /*if(!has_global_map)
    return;*/
  has_odom = true;
  _odom = odom;
}

pcl::PointCloud<pcl::PointXYZ> _cloud_all_map, _local_map;
pcl::VoxelGrid<pcl::PointXYZ> _voxel_sampler;
sensor_msgs::msg::PointCloud2 _local_map_pcd;

pcl::search::KdTree<pcl::PointXYZ> _kdtreeLocalMap;
vector<int> _pointIdxRadiusSearch;
vector<float> _pointRadiusSquaredDistance;

void rcvGlobalPointCloudCallBack(
    const sensor_msgs::msg::PointCloud2::SharedPtr pointcloud_map) {
  if (has_global_map) return;

  // ROS_WARN("Global Pointcloud received..");
  RCLCPP_WARN(rclcpp::get_logger("pcl_render"), "Global Pointcloud received..");

  pcl::PointCloud<pcl::PointXYZ> cloud_input;
  pcl::fromROSMsg(*pointcloud_map, cloud_input);

  _voxel_sampler.setLeafSize(0.1f, 0.1f, 0.1f);
  _voxel_sampler.setInputCloud(cloud_input.makeShared());
  _voxel_sampler.filter(_cloud_all_map);

  _kdtreeLocalMap.setInputCloud(_cloud_all_map.makeShared());

  has_global_map = true;
}

void renderSensedPoints() {
  if (!has_global_map || !has_odom) return;

  Eigen::Quaterniond q;
  q.x() = _odom->pose.pose.orientation.x;
  q.y() = _odom->pose.pose.orientation.y;
  q.z() = _odom->pose.pose.orientation.z;
  q.w() = _odom->pose.pose.orientation.w;

  Eigen::Matrix3d rot;
  rot = q;
  Eigen::Vector3d yaw_vec = rot.col(0);

  _local_map.points.clear();
  pcl::PointXYZ searchPoint(_odom->pose.pose.position.x,
                            _odom->pose.pose.position.y,
                            _odom->pose.pose.position.z);
  _pointIdxRadiusSearch.clear();
  _pointRadiusSquaredDistance.clear();

  pcl::PointXYZ pt;
  if (_kdtreeLocalMap.radiusSearch(searchPoint, sensing_horizon,
                                   _pointIdxRadiusSearch,
                                   _pointRadiusSquaredDistance) > 0) {
    for (size_t i = 0; i < _pointIdxRadiusSearch.size(); ++i) {
      pt = _cloud_all_map.points[_pointIdxRadiusSearch[i]];

      // if ((fabs(pt.z - _odom.pose.pose.position.z) / (pt.x - _odom.pose.pose.position.x)) >
      //     tan(M_PI / 12.0))
      //   continue;
      if ((fabs(pt.z - _odom->pose.pose.position.z) / sensing_horizon) >
          tan(M_PI / 6.0))
        continue; 

      Vector3d pt_vec(pt.x - _odom->pose.pose.position.x,
                      pt.y - _odom->pose.pose.position.y,
                      pt.z - _odom->pose.pose.position.z);

      if (pt_vec.normalized().dot(yaw_vec) < 0.5) continue; 

      _local_map.points.push_back(pt);
    }
  } else {
    return;
  }

  _local_map.width = _local_map.points.size();
  _local_map.height = 1;
  _local_map.is_dense = true;

  pcl::toROSMsg(_local_map, _local_map_pcd);
  _local_map_pcd.header.frame_id = "map";

  pub_cloud->publish(_local_map_pcd);
}

void rcvLocalPointCloudCallBack(
    const sensor_msgs::msg::PointCloud2::SharedPtr pointcloud_map) {
  // do nothing, fix later
}

int main(int argc, char** argv) {
  // ros::init(argc, argv, "pcl_render");
  // ros::NodeHandle nh("~");
  rclcpp::init(argc, argv);
  auto nh = std::make_shared<rclcpp::Node>("pcl_render");//这个只是说节点的名字叫这个
  nh->declare_parameter<double>("sensing_horizon",5.0);
  nh->declare_parameter<double>("sensing_rate",30.0);
  nh->declare_parameter<double>("estimation_rate",30.0);
  nh->declare_parameter<double>("map/x_size",26.0);
  nh->declare_parameter<double>("map/y_size",20.0);
  nh->declare_parameter<double>("map/z_size",3.0);
  nh->get_parameter("sensing_horizon",sensing_horizon);
  nh->get_parameter("sensing_rate",sensing_rate);
  nh->get_parameter("estimation_rate",estimation_rate);
  nh->get_parameter("map/x_size",_x_size);
  nh->get_parameter("map/y_size",_y_size);
  nh->get_parameter("map/z_size",_z_size);

  // nh.getParam("sensing_horizon", sensing_horizon);
  // nh.getParam("sensing_rate", sensing_rate);
  // nh.getParam("estimation_rate", estimation_rate);

  // nh.getParam("map/x_size", _x_size);
  // nh.getParam("map/y_size", _y_size);
  // nh.getParam("map/z_size", _z_size);

  // subscribe point cloud
  // global_map_sub = nh.subscribe("global_map", 1, rcvGlobalPointCloudCallBack);
  global_map_sub = nh->create_subscription<sensor_msgs::msg::PointCloud2>("/global_map",1,rcvGlobalPointCloudCallBack);

  // local_map_sub = nh.subscribe("local_map", 1, rcvLocalPointCloudCallBack);
  local_map_sub = nh->create_subscription<sensor_msgs::msg::PointCloud2>("/local_map",1,rcvLocalPointCloudCallBack);

  // odom_sub = nh.subscribe("odometry", 50, rcvOdometryCallbck);
  odom_sub = nh->create_subscription<nav_msgs::msg::Odometry>("/odometry",50,rcvOdometryCallbck);

  // publisher depth image and color image
  // pub_cloud =
      // nh.advertise<sensor_msgs::msg::PointCloud2>("pcl_render_node/cloud", 10);
  pub_cloud = nh->create_publisher<sensor_msgs::msg::PointCloud2>("pcl_render_node/cloud", 10);

  double sensing_duration = 1.0 / sensing_rate * 2.5;

  // local_sensing_timer =
  //     nh.createTimer(ros::Duration(sensing_duration), renderSensedPoints);
  local_sensing_timer = nh->create_wall_timer(std::chrono::milliseconds(int(sensing_duration*1000)), renderSensedPoints);

  _inv_resolution = 1.0 / _resolution;

  _gl_xl = -_x_size / 2.0;
  _gl_yl = -_y_size / 2.0;
  _gl_zl = 0.0;

  _GLX_SIZE = (int)(_x_size * _inv_resolution);
  _GLY_SIZE = (int)(_y_size * _inv_resolution);
  _GLZ_SIZE = (int)(_z_size * _inv_resolution);

  rclcpp::Rate rate(100);
  bool status = rclcpp::ok();
  while (status) {
    rclcpp::spin_some(nh);
    status = rclcpp::ok();
    rate.sleep();
  }
}
