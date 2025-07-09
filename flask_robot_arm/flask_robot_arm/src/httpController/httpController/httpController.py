#!/usr/bin/env python3

import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Pose
from flask import Flask, request, jsonify
import threading
import time
import math
from typing import Tuple, List
# from tf_transformations import quaternion_from_euler
def quaternion_from_euler(roll, pitch, yaw):
    """
    将欧拉角转换为四元数
    替代tf_transformations.quaternion_from_euler
    """
    roll=-roll
    yaw=-yaw
    # 计算半角
    cr = math.cos(roll * 0.5)
    sr = math.sin(roll * 0.5)
    cp = math.cos(pitch * 0.5)
    sp = math.sin(pitch * 0.5)
    cy = math.cos(yaw * 0.5)
    sy = math.sin(yaw * 0.5)

    # 计算四元数
    qw = cr * cp * cy + sr * sp * sy
    qx = sr * cp * cy - cr * sp * sy
    qy = cr * sp * cy + sr * cp * sy
    qz = cr * cp * sy - sr * sp * cy

    return [qx, qy, qz, qw]
class PoseRPY:
    """使用RPY角表示的位姿类"""
    def __init__(self, x=0.0, y=0.0, z=0.0, roll=0.0, pitch=0.0, yaw=0.0):
        self.x = x
        self.y = y
        self.z = z
        self.roll = roll    # 绕X轴旋转（弧度）
        self.pitch = pitch  # 绕Y轴旋转（弧度）
        self.yaw = yaw      # 绕Z轴旋转（弧度）
    
    def to_dict(self):
        """转换为字典格式，角度使用度数"""
        return {
            'position': {'x': self.x, 'y': self.y, 'z': self.z},
            'orientation': {
                'roll': math.degrees(self.roll),
                'pitch': math.degrees(self.pitch), 
                'yaw': math.degrees(self.yaw)
            }
        }
    
    def copy(self):
        """创建副本"""
        return PoseRPY(self.x, self.y, self.z, self.roll, self.pitch, self.yaw)

