import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_prefix
from launch.substitutions import LaunchConfiguration
from launch.actions import ExecuteProcess
def generate_launch_description():
    # show_public_param=False
    # sensorOffsetX = 0.0
    # sensorOffsetY = 0.0
    # twoWayDrive = True
    # maxSpeed = 2.0
    # autonomyMode = True
    # autonomySpeed = 2.0
    # joyToSpeedDelay = 2.0
    # 获取软件包路径
    # local_planner_dir = get_package_share_directory('local_planner')
    # local_planner_para_dir = os.path.join(get_package_share_directory('local_planner'), 'config', 'localPlanner.yaml')
    # local_planner_para_dir_1 = os.path.join(get_package_share_directory('local_planner'), 'config', 'pathFollower.yaml')

    # print(local_planner_para_dir)
    # print(LaunchConfiguration('test'))
    ego_planner_node = Node(
        package='ego_planner',
        executable='ego_planner_node',
        output='screen',
        name=LaunchConfiguration('name_of_ego_planner_node'),
        # parameters=[local_planner_para_dir,]
        remappings=[
        # 重映射 position_cmd 话题
            ('odom_world', LaunchConfiguration('odom_world')),
            ('planning/bspline', LaunchConfiguration('planning/bspline')),
            ('planning/data_display', LaunchConfiguration('planning/data_display')),
            ('planning/broadcast_bspline_from_planner', '/broadcast_bspline'),
            ('planning/broadcast_bspline_to_planner', '/broadcast_bspline'),
            ('grid_map/odom', LaunchConfiguration('grid_map/odom')),
            ('grid_map/cloud', LaunchConfiguration('grid_map/cloud')),
            ('grid_map/pose', LaunchConfiguration('grid_map/pose')),
            ('grid_map/depth', LaunchConfiguration('grid_map/depth')),
            ('grid_map/occupancy_inflate', LaunchConfiguration('grid_map/occupancy_inflate')),
            ('optimal_list', LaunchConfiguration('optimal_list')),
            ],
        parameters=[
            # local_planner_para_dir,#9
                    {'fsm/flight_type': LaunchConfiguration('flight_type')},
                    {'fsm/thresh_replan_time': 1.0},
                    {'fsm/thresh_no_replan_meter': 1.0},
                    {'fsm/planning_horizon': LaunchConfiguration('planning_horizon')},
                    {'fsm/planning_horizen_time': 3.0},
                    {'fsm/emergency_time': 1.0},
                    {'fsm/realworld_experiment': False},
                    {'fsm/fail_safe': True},

                    {'fsm/waypoint_num': LaunchConfiguration('point_num')},
                    {'fsm/waypoint0_x': LaunchConfiguration('point0_x')},
                    {'fsm/waypoint0_y': LaunchConfiguration('point0_y')},
                    {'fsm/waypoint0_z': LaunchConfiguration('point0_z')},
                    {'fsm/waypoint1_x': LaunchConfiguration('point1_x')},
                    {'fsm/waypoint1_y': LaunchConfiguration('point1_y')},
                    {'fsm/waypoint1_z': LaunchConfiguration('point1_z')},
                    {'fsm/waypoint2_x': LaunchConfiguration('point2_x')},
                    {'fsm/waypoint2_y': LaunchConfiguration('point2_y')},
                    {'fsm/waypoint2_z': LaunchConfiguration('point2_z')},
                    {'fsm/waypoint3_x': LaunchConfiguration('point3_x')},
                    {'fsm/waypoint3_y': LaunchConfiguration('point3_y')},
                    {'fsm/waypoint3_z': LaunchConfiguration('point3_z')},
                    {'fsm/waypoint4_x': LaunchConfiguration('point4_x')},
                    {'fsm/waypoint4_y': LaunchConfiguration('point4_y')},
                    {'fsm/waypoint4_z': LaunchConfiguration('point4_z')},

                    {'grid_map/resolution': 0.1},
                    {'grid_map/map_size_x': LaunchConfiguration('map_size_x_')},
                    {'grid_map/map_size_y': LaunchConfiguration('map_size_y_')},
                    {'grid_map/map_size_z': LaunchConfiguration('map_size_z_')},
                    {'grid_map/local_update_range_x': 5.5},
                    {'grid_map/local_update_range_y': 5.5},
                    {'grid_map/local_update_range_z': 4.5},
                    {'grid_map/obstacles_inflation': 0.099},
                    {'grid_map/local_map_margin': 10},
                    {'grid_map/ground_height': -0.01},
                    # {'grid_map/local_update_range_y': 5.5}
                    {'grid_map/cx': LaunchConfiguration('cx')},
                    {'grid_map/cy': LaunchConfiguration('cy')},
                    {'grid_map/fx': LaunchConfiguration('fx')},
                    {'grid_map/fy': LaunchConfiguration('fy')},

                    {'grid_map/use_depth_filter': True},
                    {'grid_map/depth_filter_tolerance': 0.15},
                    {'grid_map/depth_filter_maxdist': 5.0},
                    {'grid_map/depth_filter_mindist': 0.2},
                    {'grid_map/depth_filter_margin': 2},
                    {'grid_map/k_depth_scaling_factor': 1000.0},
                    {'grid_map/skip_pixel': 2},

                    {'grid_map/p_hit': 0.65},
                    {'grid_map/p_miss': 0.35},
                    {'grid_map/p_min': 0.12},
                    {'grid_map/p_max': 0.90},
                    {'grid_map/p_occ': 0.80},
                    {'grid_map/min_ray_length': 0.1},
                    {'grid_map/max_ray_length': 4.5},

                    {'grid_map/virtual_ceil_height': 2.9},
                    {'grid_map/visualization_truncate_height': 1.8},
                    {'grid_map/show_occ_time': False},
                    {'grid_map/pose_type': 1},
                    {'grid_map/frame_id': "world"},

                    {'manager/max_vel': LaunchConfiguration('max_vel')},
                    {'manager/max_acc': LaunchConfiguration('max_acc')},
                    {'manager/max_jerk': 4.0},
                    {'manager/control_points_distance': 0.4},
                    {'manager/feasibility_tolerance': 0.05},
                    {'manager/planning_horizon': LaunchConfiguration('planning_horizon')},
                    {'manager/use_distinctive_trajs': LaunchConfiguration('use_distinctive_trajs')},
                    {'manager/drone_id': LaunchConfiguration('drone_id')},

                    {'optimization/lambda_smooth': 1.0},
                    {'optimization/lambda_collision': 0.5},
                    {'optimization/lambda_feasibility': 0.1},
                    {'optimization/lambda_fitness': 1.0},
                    {'optimization/dist0': 0.5},
                    {'optimization/swarm_clearance': 0.5},
                    {'optimization/max_vel': LaunchConfiguration('max_vel')},
                    {'optimization/max_acc': LaunchConfiguration('max_acc')},
                    {'bspline/limit_vel': LaunchConfiguration('max_vel')},
                    {'bspline/limit_acc': LaunchConfiguration('max_acc')},
                    {'bspline/limit_ratio': 1.1},

                    {'prediction/obj_num': LaunchConfiguration('obj_num_set')},
                    {'prediction/lambda': 1.0},
                    {'prediction/predict_rate': 1.0}
                    ]
    )
    # traj_server_node = Node(
    #     package='ego_planner',
    #     executable='traj_server',
    #     output='screen',
    #     name="drone_"+drone_id+"_traj_server",
    #     # parameters=[local_planner_para_dir,]
    #     remappings=[
    #     # 重映射 position_cmd 话题
    #         ('position_cmd', "/drone_"+drone_id+"_planning/pos_cmd"),

    #         # 重映射 planning/bspline 话题
    #         ('planning/bspline', "/drone_"+drone_id+"_planning/bspline"),
    #         ],

    # )
    ld = LaunchDescription()
    ld.add_action(ego_planner_node)


    return ld
