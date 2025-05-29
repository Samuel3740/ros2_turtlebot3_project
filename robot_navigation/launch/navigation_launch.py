from launch import LaunchDescription
from launch_ros.actions import Node
import os
from ament_index_python.packages import get_package_share_directory
from launch.actions import IncludeLaunchDescription, SetEnvironmentVariable
from launch.launch_description_sources import PythonLaunchDescriptionSource


def generate_launch_description():
    # 패키지 이름을 autonomous_tb3에서 robot_navigation으로 변경
    config_dir = os.path.join(get_package_share_directory("robot_navigation"), "config")
    map_file = os.path.join(config_dir, "map.yaml")
    params_file = os.path.join(config_dir, "tb3_nav_params.yaml")
    waypoints_file = os.path.join(config_dir, "waypoints.yaml")
    rviz_config = os.path.join(config_dir, "tb3_nav.rviz")

    return LaunchDescription(
        [
            # Set the TurtleBot3 model
            SetEnvironmentVariable("TURTLEBOT3_MODEL", "burger"),
            # Bringing our Robot
            IncludeLaunchDescription(
                PythonLaunchDescriptionSource(
                    [
                        get_package_share_directory("turtlebot3_gazebo"),
                        "/launch",
                        "/empty_world.launch.py",
                    ]
                )
            ),
            # Integrating Nav2 Stack
            IncludeLaunchDescription(
                PythonLaunchDescriptionSource(
                    [
                        get_package_share_directory("nav2_bringup"),
                        "/launch",
                        "/bringup_launch.py",
                    ]
                ),
                launch_arguments={"map": map_file, "params_file": params_file}.items(),
            ),
            # Rviz2 bringup
            Node(
                package="rviz2",
                output="screen",
                executable="rviz2",
                name="rviz2_node",
                arguments=["-d", rviz_config],
            ),
            # Waypoint Navigator 노드 실행
            Node(
                package="robot_navigation",
                executable="waypoint_navigator",
                name="waypoint_navigator",
                output="screen",
                parameters=[{"waypoints_file": waypoints_file}],
            ),
        ]
    )