class MoveItHttpController(Node):
    """
    MoveIt2 HTTP控制器节点
    结合HTTP服务器和ROS2发布器，通过HTTP请求控制机械臂运动
    """
    
    def __init__(self):
        super().__init__('moveit_http_controller')
        
        # 创建位姿发布器
        self.pose_publisher = self.create_publisher(Pose, 'pose', 10)
        
        # 存储当前位姿（使用RPY角）
        self.current_pose = PoseRPY(x=0.0, y=0.0, z=0.3, roll=math.pi, pitch=0.0, yaw=0.0)
        
        # 预定义位姿（使用RPY角）
        self.home_pose = PoseRPY(x=0.2, y=0.0, z=0.25, roll=math.pi, pitch=0.0, yaw=0.0)
        self.stand_pose = PoseRPY(x=0.2, y=0.0, z=0.25, roll=0.0, pitch=0.0, yaw=0.0)
        
        # 初始化Flask应用
        self.app = Flask(__name__)
        self.setup_routes()
        
        self.get_logger().info('MoveIt HTTP Controller initialized')
    
    def rpy_to_ros_pose(self, pose_rpy: PoseRPY) -> Pose:
        """将RPY位姿转换为ROS Pose消息（四元数）"""
        pose_msg = Pose()
        pose_msg.position.x = pose_rpy.x
        pose_msg.position.y = pose_rpy.y
        pose_msg.position.z = pose_rpy.z
        
        # 转换RPY到四元数
        quat = quaternion_from_euler(pose_rpy.roll, pose_rpy.pitch, pose_rpy.yaw)
        pose_msg.orientation.x = quat[0]
        pose_msg.orientation.y = quat[1]
        pose_msg.orientation.z = quat[2]
        pose_msg.orientation.w = quat[3]
        
        return pose_msg
    
    def setup_routes(self):
        """设置HTTP路由"""
        
        @self.app.route('/right', methods=['GET'])
        def move_right():
            """向右移动"""
            try:
                distance = float(request.args.get('dis', 0)) / 100.0  # 转换cm到m
                target_pose = self.right(distance)
                self.publish_pose_rpy(target_pose)
                return jsonify({
                    'status': 'success',
                    'action': 'move_right',
                    'distance_cm': distance * 100,
                    'target_pose': target_pose.to_dict()
                })
            except Exception as e:
                self.get_logger().error(f'Error in move_right: {str(e)}')
                return jsonify({'status': 'error', 'message': str(e)}), 500
        
        @self.app.route('/left', methods=['GET'])
        def move_left():
            """向左移动"""
            try:
                distance = float(request.args.get('dis', 0)) / 100.0  # 转换cm到m
                target_pose = self.left(distance)
                self.publish_pose_rpy(target_pose)
                return jsonify({
                    'status': 'success',
                    'action': 'move_left',
                    'distance_cm': distance * 100,
                    'target_pose': target_pose.to_dict()
                })
            except Exception as e:
                self.get_logger().error(f'Error in move_left: {str(e)}')
                return jsonify({'status': 'error', 'message': str(e)}), 500
        
        @self.app.route('/forward', methods=['GET'])
        def move_forward():
            """向前移动"""
            try:
                distance = float(request.args.get('dis', 0)) / 100.0  # 转换cm到m
                target_pose = self.forward(distance)
                self.publish_pose_rpy(target_pose)
                return jsonify({
                    'status': 'success',
                    'action': 'move_forward',
                    'distance_cm': distance * 100,
                    'target_pose': target_pose.to_dict()
                })
            except Exception as e:
                self.get_logger().error(f'Error in move_forward: {str(e)}')
                return jsonify({'status': 'error', 'message': str(e)}), 500
        
        @self.app.route('/backward', methods=['GET'])
        def move_backward():
            """向后移动"""
            try:
                distance = float(request.args.get('dis', 0)) / 100.0  # 转换cm到m
                target_pose = self.backward(distance)
                self.publish_pose_rpy(target_pose)
                return jsonify({
                    'status': 'success',
                    'action': 'move_backward',
                    'distance_cm': distance * 100,
                    'target_pose': target_pose.to_dict()
                })
            except Exception as e:
                self.get_logger().error(f'Error in move_backward: {str(e)}')
                return jsonify({'status': 'error', 'message': str(e)}), 500
        
        @self.app.route('/up', methods=['GET'])
        def move_up():
            """向上移动"""
            try:
                distance = float(request.args.get('dis', 0)) / 100.0  # 转换cm到m
                target_pose = self.up(distance)
                self.publish_pose_rpy(target_pose)
                return jsonify({
                    'status': 'success',
                    'action': 'move_up',
                    'distance_cm': distance * 100,
                    'target_pose': target_pose.to_dict()
                })
            except Exception as e:
                self.get_logger().error(f'Error in move_up: {str(e)}')
                return jsonify({'status': 'error', 'message': str(e)}), 500
        
        @self.app.route('/down', methods=['GET'])
        def move_down():
            """向下移动"""
            try:
                distance = float(request.args.get('dis', 0)) / 100.0  # 转换cm到m
                target_pose = self.down(distance)
                self.publish_pose_rpy(target_pose)
                return jsonify({
                    'status': 'success',
                    'action': 'move_down',
                    'distance_cm': distance * 100,
                    'target_pose': target_pose.to_dict()
                })
            except Exception as e:
                self.get_logger().error(f'Error in move_down: {str(e)}')
                return jsonify({'status': 'error', 'message': str(e)}), 500
        
        @self.app.route('/greet', methods=['GET'])
        def greet():
            """执行问候动作序列"""
            try:
                action_type = int(request.args.get('type', 1))
                if action_type not in [1, 2, 3]:
                    return jsonify({'status': 'error', 'message': 'Invalid action type. Must be 1, 2, or 3'}), 400
                
                pose_sequence = self.get_greet_sequence(action_type)
                self.execute_pose_sequence(pose_sequence)
                
                return jsonify({
                    'status': 'success',
                    'action': 'greet',
                    'type': action_type,
                    'sequence_length': len(pose_sequence)
                })
            except Exception as e:
                self.get_logger().error(f'Error in greet: {str(e)}')
                return jsonify({'status': 'error', 'message': str(e)}), 500
        
        @self.app.route('/target', methods=['GET'])
        def target():
            """根据目标描述获取位姿并移动"""
            try:
                target_description = request.args.get('desc', '')
                if not target_description:
                    return jsonify({'status': 'error', 'message': 'Missing target description'}), 400
                
                if len(target_description) > 20:
                    return jsonify({'status': 'error', 'message': 'Target description too long (max 20 characters)'}), 400
                
                # 调用getPos函数获取目标位姿
                target_pose = self.getPos(target_description)
                self.publish_pose_rpy(target_pose)
                
                return jsonify({
                    'status': 'success',
                    'action': 'target',
                    'description': target_description,
                    'target_pose': target_pose.to_dict()
                })
            except Exception as e:
                self.get_logger().error(f'Error in target: {str(e)}')
                return jsonify({'status': 'error', 'message': str(e)}), 500
        
        @self.app.route('/grasp', methods=['GET'])
        def grasp():
            """控制夹爪开合"""
            try:
                action = int(request.args.get('action', 0))
                if action not in [0, 1]:
                    return jsonify({'status': 'error', 'message': 'Invalid action. Must be 0 (release) or 1 (grasp)'}), 400
                
                result = self.control_gripper(action)
                action_name = 'grasp' if action == 1 else 'release'
                
                return jsonify({
                    'status': 'success',
                    'action': 'grasp',
                    'gripper_action': action_name,
                    'result': result
                })
            except Exception as e:
                self.get_logger().error(f'Error in grasp: {str(e)}')
                return jsonify({'status': 'error', 'message': str(e)}), 500
        
        @self.app.route('/rotate_roll', methods=['GET'])
        def rotate_roll():
            """绕X轴（Roll）旋转"""
            try:
                angle_deg = float(request.args.get('angle', 0))
                target_pose = self.rotate_end_effector('roll', math.radians(angle_deg))
                self.publish_pose_rpy(target_pose)
                
                return jsonify({
                    'status': 'success',
                    'action': 'rotate_roll',
                    'angle_degrees': angle_deg,
                    'target_pose': target_pose.to_dict()
                })
            except Exception as e:
                self.get_logger().error(f'Error in rotate_roll: {str(e)}')
                return jsonify({'status': 'error', 'message': str(e)}), 500
        
        @self.app.route('/rotate_pitch', methods=['GET'])
        def rotate_pitch():
            """绕Y轴（Pitch）旋转"""
            try:
                angle_deg = float(request.args.get('angle', 0))
                target_pose = self.rotate_end_effector('pitch', math.radians(angle_deg))
                self.publish_pose_rpy(target_pose)
                
                return jsonify({
                    'status': 'success',
                    'action': 'rotate_pitch',
                    'angle_degrees': angle_deg,
                    'target_pose': target_pose.to_dict()
                })
            except Exception as e:
                self.get_logger().error(f'Error in rotate_pitch: {str(e)}')
                return jsonify({'status': 'error', 'message': str(e)}), 500
        
        @self.app.route('/rotate_yaw', methods=['GET'])
        def rotate_yaw():
            """绕Z轴（Yaw）旋转"""
            try:
                angle_deg = float(request.args.get('angle', 0))
                target_pose = self.rotate_end_effector('yaw', math.radians(angle_deg))
                self.publish_pose_rpy(target_pose)
                
                return jsonify({
                    'status': 'success',
                    'action': 'rotate_yaw',
                    'angle_degrees': angle_deg,
                    'target_pose': target_pose.to_dict()
                })
            except Exception as e:
                self.get_logger().error(f'Error in rotate_yaw: {str(e)}')
                return jsonify({'status': 'error', 'message': str(e)}), 500
        
        @self.app.route('/stand', methods=['GET'])
        def stand():
            """移动到站立位姿"""
            try:
                self.publish_pose_rpy(self.stand_pose)
                return jsonify({
                    'status': 'success',
                    'action': 'stand',
                    'target_pose': self.stand_pose.to_dict()
                })
            except Exception as e:
                self.get_logger().error(f'Error in stand: {str(e)}')
                return jsonify({'status': 'error', 'message': str(e)}), 500
        
        @self.app.route('/home', methods=['GET'])
        def home():
            """返回到待机位姿"""
            try:
                self.publish_pose_rpy(self.home_pose)
                return jsonify({
                    'status': 'success',
                    'action': 'home',
                    'target_pose': self.home_pose.to_dict()
                })
            except Exception as e:
                self.get_logger().error(f'Error in home: {str(e)}')
                return jsonify({'status': 'error', 'message': str(e)}), 500
        
        @self.app.route('/status', methods=['GET'])
        def get_status():
            """获取当前状态"""
            return jsonify({
                'status': 'running',
                'current_pose': self.current_pose.to_dict()
            })
        
        @self.app.route('/set_pose', methods=['POST'])
        def set_pose():
            """直接设置位姿（支持RPY角度输入）"""
            try:
                data = request.get_json()
                target_pose = PoseRPY()
                target_pose.x = data['position']['x']
                target_pose.y = data['position']['y']
                target_pose.z = data['position']['z']
                
                # 支持度数输入
                if 'orientation' in data:
                    target_pose.roll = math.radians(data['orientation'].get('roll', 0))
                    target_pose.pitch = math.radians(data['orientation'].get('pitch', 0))
                    target_pose.yaw = math.radians(data['orientation'].get('yaw', 0))
                
                self.publish_pose_rpy(target_pose)
                return jsonify({
                    'status': 'success',
                    'action': 'set_pose',
                    'target_pose': target_pose.to_dict()
                })
            except Exception as e:
                self.get_logger().error(f'Error in set_pose: {str(e)}')
                return jsonify({'status': 'error', 'message': str(e)}), 500
    
    def right(self, distance: float) -> PoseRPY:
        """向右移动指定距离"""
        target_pose = self.current_pose.copy()
        target_pose.y -= distance  # 假设Y轴负方向为右
        return target_pose
    
    def left(self, distance: float) -> PoseRPY:
        """向左移动指定距离"""
        target_pose = self.current_pose.copy()
        target_pose.y += distance  # 假设Y轴正方向为左
        return target_pose
    
    def forward(self, distance: float) -> PoseRPY:
        """向前移动指定距离"""
        target_pose = self.current_pose.copy()
        target_pose.x += distance  # 假设X轴正方向为前
        return target_pose
    
    def backward(self, distance: float) -> PoseRPY:
        """向后移动指定距离"""
        target_pose = self.current_pose.copy()
        target_pose.x -= distance  # 假设X轴负方向为后
        return target_pose
    
    def up(self, distance: float) -> PoseRPY:
        """向上移动指定距离"""
        target_pose = self.current_pose.copy()
        target_pose.z += distance  # Z轴正方向为上
        return target_pose
    
    def down(self, distance: float) -> PoseRPY:
        """向下移动指定距离"""
        target_pose = self.current_pose.copy()
        target_pose.z -= distance  # Z轴负方向为下
        return target_pose
    
    def publish_pose_rpy(self, pose_rpy: PoseRPY):
        """发布RPY位姿到pose话题（转换为四元数）"""
        ros_pose = self.rpy_to_ros_pose(pose_rpy)
        print(ros_pose)
        self.pose_publisher.publish(ros_pose)
        self.current_pose = pose_rpy  # 更新当前位姿
        self.get_logger().info(f'Published pose: x={pose_rpy.x:.3f}, y={pose_rpy.y:.3f}, z={pose_rpy.z:.3f}, '
                              f'roll={math.degrees(pose_rpy.roll):.1f}°, '
                              f'pitch={math.degrees(pose_rpy.pitch):.1f}°, '
                              f'yaw={math.degrees(pose_rpy.yaw):.1f}°')
    
    def get_greet_sequence(self, action_type: int) -> List[PoseRPY]:
        """获取问候动作序列"""
        sequences = {
            1: self._greet_sequence_1(),  # 简单挥手
        }
        return sequences.get(action_type, [])
    
    def _greet_sequence_1(self) -> List[PoseRPY]:
        """问候动作序列1：简单挥手"""
        poses = []
        
        # 起始位置（抬起手臂）

        
        return poses
    
    def _greet_sequence_2(self) -> List[PoseRPY]:
        """问候动作序列2：点头致意"""
        poses = []
        
        # 正常位置
        poses.append(PoseRPY(x=0.25, y=0.0, z=0.3, roll=math.pi, pitch=0.0, yaw=0.0))
        # 前倾（点头）
        poses.append(PoseRPY(x=0.3, y=0.0, z=0.25, roll=math.pi, pitch=0.0, yaw=0.0))
        # 回到正常位置
        poses.append(PoseRPY(x=0.25, y=0.0, z=0.3, roll=math.pi, pitch=0.0, yaw=0.0))
        
        return poses
    
    def _greet_sequence_3(self) -> List[PoseRPY]:
        """问候动作序列3：复杂问候"""
        poses = []
        
        # 组合前两个动作
        poses.extend(self._greet_sequence_1())
        poses.extend(self._greet_sequence_2())
        
        # 添加额外的动作
        poses.append(PoseRPY(x=0.15, y=0.0, z=0.45, roll=0.0, pitch=0.0, yaw=0.0))
        
        return poses
    
    def execute_pose_sequence(self, poses: List[PoseRPY]):
        """执行位姿序列"""
        for i, pose in enumerate(poses):
            self.get_logger().info(f'Executing pose {i+1}/{len(poses)}')
            self.publish_pose_rpy(pose)
            time.sleep(1.0)  # 等待1秒再执行下一个动作
    
    def getPos(self, target_description: str) -> PoseRPY:
        """
        根据目标描述获取位姿（返回RPY格式）
        这里是示例实现，你需要根据实际需求替换
        """
        # 示例：根据不同的描述返回不同的位姿
        target_poses = {
            '苹果': PoseRPY(x=0.3, y=0.1, z=0.2, roll=math.pi, pitch=0.0, yaw=0.0),
            '杯子': PoseRPY(x=0.25, y=-0.1, z=0.15, roll=math.pi, pitch=0.0, yaw=0.0),
            '书本': PoseRPY(x=0.35, y=0.0, z=0.1, roll=math.pi, pitch=0.0, yaw=0.0),
            '钥匙': PoseRPY(x=0.2, y=0.15, z=0.05, roll=math.pi, pitch=0.0, yaw=0.0),
        }
        
        # 默认位姿
        default_pose = PoseRPY(x=0.3, y=0.0, z=0.2, roll=math.pi, pitch=0.0, yaw=0.0)
        
        # 简单的关键词匹配
        for keyword, pose in target_poses.items():
            if keyword in target_description:
                self.get_logger().info(f'Found target "{keyword}" for description "{target_description}"')
                return pose
        
        self.get_logger().info(f'Using default pose for description "{target_description}"')
        return default_pose
    
    def control_gripper(self, action: int) -> str:
        """
        控制夹爪开合
        这里是示例实现，你需要根据实际的夹爪控制接口替换
        """
        if action == 1:
            # 夹住的逻辑
            self.get_logger().info('Gripper: Grasping')
            return 'Gripper closed successfully'
        else:
            # 松开的逻辑
            self.get_logger().info('Gripper: Releasing')
            return 'Gripper opened successfully'
    
    def rotate_end_effector(self, axis: str, angle: float) -> PoseRPY:
        """
        旋转末端执行器（直接操作RPY角）
        axis: 'roll', 'pitch', 'yaw'
        angle: 弧度
        """
        target_pose = self.current_pose.copy()
        
        if axis == 'roll':
            target_pose.roll += angle
        elif axis == 'pitch':
            target_pose.pitch += angle
        elif axis == 'yaw':
            target_pose.yaw += angle
        
        self.get_logger().info(f'Rotating {axis} by {math.degrees(angle):.1f} degrees')
        return target_pose
    
    def run_flask_server(self):
        """运行Flask服务器"""
        @self.app.after_request
        def after_request(response):
            response.headers.add('Access-Control-Allow-Origin', '*')
            response.headers.add('Access-Control-Allow-Headers', 'Content-Type,Authorization')
            response.headers.add('Access-Control-Allow-Methods', 'GET,PUT,POST,DELETE,OPTIONS')
            return response
        self.app.run(host='192.168.0.134', port=1145, debug=True, use_reloader=False)

