import subprocess
import can
import rclpy
from rclpy.node import Node
from std_msgs.msg import String
from motor_msg.msg import Motor
def generate_can_command(
    control_mode: int,
    direction: int,
    subdivision: int,
    speed: float,
    position: float = None,
    current: int = None
) -> str:
    """
    生成CAN控制命令
    :param control_mode: 控制模式 (1-4)
      1=速度模式, 2=位置模式, 3=力矩模式, 4=绝对角度模式
    :param direction: 转向 (0=逆时针, 1=顺时针)
    :param subdivision: 电机细分值 (2/4/8/16/32)
    :param speed: 转速 (rad/s)
    :param position: 目标角度（度）- 模式2和4时必填
    :param current: 电流值（mA）- 模式3时必填
    :return: CAN数据字符串（14字符长度）
    """
    # 参数有效性验证
    if control_mode not in {1, 2, 3, 4}:
        print("控制模式必须为1-4")
    if direction not in {0, 1}:
        print("转向必须为0或1")
    if subdivision not in {2, 4, 8, 16, 32}:
        print("细分值必须为2/4/8/16/32")
    
    # 模式参数依赖检查
    if control_mode in {2, 4} and position is None:
        print("位置模式和绝对角度模式需要position参数")
    if control_mode == 3 and current is None:
        print("力矩模式需要current参数")

    # 控制模式字节
    mode_byte = control_mode
    
    # 方向字节
    dir_byte = direction
    
    # 细分值字节
    subdiv_byte = subdivision
    
    # 位置/电流数据转换
    pos_bytes = [0x00, 0x00]  # 默认填充00
    if control_mode == 2 or control_mode == 4:
        # 角度放大十倍后转16位整数
        angle = int(position * 10)
        pos_bytes = [(angle >> 8) & 0xFF, angle & 0xFF]
    elif control_mode == 3:
        # 电流值直接转16位整数
        current_val = current
        pos_bytes = [(current_val >> 8) & 0xFF, current_val & 0xFF]

    # 转速处理（放大十倍）
    speed_scaled = int(speed * 10)
    speed_bytes = [(speed_scaled >> 8) & 0xFF, speed_scaled & 0xFF]

    # 构建完整字节序列
    can_bytes = [
        mode_byte,          # 控制模式
        dir_byte,           # 转向
        subdiv_byte,        # 细分值
        pos_bytes[0],       # POS_H/电流高字节
        pos_bytes[1],       # POS_L/电流低字节
        speed_bytes[0],     # SPEED_H
        speed_bytes[1]      # SPEED_L
    ]

    # 转换为十六进制字符串
    return can_bytes

def send_can_message(channel="can0", can_id=0x123, data=[0x01, 0x02, 0x03]):
    try:
        bus = can.interface.Bus(channel=channel, bustype='socketcan')
        msg = can.Message(
            arbitration_id=can_id,
            data=data,
            is_extended_id=False
        )
        bus.send(msg)
        # print(f"Sent to {channel}: ID={hex(can_id)}, Data={bytes(data).hex()}")
    
    except can.CanError as e:
        print(f"CAN error: {e}")
        # raise

class MotorController(Node):
    def __init__(self):
        super().__init__('motor_node')
        self.get_logger().info("步进电机控制节点已启动")
        self.init_can()
        self.bus = can.interface.Bus(channel='can0', bustype='socketcan')
        self.motor_subscriber_ = self.create_subscription(Motor,'motor_control',self.control_callback,10)
    
    def init_can(self):
        subprocess.run(['sudo', 'ip', 'link', 'set', 'down', 'can0'])
        subprocess.run(['sudo', 'ip', 'link', 'set', 'can0', 'type', 'can', 'bitrate', '1000000'])
        subprocess.run(['sudo', 'ip', 'link', 'set', 'up', 'can0'])
        self.get_logger().info("CAN接口已初始化")
    
    def control_callback(self,msg):
        self.get_logger().info(f'控制模式{msg.mode}，转向{msg.dir}，细分值{msg.sub_divide}，速度{msg.speed}，角度{msg.angle}')
        bts=generate_can_command(
            control_mode=msg.mode,
            direction=msg.dir,
            subdivision=msg.sub_divide,
            speed=msg.speed,
            position=msg.angle,
            # current=msg.current
        )
        send_can_message("can0", msg.id, bts)
        self.get_logger().info(f"向0x00{msg.id}发送{bts}")

def main(args=None):
    rclpy.init(args=args)
    motor_controller = MotorController()
    rclpy.spin(motor_controller)
    motor_controller.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()