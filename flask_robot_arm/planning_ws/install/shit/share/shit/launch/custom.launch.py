from launch import LaunchDescription
from launch_ros.actions import Node
from moveit_configs_utils import MoveItConfigsBuilder
from moveit_configs_utils.launches import generate_demo_launch

def generate_launch_description():
    moveit_config = MoveItConfigsBuilder("myArm3", package_name="shit").to_moveit_configs()
    ld = generate_demo_launch(moveit_config)
    
    # 添加虚拟控制器节点
    fake_controller_node = Node(
        package='shit',
        executable='fake_controller_server.py',
        output='screen'
    )
    ld.add_action(fake_controller_node)
    
    return ld