import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_prefix
from launch.substitutions import LaunchConfiguration
from launch.actions import ExecuteProcess
def generate_launch_description():

    poscmd_2_odom_node = Node(
        package='poscmd_2_odom',
        executable='poscmd_2_odom',
        output='screen',
        name=LaunchConfiguration('name_of_poscmd_2_odom'),
        # parameters=[local_planner_para_dir,]
        remappings=[
        # 重映射 position_cmd 话题
            ('command', LaunchConfiguration('command')),
            ('odometry', LaunchConfiguration('odometry')),
            ],
        parameters=[
            # local_planner_para_dir,#9
                    {'init_x': LaunchConfiguration('init_x_')},
                    {'init_y': LaunchConfiguration('init_y_')},
                    {'init_z': LaunchConfiguration('init_z_')},
                    ]
    )
    odom_visualization_node = Node(
        package='odom_visualization',
        executable='odom_visualization',
        output='screen',
        name=LaunchConfiguration('name_of_odom_visualization'),
        # parameters=[local_planner_para_dir,]
        remappings=[
        # 重映射 position_cmd 话题
            ('odom', LaunchConfiguration('odom')),
            ('robot', LaunchConfiguration('robot')),
            
            ('optimal_list', LaunchConfiguration('optimal_list')),
            ('path', LaunchConfiguration('path')),
            # ('odometry', LaunchConfiguration('odometry')),
            ],
        parameters=[
            # local_planner_para_dir,#9
                    {'color/a': 1.0},
                    {'color/r': 0.0},
                    {'color/g': 0.0},
                    {'color/b': 0.0},
                    {'covariance_scale': 100.0},
                    {'robot_scale': 1.0},
                    {'tf45': False},
                    {'drone_id': LaunchConfiguration('drone_id')},
                    # {'robot_scale': 1.0},
                    ]
    )

    local_sensing_node = Node(
        package='local_sensing_node',
        executable='pcl_render_node',
        output='screen',
        name=LaunchConfiguration('name_of_local_sensing_node'),
        # parameters=[local_planner_para_dir,]
        remappings=[
        # 重映射 position_cmd 话题
            ('global_map', "/map_generator/global_cloud"),
            ('odometry', LaunchConfiguration('odometry')),
            ('pcl_render_node/cloud', LaunchConfiguration('pcl_render_node/cloud')),
            # ('path', LaunchConfiguration('path')),
            # ('odometry', LaunchConfiguration('odometry')),  
            ],
        parameters=[
            # local_planner_para_dir,#9
                    {'sensing_horizon': 5.0},
                    {'sensing_rate': 30.0},
                    {'estimation_rate': 30.0},
                    {'map/x_size': LaunchConfiguration('map_size_x_')},
                    {'map/y_size': LaunchConfiguration('map_size_y_')},
                    {'map/z_size': LaunchConfiguration('map_size_z_')},

                    {'cam_width': 640},
                    {'cam_height': 480},
                    {'cam_fx': 387.229248046875},
                    {'cam_fy': 321.04638671875},
                    {'cam_cx': 321.04638671875},
                    {'cam_cy': 243.44969177246094},
                    # {'tf45': False},
                    # {'drone_id': LaunchConfiguration('drone_id')},
                    # {'robot_scale': 1.0},
                    ]
    )
    # print("hhhhhhhhhhhhhhhhhhhhhhhhhhhsimulator------------")
    ld = LaunchDescription()
    ld.add_action(poscmd_2_odom_node)
    ld.add_action(odom_visualization_node)
    ld.add_action(local_sensing_node)


    return ld
