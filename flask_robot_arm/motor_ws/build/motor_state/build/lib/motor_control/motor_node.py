import subprocess
import can
import rclpy
from rclpy.node import Node
from std_msgs.msg import String
from receive_msg.msg import Receive
def decode_can_data(data_str):
    # 确保数据长度为16个字符（8字节）
    if len(data_str) != 16:
        return -1
    
    # 将字符串分割为字节数组
    bytes_list = [int(data_str[i:i+2], 16) for i in range(0, 16, 2)]
    
    # 解析各字段
    addr = bytes_list[0]                # 地址
    status = bytes_list[1]              # 位置到达状态
    speed_high = bytes_list[2]           # 速度高八位
    speed_low = bytes_list[3]            # 速度低八位
    angle_highest = bytes_list[4]        # 角度高16位的高8位
    angle_high = bytes_list[5]           # 角度高16位的低8位
    angle_low_high = bytes_list[6]       # 角度低16位的高8位
    angle_low_low = bytes_list[7]        # 角度低16位的低8位
    
    # 计算速度（单位：rad/s）
    speed = (speed_high << 8 | speed_low) / 10.0
    
    # 计算角度（单位：度）
    angle_high16 = (angle_highest << 8) | angle_high
    angle_low16 = (angle_low_high << 8) | angle_low_low
    angle = (angle_high16 << 16 | angle_low16) / 10.0
    
    # 生成状态描述
    if status == 1:
        message = f"电机（地址{addr}）已经转动角度达到{angle:.1f}度位置。"
    else:
        message = f"电机（地址{addr}）转速为{speed:.1f}rad/s，转动到了{angle:.1f}度，未到达目标位置。"
    
    return {
        "address": addr,
        "status": status,
        "speed": speed,
        "angle": angle,
        "message": message
    }
def receive_can_messages(channel="can0", timeout=0.2):
    """持续接收 CAN 消息"""
    try:
        bus = can.interface.Bus(channel=channel, bustype='socketcan')
        msg = bus.recv(timeout=timeout)
        if msg:
            package=decode_can_data(msg.data.hex())
            if package != -1:
                return package
    except KeyboardInterrupt:
        print("\nStopped by user")
class StateReader(Node):
    def __init__(self):
        super().__init__('state_node')
        self.get_logger().info("步进电机状态节点已启动")
        self.init_can()
        self.motor_publisher_ = self.create_publisher(Receive,'motor_state',1)
        self.timer = self.create_timer(0.1, self.timer_callback)
    
    def init_can(self):
        subprocess.run(['sudo', 'ip', 'link', 'set', 'down', 'can0'])
        subprocess.run(['sudo', 'ip', 'link', 'set', 'can0', 'type', 'can', 'bitrate', '1000000'])
        subprocess.run(['sudo', 'ip', 'link', 'set', 'up', 'can0'])
        self.get_logger().info("CAN接口已初始化")
    
    def timer_callback(self):
        pkg=receive_can_messages(channel="can0", timeout=0.2)
        if pkg != -1:
            msg = Receive()
            msg.id = pkg['address']
            msg.status = pkg['status']
            msg.speed = pkg['speed']
            msg.angle = pkg['angle']
            self.motor_publisher_.publish(msg)
            self.get_logger().info(f"发布消息: {pkg['message']}")
        

def main(args=None):
    rclpy.init(args=args)
    state_reader = StateReader()
    rclpy.spin(state_reader)
    state_reader.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()