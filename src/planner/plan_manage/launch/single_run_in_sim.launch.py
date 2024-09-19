# import os
# from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
import os
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration
from launch.conditions import IfCondition
from launch.actions import DeclareLaunchArgument
# from ament_index_python.packages import get_package_prefix
# from launch.substitutions import LaunchConfiguration
# from launch.actions import ExecuteProcess

def generate_launch_description():
    drone_id_ = "0"
    odometry_topic = "visual_slam/odom"
    map_size_x_ = LaunchConfiguration('map_size_x_', default='50.0')
    map_size_y_ = LaunchConfiguration('map_size_y_', default='25.0')
    map_size_z_ = LaunchConfiguration('map_size_z_', default='2.0')

    random_forest_para_dir = os.path.join(get_package_share_directory('ego_planner'), 'config', 'random_forest.yaml')
    rviz_para_dir = os.path.join(get_package_share_directory('ego_planner'), 'config', 'ros2_ego_rviz.rviz')
    # print(rviz_para_dir)
    # rviz_node_1=DeclareLaunchArgument(
    #     'use_rviz',
    #     default_value='true',
    #     description='Launch RViz2 if true'
    # ),
    rviz_node = Node(
        package="rviz2",
        executable="rviz2",
        name="rviz2",
        output="screen",
        arguments=['-d', rviz_para_dir]
    )
    random_forest_node = Node(
        package='map_generator',
        executable='random_forest',
        output='screen',
        parameters=[random_forest_para_dir,]
    )
    traj_server_node = Node(
        package='ego_planner',
        executable='traj_server',
        output='screen',
        name="drone_"+drone_id_+"_traj_server",
        # parameters=[local_planner_para_dir,]
        remappings=[
        # 重映射 position_cmd 话题
            ('position_cmd', "/drone_"+drone_id_+"_planning/pos_cmd"),

            # 重映射 planning/bspline 话题
            ('planning/bspline', "/drone_"+drone_id_+"_planning/bspline"),
            ],

    )

    advanced_param_launch = os.path.join(get_package_share_directory('ego_planner'), "launch",'advanced_param.launch.py')
    included_launch_advanced_param = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(advanced_param_launch),
        launch_arguments={
                            'drone_id':drone_id_,
                          'map_size_x_': map_size_x_,
                          'map_size_y_': map_size_y_,
                          'map_size_z_':map_size_z_,
                          'obj_num_set': LaunchConfiguration('obj_num_set', default='10'),
                          'camera_pose_topic': LaunchConfiguration('camera_pose_topic', default='pcl_render_node/camera_pose'),
                          'depth_topic': LaunchConfiguration('depth_topic', default='pcl_render_node/depth'),
                          'cloud_topic': LaunchConfiguration('cloud_topic', default='pcl_render_node/cloud'),
                          'cx': LaunchConfiguration('cx', default='321.04638671875'),
                          'cy': LaunchConfiguration('cy', default='243.44969177246094'),
                          'fx': LaunchConfiguration('fx', default='387.229248046875'),
                          'fy': LaunchConfiguration('fy', default='387.229248046875'),
                          'max_vel': LaunchConfiguration('max_vel', default='2.0'),
                          'max_acc': LaunchConfiguration('max_acc', default='6.0 '),
                          'planning_horizon': LaunchConfiguration('planning_horizon', default='7.5'),
                          'use_distinctive_trajs': LaunchConfiguration('use_distinctive_trajs', default='True'),
                          'flight_type': LaunchConfiguration('flight_type', default='1'),
                          'point_num': LaunchConfiguration('point_num', default='4'),
                          'point0_x': LaunchConfiguration('point0_x', default='15.0'),
                          'point0_y': LaunchConfiguration('point0_y', default='0.0'),
                          'point0_z': LaunchConfiguration('point0_z', default='1.0'),
                          'point1_x': LaunchConfiguration('point1_x', default='-15.0 '),
                          'point1_y': LaunchConfiguration('point1_y', default='0.0 '),
                          'point1_z': LaunchConfiguration('point1_z', default='1.0 '),
                          'point2_x': LaunchConfiguration('point2_x', default='15.0 '),
                          'point2_y': LaunchConfiguration('point2_y', default='0.0 '),
                          'point2_z': LaunchConfiguration('point2_z', default='1.0 '),
                          'point3_x': LaunchConfiguration('point3_x', default='-15.0 '),
                          'point3_y': LaunchConfiguration('point3_y', default='0.0 '),
                          'point3_z': LaunchConfiguration('point3_z', default='1.0 '),
                          'point4_x': LaunchConfiguration('point4_x', default='15.0 '),
                          'point4_y': LaunchConfiguration('point4_y', default='0.0 '),
                          'point4_z': LaunchConfiguration('point4_z', default='1.0 '),
                          "name_of_ego_planner_node": LaunchConfiguration('name_of_ego_planner_node', default="drone_"+drone_id_+"_ego_planner_node"),
                          'odom_world': LaunchConfiguration('odom_world', default='/drone_'+drone_id_+'_'+odometry_topic),
                          'planning/bspline': LaunchConfiguration('planning/bspline', default='/drone_'+drone_id_+'_planning/bspline'),
                          'planning/data_display': LaunchConfiguration('planning/data_display', default='/drone_'+drone_id_+'_planning/data_display'),
                          'grid_map/odom': LaunchConfiguration('grid_map/odom', default='/drone_'+drone_id_+'_'+odometry_topic),
                          'grid_map/cloud': LaunchConfiguration('grid_map/cloud', default='/drone_'+drone_id_+'_pcl_render_node/cloud'),
                          'grid_map/pose': LaunchConfiguration('grid_map/pose', default='/drone_'+drone_id_+'_pcl_render_node/camera_pose'),
                          'grid_map/depth': LaunchConfiguration('grid_map/depth', default='/drone_'+drone_id_+'_pcl_render_node/depth'),
                          'grid_map/occupancy_inflate': LaunchConfiguration('grid_map/occupancy_inflate', default='/drone_'+drone_id_+'_ego_planner_node/grid_map/occupancy_inflate'),
                          'optimal_list': LaunchConfiguration('optimal_list', default='/drone_'+drone_id_+'_ego_planner_node/optimal_list'),


                          
                          #TAG 这里实在没办法把字符串传过去再拼接，但是可以直接传过去直接做为那边函数的形参输入用，所以这里只能每次多输入点节点名  以及关于drone_id_的话题了
                          }.items()
    )

    simulator_launch = os.path.join(get_package_share_directory('ego_planner'), "launch",'simulator.launch.py')
    included_launch_simulator = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(simulator_launch),
        launch_arguments={
                            'drone_id':drone_id_,
                          'map_size_x_': map_size_x_,
                          'map_size_y_': map_size_y_,
                          'map_size_z_': map_size_z_,
                          'init_x_': LaunchConfiguration('init_x_', default='-15.0'),
                          'init_y_': LaunchConfiguration('init_y_', default='0.0'),
                          'init_z_': LaunchConfiguration('init_z_', default='0.1'),
                          'odometry_topic': LaunchConfiguration('odometry_topic', default=odometry_topic),
                          'name_of_poscmd_2_odom': LaunchConfiguration('name_of_poscmd_2_odom', default='drone_'+drone_id_+'_poscmd_2_odom'),
                          'command': LaunchConfiguration('command', default='drone_'+drone_id_+'_planning/pos_cmd'),
                          'odometry': LaunchConfiguration('odometry', default='drone_'+drone_id_+'_'+odometry_topic),
                          'optimal_list': LaunchConfiguration('optimal_list', default='/drone_'+drone_id_+'_ego_planner_node/optimal_list'),

                          'name_of_odom_visualization': LaunchConfiguration('name_of_odom_visualization', default='drone_'+drone_id_+'_odom_visualization'),
                          'odom': LaunchConfiguration('odom', default='/drone_'+drone_id_+'_visual_slam/odom'),
                          'robot': LaunchConfiguration('robot', default='/drone_'+drone_id_+'_odom_visualization/robot'),
                          'path': LaunchConfiguration('path', default='/drone_'+drone_id_+'_odom_visualization/path'),
                          
                          'name_of_local_sensing_node': LaunchConfiguration('name_of_local_sensing_node', default='drone_'+drone_id_+'_pcl_render_node'),
                          'pcl_render_node/cloud': LaunchConfiguration('pcl_render_node/cloud', default='drone_'+drone_id_+'_pcl_render_node/cloud'),

                          }.items()
    )
    


    ld = LaunchDescription()
    # ld.add_action(rviz_node_1)
    ld.add_action(rviz_node)
    ld.add_action(included_launch_advanced_param)
    ld.add_action(random_forest_node)
    ld.add_action(traj_server_node)
    ld.add_action(included_launch_simulator)

    return ld