def main(args=None):
    """主函数"""
    rclpy.init(args=args)
    
    # 创建节点
    controller = MoveItHttpController()
    
    # 在单独的线程中运行Flask服务器
    flask_thread = threading.Thread(target=controller.run_flask_server)
    flask_thread.daemon = True
    flask_thread.start()
    
    controller.get_logger().info('HTTP server started on http://192.168.0.134:1145')
    controller.get_logger().info('Available endpoints:')
    controller.get_logger().info('  Basic Movement:')
    controller.get_logger().info('    GET /right?dis=<distance_cm> - 向右移动')
    controller.get_logger().info('    GET /left?dis=<distance_cm> - 向左移动')
    controller.get_logger().info('    GET /forward?dis=<distance_cm> - 向前移动')
    controller.get_logger().info('    GET /backward?dis=<distance_cm> - 向后移动')
    controller.get_logger().info('    GET /up?dis=<distance_cm> - 向上移动')
    controller.get_logger().info('    GET /down?dis=<distance_cm> - 向下移动')
    controller.get_logger().info('  Advanced Functions:')
    controller.get_logger().info('    GET /greet?type=<1|2|3> - 执行问候动作')
    controller.get_logger().info('    GET /target?desc=<target_description> - 根据目标描述移动')
    controller.get_logger().info('    GET /grasp?action=<0|1> - 控制夹爪 (0=松开, 1=夹住)')
    controller.get_logger().info('  Rotation:')
    controller.get_logger().info('    GET /rotate_roll?angle=<degrees> - 绕X轴旋转')
    controller.get_logger().info('    GET /rotate_pitch?angle=<degrees> - 绕Y轴旋转')
    controller.get_logger().info('    GET /rotate_yaw?angle=<degrees> - 绕Z轴旋转')
    controller.get_logger().info('  Predefined Poses:')
    controller.get_logger().info('    GET /stand - 移动到站立位姿')
    controller.get_logger().info('    GET /home - 返回待机位姿')
    controller.get_logger().info('  Utilities:')
    controller.get_logger().info('    GET /status - 获取当前状态')
    controller.get_logger().info('    POST /set_pose - 直接设置位姿 (支持RPY角度)')
    
    try:
        # 运行ROS2节点
        rclpy.spin(controller)
    except KeyboardInterrupt:
        controller.get_logger().info('Shutting down...')
    finally:
        controller.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main()