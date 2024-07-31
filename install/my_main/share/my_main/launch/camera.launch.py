from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    return LaunchDescription ([
        Node(
            package='my_main',
            executable='camera',
            name='camera',
            output='screen'
        ),
        Node(
            package='my_main',
            executable='object',
            name='object',
            output='screen'
        ),
        Node(
            package='my_main',
            executable='can',
            name='can',
            output='screen'
        ),
        
    ])
