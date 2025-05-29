#!/usr/bin/env python3
#
# Launch file for robot_master with TurtleBot3 simulation
#
# @date May 2025
#

import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.actions import IncludeLaunchDescription
from launch.conditions import IfCondition
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():
    # 런치 디렉토리 가져오기
    turtlebot3_gazebo_dir = get_package_share_directory("turtlebot3_gazebo")

    # 런치 설정 변수
    use_sim_time = LaunchConfiguration("use_sim_time", default="true")
    sim_mode = LaunchConfiguration("sim_mode", default="true")
    x_pose = LaunchConfiguration("x_pose", default="0.0")
    y_pose = LaunchConfiguration("y_pose", default="0.0")

    # 런치 인자 선언
    declare_use_sim_time_cmd = DeclareLaunchArgument(
        "use_sim_time", default_value="true", description="시뮬레이션 시간 사용 여부"
    )

    declare_sim_mode_cmd = DeclareLaunchArgument(
        "sim_mode", default_value="true", description="시뮬레이션 모드 활성화 여부"
    )

    declare_x_pose_cmd = DeclareLaunchArgument(
        "x_pose", default_value="0.0", description="로봇 초기 X 위치"
    )

    declare_y_pose_cmd = DeclareLaunchArgument(
        "y_pose", default_value="0.0", description="로봇 초기 Y 위치"
    )

    # TurtleBot3 시뮬레이션 런치 파일
    turtlebot3_world_cmd = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(turtlebot3_gazebo_dir, "launch", "empty_world.launch.py")
        ),
        launch_arguments={
            "use_sim_time": use_sim_time,
            "x_pose": x_pose,
            "y_pose": y_pose,
        }.items(),
    )

    # 토픽 브릿지 노드 (시뮬레이션용 TwistStamped를 시뮬레이션 cmd_vel로 리매핑)
    topic_bridge_cmd = Node(
        package="topic_tools",
        executable="relay",
        name="sim_vel_relay",
        output="screen",
        condition=IfCondition(sim_mode),
        parameters=[{"use_sim_time": use_sim_time}],
        arguments=["sim_cmd_vel", "cmd_vel"],
    )

    # robot_master 노드 실행
    robot_master_cmd = Node(
        package="robot_master",
        executable="robot_master_node",
        name="robot_master",
        output="screen",
        parameters=[{"use_sim_time": use_sim_time}, {"sim_mode": sim_mode}],
    )

    # 런치 설명 생성
    ld = LaunchDescription()

    # 모든 명령어를 런치 설명에 추가
    ld.add_action(declare_use_sim_time_cmd)
    ld.add_action(declare_sim_mode_cmd)
    ld.add_action(declare_x_pose_cmd)
    ld.add_action(declare_y_pose_cmd)
    ld.add_action(turtlebot3_world_cmd)
    ld.add_action(topic_bridge_cmd)
    ld.add_action(robot_master_cmd)

    return ld
