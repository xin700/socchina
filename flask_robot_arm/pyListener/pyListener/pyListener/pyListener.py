#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from moveit_msgs.msg import DisplayTrajectory
import math
import matplotlib.pyplot as plt
import json
import requests
from . import ZDT
import time
import os
import subprocess

class TrajectoryProcessor(Node):
    def __init__(self):
        super().__init__('trajectory_processor')
        self.subscription = self.create_subscription(
            DisplayTrajectory,
            '/display_planned_path',
            self.trajectory_callback,
            10)
        script_path = os.path.expanduser('~/test/test.sh')
        subprocess.run([script_path],shell=True)
        self.get_logger().info("Trajectory Processor Node has been started.")
        self.arm=ZDT.armController(motorParams=
                       {'offset':    [42,    0,      0,      0,      0,          0   ],
                        'minn':      [2.5,   0,      0,      0,      -90,        -180],
                        'maxx':      [78.5,  150,    180,    335,    90,         180 ],
                        'dir':       [1,     1,      -1,     1,      -1,         1   ],
                        'decRate':   [50,    50,     50,     50,     3.166667,   1   ]
                        })
        initResult=self.arm.initialize()
        if not initResult:
            self.get_logger().error("Motor initialization failed. Please check the connections and parameters.")
            rclpy.shutdown()
            return
        self.get_logger().info("Motor initialized successfully.")
        self.arm.position([0, 0, 45, 0, 0, 0], 2)
        self.get_logger().info("Arm positioned to initial state.")
        # 初始化存储数组
        self.start = [0.0] * 6
        self.end = [0.0] * 6
        self.pos = []
        self.vel = []
        
        subprocess.run([script_path],shell=True)

    def trajectory_callback(self, msg):
        try:
            # 处理起始点
            start_joints = msg.trajectory_start.joint_state
            for i in range(6):
                radian = start_joints.position[i]
                self.start[i] = math.degrees(radian)

            # 处理轨迹
            trajectory = msg.trajectory[0].joint_trajectory
            num_points = len(trajectory.points)
            
            # 处理终点
            last_point = trajectory.points[-1]
            for i in range(6):
                radian = last_point.positions[i]
                self.end[i] = math.degrees(radian)

            # 处理运动序列
            self.pos = []
            self.vel = []
            self.acc = []
            for point in trajectory.points:
                current_pos = []
                current_vel = []
                current_acc = []
                for j in range(6):
                    # 转换位置到度数
                    current_pos.append(math.degrees(point.positions[j]))
                    # 保持速度单位为rad/s（如需转换到deg/s则使用：math.degrees(point.velocities[j]）
                    current_vel.append(math.degrees(point.velocities[j]))
                    current_acc.append(math.degrees(point.accelerations[j]))
                self.pos.append(current_pos)
                self.vel.append(current_vel)
                self.acc.append(current_acc)

            # 调用控制接口
            self.motorControl(self.start, self.end, self.pos, self.vel, self.acc)

        except Exception as e:
            self.get_logger().error(f"Error processing trajectory: {str(e)}")

    def motorControl(self, start, end, pos, vel, acc):
        
        self.get_logger().info(f"Start: {start}, End: {end}")
        timee=math.ceil(len(pos)*0.1)
        timee=max(min(timee, 4),2)
        self.arm.position(end,timee)
        time.sleep(timee)
        script_path = os.path.expanduser('~/test/test.sh')
        subprocess.run([script_path],shell=True)
        print(timee)

def main(args=None):
    rclpy.init(args=args)
    processor = TrajectoryProcessor()
    
    try:
        rclpy.spin(processor)
    except KeyboardInterrupt:
        processor.get_logger().info('Shutting down...')
    finally:
        processor.arm.position([0, 0, 45, 0, 0, 0], 4)
        processor.get_logger().info('returning to initial position...')
        time.sleep(4)
        processor.arm.position([0, 0, 0, 0, 0, 0], 2)
        processor.get_logger().info('Arm returned to initial position.')
        time.sleep(2)
        processor.get_logger().info('Destroying node...')
        processor.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main()