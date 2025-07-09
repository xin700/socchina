# #!/usr/bin/env python3
"""
优化的ZDT步进电机控制库
主要优化：
1. 提高通信效率 - 减少不必要的响应等待
2. 简化分包逻辑 - 优化长命令处理
3. 添加状态缓存 - 减少重复查询
4. 改进错误处理 - 添加重试机制
5. 批量操作支持 - 提高多电机控制效率
"""

import time
import struct
import can
import threading
import logging
from typing import Union, List, Tuple, Optional, Dict, Any
from dataclasses import dataclass
from collections import defaultdict
import math

@dataclass
class MotorStatus:
    """电机状态缓存"""
    enabled: bool = False
    position_reached: bool = False
    stalled: bool = False
    stall_protection: bool = False
    last_update: float = 0.0
    
@dataclass
class MotorPosition:
    """电机位置信息"""
    direction: bool = False
    position: float = 0.0
    last_update: float = 0.0

class OptimizedZDTStepperMotor:
    """优化的ZDT步进电机控制类"""

    # 命令功能码
    CMD_ENABLE = 0xF3
    CMD_SPEED_MODE = 0xF6
    CMD_POSITION_MODE = 0xFD
    CMD_STOP = 0xFE
    CMD_SYNC_MOVE = 0xFF
    CMD_READ_VOLTAGE = 0x24
    CMD_READ_CURRENT = 0x27
    CMD_READ_ENCODER = 0x31
    CMD_READ_PULSE_COUNT = 0x32
    CMD_READ_TARGET_POS = 0x33
    CMD_READ_REAL_TARGET_POS = 0x34
    CMD_READ_REAL_SPEED = 0x35
    CMD_READ_REAL_POS = 0x36
    CMD_READ_POS_ERROR = 0x37
    CMD_READ_MOTOR_STATUS = 0x3A
    CMD_READ_ORIGIN_STATUS = 0x3B
    CMD_READ_CONFIG = 0x42
    CMD_READ_SYSTEM_STATUS = 0x43
    CMD_MODIFY_CONFIG = 0x48
    
    # 特殊命令子码
    SUB_ENABLE = 0xAB
    SUB_STOP = 0x98
    SUB_SYNC_MOVE = 0x66
    SUB_MODIFY_CONFIG = 0xD1

    # 默认校验字节
    DEFAULT_CHECKSUM = 0x6B
    
    def __init__(self, can_channel: str = 'can0', bitrate: int = 500000, 
                 motor_id: int = 1, extended_id: bool = True,
                 cache_timeout: float = 0.1, max_retries: int = 3):
        """
        初始化优化的ZDT步进电机控制器
        
        Args:
            can_channel: CAN接口名称
            bitrate: CAN波特率
            motor_id: 电机ID地址
            extended_id: 使用扩展CAN ID
            cache_timeout: 状态缓存超时时间(秒)
            max_retries: 最大重试次数
        """
        self.motor_id = motor_id
        self.extended_id = extended_id
        self.cache_timeout = cache_timeout
        self.max_retries = max_retries
        
        # 状态缓存
        self.status_cache = MotorStatus()
        self.position_cache = MotorPosition()
        
        # 通信优化
        self.last_command_time = 0.0
        self.min_command_interval = 0.001  # 最小命令间隔(ms)
        
        # 线程锁
        self.can_lock = threading.Lock()
        
        # 初始化CAN总线
        try:
            self.can_bus = can.interface.Bus(
                channel=can_channel, 
                bustype='socketcan', 
                bitrate=bitrate
            )
            logging.info(f"Connected to {can_channel} at {bitrate} bps for motor {motor_id}")
        except Exception as e:
            logging.error(f"Failed to initialize CAN bus: {e}")
            raise
        
        # 等待总线初始化
        time.sleep(0.05)
        
    def close(self):
        """关闭CAN总线连接"""
        if hasattr(self, 'can_bus'):
            self.can_bus.shutdown()
            
    def _wait_command_interval(self):
        """等待命令间隔以避免总线拥塞"""
        current_time = time.time()
        elapsed = current_time - self.last_command_time
        if elapsed < self.min_command_interval:
            time.sleep(self.min_command_interval - elapsed)
        self.last_command_time = time.time()
    
    def _send_command(self, cmd_data: List[int], expect_response: bool = True, 
                      timeout: float = 0.5, retries: int = None) -> Union[List[int], None]:
        """
        发送命令到电机（优化版本）
        
        Args:
            cmd_data: 命令字节列表
            expect_response: 是否等待响应
            timeout: 响应超时时间
            retries: 重试次数（None使用默认值）
            
        Returns:
            响应数据或None
        """
        if retries is None:
            retries = self.max_retries
        
        with self.can_lock:
            self._wait_command_interval()
            
            for attempt in range(retries + 1):
                try:
                    success = self._send_command_once(cmd_data)
                    if not success:
                        if attempt < retries:
                            time.sleep(0.01)  # 短暂等待后重试
                            continue
                        else:
                            return None
                    
                    if not expect_response:
                        return []  # 返回空列表表示成功但无响应
                    
                    # 等待响应
                    response = self._wait_for_response(timeout)
                    if response is not None:
                        return response
                    
                    if attempt < retries:
                        time.sleep(0.01)
                        continue
                        
                except Exception as e:
                    logging.warning(f"Command attempt {attempt + 1} failed: {e}")
                    if attempt < retries:
                        time.sleep(0.01)
                        continue
                    else:
                        logging.error(f"All {retries + 1} attempts failed for motor {self.motor_id}")
                        return None
            
            return None
    
    def _send_command_once(self, cmd_data: List[int]) -> bool:
        """发送单次命令"""
        try:
            if len(cmd_data) <= 8:
                # 短命令，单包发送
                arb_id = (self.motor_id << 8) | 0x00
                msg = can.Message(
                    arbitration_id=arb_id,
                    data=cmd_data,
                    is_extended_id=self.extended_id
                )
                self.can_bus.send(msg, timeout=0.1)
            else:
                # 长命令，分包发送（优化版本）
                self._send_long_command(cmd_data)
            
            return True
            
        except Exception as e:
            logging.warning(f"Send command failed: {e}")
            return False
    
    def _send_long_command(self, cmd_data: List[int]):
        """优化的长命令分包发送"""
        packet_num = 0
        remaining_data = cmd_data[:]
        
        while remaining_data:
            if packet_num == 0:
                # 第一包：取前8字节
                packet_data = remaining_data[:8]
                remaining_data = remaining_data[8:]
            else:
                # 后续包：第一个字节是功能码，然后是7字节数据
                func_code = cmd_data[0]
                packet_data = [func_code] + remaining_data[:7]
                remaining_data = remaining_data[7:]
            
            arb_id = (self.motor_id << 8) | packet_num
            msg = can.Message(
                arbitration_id=arb_id,
                data=packet_data,
                is_extended_id=self.extended_id
            )
            
            self.can_bus.send(msg, timeout=0.1)
            packet_num += 1
            
            if remaining_data:
                time.sleep(0.002)  # 减少包间延迟
    
    def _wait_for_response(self, timeout: float) -> Union[List[int], None]:
        """等待响应（修正版本）"""
        response_data = []
        start_time = time.time()
        expected_packet = 0
        
        while time.time() - start_time < timeout:
            try:
                response = self.can_bus.recv(timeout=0.05)
                if not response:
                    continue
                    
                # 从arbitration_id中提取motor_id和包号
                response_motor_id = response.arbitration_id >> 8
                packet_num = response.arbitration_id & 0xFF
                
                # 检查是否是当前电机的响应
                if response_motor_id != self.motor_id:
                    continue
                
                if packet_num == expected_packet:
                    # 直接添加数据，不需要去除motor_id（因为数据中本来就没有motor_id）
                    response_data.extend(response.data)
                    expected_packet += 1
                    
                    # 检查是否是最后一包（数据长度小于8）
                    if len(response.data) < 8:
                        break
                else:
                    # 包序号不匹配，可能是丢包
                    logging.warning(f"Packet sequence mismatch: expected {expected_packet}, got {packet_num}")
                    return None
                    
            except Exception as e:
                logging.warning(f"Response receive error: {e}")
                return None
        
        return response_data if response_data else None
    
    # ==================== 控制命令（优化版本） ====================
    
    def enable(self, enable_state: bool = True, sync_flag: bool = False) -> bool:
        """使能或禁用电机（优化版本）"""
        cmd = [self.CMD_ENABLE, self.SUB_ENABLE, 
               0x01 if enable_state else 0x00, 
               0x01 if sync_flag else 0x00, 
               self.DEFAULT_CHECKSUM]
        
        response = self._send_command(cmd, expect_response=True, timeout=0.2)
        success = response and len(response) >= 3 and response[1] == 0x02
        
        if success:
            # 更新状态缓存
            self.status_cache.enabled = enable_state
            self.status_cache.last_update = time.time()
            
        return success
    
    def speed_mode(self, direction: bool = False, speed: int = 0, 
                   acceleration: int = 0, sync_flag: bool = False) -> bool:
        """速度模式控制（优化版本）"""
        speed_high = (speed >> 8) & 0xFF
        speed_low = speed & 0xFF
        
        cmd = [self.CMD_SPEED_MODE, 
               0x01 if direction else 0x00, 
               speed_high, speed_low, 
               acceleration, 
               0x01 if sync_flag else 0x00,
               self.DEFAULT_CHECKSUM]
        
        # 速度命令通常不需要等待响应，提高效率
        expect_resp = True  # 同步模式才需要响应
        response = self._send_command(cmd, expect_response=expect_resp, timeout=0.1)
        print(self.motor_id,response)
        if expect_resp:
            return response and len(response) >= 3 and response[2] == 0x02
        else:
            return response is not None  # 发送成功即可
    
    def position_mode(self, direction: bool = False, speed: int = 0, 
                     acceleration: int = 0, pulse_count: int = 0, 
                     is_absolute: bool = False, sync_flag: bool = False) -> bool:
        """位置模式控制（优化版本）"""
        speed_high = (speed >> 8) & 0xFF
        speed_low = speed & 0xFF
        
        pulse_count_bytes = [
            (pulse_count >> 24) & 0xFF,
            (pulse_count >> 16) & 0xFF,
            (pulse_count >> 8) & 0xFF,
            pulse_count & 0xFF
        ]
        
        cmd = [self.CMD_POSITION_MODE, 
               0x01 if direction else 0x00, 
               speed_high, speed_low, 
               acceleration] + pulse_count_bytes + [
               0x01 if is_absolute else 0x00,
               0x01 if sync_flag else 0x00,
               self.DEFAULT_CHECKSUM]
               
        response = self._send_command(cmd, expect_response=True, timeout=0.2)
        print(response)
        return response and len(response) >= 3 and response[1] == 0x02
    
    def move_angle(self, angle: float, direction: bool = None, speed: int = 300, 
                  acceleration: int = 5, is_absolute: bool = False, 
                  sync_flag: bool = False, microsteps: int = 16) -> bool:
        """角度移动（优化版本）"""
        if direction is None:
            if angle < 0:
                direction = True
                angle = abs(angle)
            else:
                direction = False
                
        pulse_count = self._angle_to_pulse(angle, microsteps)
        
        return self.position_mode(
            direction=direction,
            speed=speed,
            acceleration=acceleration,
            pulse_count=pulse_count,
            is_absolute=is_absolute,
            sync_flag=sync_flag
        )
    
    def stop(self, sync_flag: bool = False) -> bool:
        """停止电机（优化版本）"""
        cmd = [self.CMD_STOP, self.SUB_STOP, 
               0x01 if sync_flag else 0x00, 
               self.DEFAULT_CHECKSUM]
               
        response = self._send_command(cmd, expect_response=False, timeout=0.1)
        return response is not None
    
    def sync_move(self) -> bool:
        """同步运动（优化版本）"""
        cmd = [self.CMD_SYNC_MOVE, self.SUB_SYNC_MOVE, 
               self.DEFAULT_CHECKSUM]
               
        response = self._send_command(cmd, expect_response=True, timeout=0.2)
        return response and len(response) >= 3 and response[2] == 0x02
    
    # ==================== 读取命令（带缓存优化） ====================
    
    def read_motor_status(self, use_cache: bool = True) -> dict:
        """读取电机状态（带缓存）"""
        current_time = time.time()
        
        if (use_cache and 
            current_time - self.status_cache.last_update < self.cache_timeout):
            return {
                'enabled': self.status_cache.enabled,
                'position_reached': self.status_cache.position_reached,
                'stalled': self.status_cache.stalled,
                'stall_protection': self.status_cache.stall_protection
            }
        
        cmd = [self.CMD_READ_MOTOR_STATUS, self.DEFAULT_CHECKSUM]
        response = self._send_command(cmd, timeout=0.3)
        
        if response and len(response) >= 3:
            status_byte = response[1]
            status = {
                'enabled': bool(status_byte & 0x01),
                'position_reached': bool(status_byte & 0x02),
                'stalled': bool(status_byte & 0x04),
                'stall_protection': bool(status_byte & 0x08)
            }
            
            # 更新缓存
            self.status_cache.enabled = status['enabled']
            self.status_cache.position_reached = status['position_reached']
            self.status_cache.stalled = status['stalled']
            self.status_cache.stall_protection = status['stall_protection']
            self.status_cache.last_update = current_time
            
            return status
        return {}
    
    def read_real_position(self, use_cache: bool = True, debug: bool = False) -> Tuple[bool, float]:
        """读取实时位置（修正版本，带调试选项）"""
        current_time = time.time()
        
        if (use_cache and 
            current_time - self.position_cache.last_update < self.cache_timeout):
            return (self.position_cache.direction, self.position_cache.position)
        cmd = [self.CMD_READ_REAL_POS, self.DEFAULT_CHECKSUM]
        response = self._send_command(cmd, timeout=0.1)
        print(self.motor_id,response)
        if debug and response:
            hex_response = ' '.join([f'{b:02X}' for b in response])
            logging.info(f"Motor {self.motor_id} position response: {hex_response}")
        
        if response and len(response) >= 7:
            # 数据格式：功能码(36) + 方向(01/00) + 位置数据(4字节) + 校验(6B)
            func_code = response[0]  # 应该是0x36
            direction = response[1] == 0x01
            position_raw = (response[2] << 24) | (response[3] << 16) | (response[4] << 8) | response[5]
            checksum = response[6]  # 应该是0x6B
            # print(response,position_raw)
            if debug:
                logging.info(f"Motor {self.motor_id}: func=0x{func_code:02X}, dir={'CCW' if direction else 'CW'}, "
                           f"raw_pos={position_raw}, checksum=0x{checksum:02X}")
            
            position_degrees = (position_raw * 360.0) / 65536.0
            
            # 更新缓存
            self.position_cache.direction = direction
            self.position_cache.position = position_degrees
            self.position_cache.last_update = current_time
            
            return (direction, position_degrees)
        
        if debug:
            logging.warning(f"Motor {self.motor_id} position read failed: response length = {len(response) if response else 0}")
        
        return (False, -1)
    
    def read_real_speed(self) -> Tuple[bool, int]:
        """读取实时速度"""
        cmd = [self.CMD_READ_REAL_SPEED, self.DEFAULT_CHECKSUM]
        response = self._send_command(cmd, timeout=0.1)
        
        if response and len(response) >= 5:
            direction = response[1] == 0x01
            speed = (response[2] << 8) | response[3]
            return (direction, speed)
        return (False, -1)
    
    def read_bus_voltage(self) -> int:
        """读取总线电压"""
        cmd = [self.CMD_READ_VOLTAGE, self.DEFAULT_CHECKSUM]
        response = self._send_command(cmd, timeout=0.3)
        
        if response and len(response) >= 4:
            voltage = (response[2] << 8) | response[3]
            return voltage
        return -1
    
    def read_phase_current(self) -> int:
        """读取相电流"""
        cmd = [self.CMD_READ_CURRENT, self.DEFAULT_CHECKSUM]
        response = self._send_command(cmd, timeout=0.3)
        
        if response and len(response) >= 4:
            current = (response[1] << 8) | response[2]
            return current
        return -1
    
    # ==================== 回零操作（保持原有功能） ====================
    
    def trigger_homing(self, mode: int, sync_flag: bool = False) -> bool:
        """触发回零操作"""
        cmd = [0x9A, mode, 
               0x01 if sync_flag else 0x00, 
               self.DEFAULT_CHECKSUM]
            
        response = self._send_command(cmd, timeout=0.5)
        return response and len(response) >= 3 and response[1] == 0x02

    def read_origin_status(self) -> dict:
        """读取回零状态标志位"""
        cmd = [0x3B, self.DEFAULT_CHECKSUM]
        response = self._send_command(cmd, timeout=0.3)
        
        if response and len(response) >= 3:
            status_byte = response[1]
            return {
                'encoder_ready': bool(status_byte & 0x01),
                'calibration_table_ready': bool(status_byte & 0x02),
                'homing_in_progress': bool(status_byte & 0x04),
                'homing_failed': bool(status_byte & 0x08)
            }
        return {}

    def modify_origin_parameters(self, params: dict, store: bool = True) -> bool:
        """修改原点回零参数"""
        mode = params.get('mode', 0)
        direction = params.get('direction', 0)
        speed = params.get('speed', 30)
        timeout = params.get('timeout', 10000)
        collision_speed = params.get('collision_speed', 300)
        collision_current = params.get('collision_current', 800)
        collision_time = params.get('collision_time', 60)
        auto_home = params.get('auto_home_on_power', 0)
        
        param_bytes = [
            mode, direction,
            (speed >> 8) & 0xFF, speed & 0xFF,
            (timeout >> 24) & 0xFF, (timeout >> 16) & 0xFF, 
            (timeout >> 8) & 0xFF, timeout & 0xFF,
            (collision_speed >> 8) & 0xFF, collision_speed & 0xFF,
            (collision_current >> 8) & 0xFF, collision_current & 0xFF,
            (collision_time >> 8) & 0xFF, collision_time & 0xFF,
            auto_home
        ]
        
        cmd = [0x4C, 0xAE, 0x01 if store else 0x00] + param_bytes + [self.DEFAULT_CHECKSUM]
        response = self._send_command(cmd, timeout=1.0)
        return response and len(response) >= 3 and response[1] == 0x02

    def abort_homing(self) -> bool:
        """强制中断并退出回零操作"""
        cmd = [0x9C, 0x48, self.DEFAULT_CHECKSUM]
        response = self._send_command(cmd, timeout=0.5)
        return response and len(response) >= 3 and response[2] == 0x02

    def home_collision(self, direction: bool = False, 
                      speed: int = 100, current: int = 800, 
                      detect_time: int = 10, wait_timeout: float = 30.0) -> bool:
        """执行多圈无限位碰撞回零（优化版本）"""
        params = {
            'mode': 2,
            'direction': 1 if direction else 0,
            'speed': speed,
            'collision_speed': 300,
            'collision_current': current,
            'collision_time': detect_time
        }
        
        if not self.modify_origin_parameters(params):
            return False
            
        if not self.trigger_homing(mode=2):
            return False
            
        # 优化的等待逻辑
        start_time = time.time()
        check_interval = 0.2
        last_check = 0
        
        while time.time() - start_time < wait_timeout:
            current_time = time.time()
            
            if current_time - last_check >= check_interval:
                status = self.read_origin_status()
                print(status)
                if not status:
                    return False
                    
                if not status.get('homing_in_progress', False):
                    return not status.get('homing_failed', True)
                    
                last_check = current_time
                
            time.sleep(0.1)  # 减少CPU占用
            
        # 超时，中断回零
        self.abort_homing()
        return False
    
    # ==================== 辅助方法 ====================
    
    def _angle_to_pulse(self, angle: float, microsteps: int = 16) -> int:
        """将角度转换为脉冲数"""
        steps_per_rev = 200  # 1.8°电机
        pulses_per_rev = steps_per_rev * microsteps
        pulse_count = int((angle / 360.0) * pulses_per_rev)
        return pulse_count
    
    def get_cached_status(self) -> dict:
        """获取缓存的状态信息"""
        return {
            'motor_id': self.motor_id,
            'status_cache_age': time.time() - self.status_cache.last_update,
            'position_cache_age': time.time() - self.position_cache.last_update,
            'enabled': self.status_cache.enabled,
            'position': self.position_cache.position,
            'direction': self.position_cache.direction
        }
    
    def clear_cache(self):
        """清除状态缓存"""
        self.status_cache.last_update = 0.0
        self.position_cache.last_update = 0.0
    
    def set_cache_timeout(self, timeout: float):
        """设置缓存超时时间"""
        self.cache_timeout = timeout


class BatchMotorController:
    """
    批量电机控制器（用于提高多电机操作效率）
    
    该类提供了对多个电机进行批量操作的接口，大幅提高了多电机系统的控制效率。
    特别适用于机械臂等需要同时控制多个电机的应用场景。
    
    主要功能：
    - 批量使能/禁用电机
    - 批量速度控制
    - 批量读取位置、速度、状态
    - 同步运动控制
    
    使用示例：
    ```python
    # 创建多个电机
    motors = [OptimizedZDTStepperMotor(motor_id=i) for i in range(1, 7)]
    batch = BatchMotorController(motors)
    
    # 批量操作
    batch.batch_enable([1,2,3,4,5,6], True)
    speeds = batch.batch_read_speeds()
    positions = batch.batch_read_positions(use_cache=True)
    ```
    """
    
    def __init__(self, motors: List[OptimizedZDTStepperMotor]):
        self.motors = motors
        self.motor_dict = {motor.motor_id: motor for motor in motors}
    
    def batch_enable(self, motor_ids: List[int] = None, enable_state: bool = True) -> Dict[int, bool]:
        """批量使能电机"""
        if motor_ids is None:
            motor_ids = list(self.motor_dict.keys())
        
        results = {}
        for motor_id in motor_ids:
            if motor_id in self.motor_dict:
                results[motor_id] = self.motor_dict[motor_id].enable(enable_state)
            else:
                results[motor_id] = False
        
        return results
    
    def get_all_motor_info(self, motor_ids: List[int] = None, use_cache: bool = True) -> Dict[int, dict]:
        """
        获取所有电机的综合信息（位置、速度、状态）
        
        Args:
            motor_ids: 要查询的电机ID列表，None表示所有电机
            use_cache: 是否使用缓存（仅适用于位置和状态）
        
        Returns:
            字典: {motor_id: {'position': (direction, angle), 
                            'speed': (direction, rpm),
                            'status': {...}}}
        """
        if motor_ids is None:
            motor_ids = list(self.motor_dict.keys())
        
        # 批量读取所有信息
        positions = self.batch_read_positions(motor_ids, use_cache)
        speeds = self.batch_read_speeds(motor_ids)
        statuses = self.batch_read_status(motor_ids, use_cache)
        
        # 组合结果
        results = {}
        for motor_id in motor_ids:
            results[motor_id] = {
                'position': positions.get(motor_id, (False, -1)),
                'speed': speeds.get(motor_id, (False, -1)),
                'status': statuses.get(motor_id, {}),
                'is_moving': speeds.get(motor_id, (False, -1))[1] > 0,
                'position_degrees': positions.get(motor_id, (False, -1))[1]
            }
        
        return results
    
    def batch_speed_control(self, speed_commands: Dict[int, dict]) -> Dict[int, bool]:
        """
        批量速度控制
        speed_commands格式: {motor_id: {'direction': bool, 'speed': int, 'acceleration': int}}
        """
        results = {}
        for motor_id, cmd in speed_commands.items():
            print(motor_id)
            if motor_id in self.motor_dict:
                motor = self.motor_dict[motor_id]
                results[motor_id] = motor.speed_mode(
                    direction=cmd.get('direction', False),
                    speed=cmd.get('speed', 0),
                    acceleration=cmd.get('acceleration', 0)
                )
            else:
                results[motor_id] = False
        
        return results
    
    def batch_pos_control(self, pos_commands: Dict[int, dict]) -> Dict[int, bool]:
        """
        批量位置控制
        pos_commands格式: {motor_id: {'position': float, 'speed': int, 'acceleration': int}}
        """
        results = {}
        for motor_id, cmd in pos_commands.items():
            print(motor_id)
            if motor_id in self.motor_dict:
                motor = self.motor_dict[motor_id]
                results[motor_id] = motor.move_angle(
                    angle=cmd.get('position', 0.0),
                    direction=cmd.get('direction', None),
                    speed=cmd.get('speed', 300),
                    acceleration=cmd.get('acceleration', 5),
                    is_absolute=False,  # 默认绝对位置
                    sync_flag=False  # 默认不同步
                )
            else:
                results[motor_id] = False
        
        return results
    
    def batch_read_positions(self, motor_ids: List[int] = None, use_cache: bool = True) -> Dict[int, Tuple[bool, float]]:
        """批量读取位置"""
        if motor_ids is None:
            motor_ids = list(self.motor_dict.keys())
        
        results = {}
        for motor_id in motor_ids:
            if motor_id in self.motor_dict:
                results[motor_id] = self.motor_dict[motor_id].read_real_position(use_cache)
            else:
                results[motor_id] = (False, -1)
        
        return results
    
    def batch_read_status(self, motor_ids: List[int] = None, use_cache: bool = True) -> Dict[int, dict]:
        """批量读取状态"""
        if motor_ids is None:
            motor_ids = list(self.motor_dict.keys())
        
        results = {}
        for motor_id in motor_ids:
            if motor_id in self.motor_dict:
                results[motor_id] = self.motor_dict[motor_id].read_motor_status(use_cache)
            else:
                results[motor_id] = {}
        
        return results
    
    def batch_read_speeds(self, motor_ids: List[int] = None) -> Dict[int, Tuple[bool, int]]:
        """
        批量读取速度
        
        Args:
            motor_ids: 要读取的电机ID列表，None表示所有电机
        
        Returns:
            字典: {motor_id: (direction, speed)}
            direction: False=CW, True=CCW
            speed: 速度值(RPM)，-1表示读取失败
        """
        if motor_ids is None:
            motor_ids = list(self.motor_dict.keys())
        
        results = {}
        for motor_id in motor_ids:
            if motor_id in self.motor_dict:
                results[motor_id] = self.motor_dict[motor_id].read_real_speed()
            else:
                results[motor_id] = (False, -1)
        
        return results
    
    def sync_move_all(self) -> bool:
        """所有电机同步运动"""
        # 发送同步运动命令到所有电机
        success = True
        for motor in self.motors:
            if not motor.sync_move():
                success = False
        
        return success

class armController:
    def __init__(self, motorParams: List[dict]):
        self.motor1 = OptimizedZDTStepperMotor(motor_id=1, cache_timeout=0.1)
        self.motor2 = OptimizedZDTStepperMotor(motor_id=2, cache_timeout=0.1)
        self.motor3 = OptimizedZDTStepperMotor(motor_id=3, cache_timeout=0.1)
        self.motor4 = OptimizedZDTStepperMotor(motor_id=4, cache_timeout=0.1)
        self.motor5 = OptimizedZDTStepperMotor(motor_id=5, cache_timeout=0.1)
        self.motor6 = OptimizedZDTStepperMotor(motor_id=6, cache_timeout=0.1)
        self.motorParams = motorParams
        self.controller = BatchMotorController([self.motor1, self.motor2, self.motor3,
                                                self.motor4, self.motor5, self.motor6])
        self.enable_results={}
        self.homeResult=False
    def initialize(self):
        self.enable_results=self.controller.batch_enable([1, 2, 3, 4, 5, 6], True)
        print(f"Enable results: {self.enable_results}")
        self.homeResult=self.motor1.home_collision(direction=False,
                speed=90,
                current=350,
                detect_time=10
            )
        print(f"Home collision result: {self.homeResult}")
        return not (False in self.enable_results.values()) or not self.homeResult
    
    def calculate_motor_params(self, angle_degrees, total_time_seconds, use_acceleration=True, max_rpm=3000, min_acc_level=1):
        PULSES_PER_REVOLUTION = 3200  # 16细分下一圈的脉冲数
        DEGREES_PER_REVOLUTION = 360   # 一圈的角度
        
        # 计算脉冲数
        pulses = int(angle_degrees * PULSES_PER_REVOLUTION / DEGREES_PER_REVOLUTION)
        
        # 如果用户选择不使用加速度或者时间很短/角度很小，使用恒速模式
        if not use_acceleration:
            # 计算所需的平均速度（RPM）
            avg_speed_rps = abs(pulses) / (total_time_seconds * PULSES_PER_REVOLUTION)  # 转/秒
            required_rpm = avg_speed_rps * 60  # 转换为RPM
            
            if required_rpm > max_rpm:
                return {
                    'error': f'所需速度 {required_rpm:.1f} RPM 超过最大限制 {max_rpm} RPM',
                    'pulses': pulses,
                    'angle_degrees': angle_degrees,
                    'use_acceleration': use_acceleration
                }
            
            return {
                'speed_rpm': math.ceil(required_rpm),
                'acceleration_level': 0,  # 不使用加减速
                'pulses': pulses,
                'angle_degrees': angle_degrees,
                'total_time': total_time_seconds,
                'motion_profile': 'constant_speed',
                'use_acceleration': use_acceleration
            }
        
        # 使用梯形速度曲线
        best_solution = None
        best_error = float('inf')
        
        # 尝试不同的加速度档位
        for acc_level in range(min_acc_level, 256):
            # 计算单位加速时间（从0到1RPM所需时间）
            unit_acc_time = (256 - acc_level) * 50e-6  # 秒
            
            # 尝试不同的最大速度
            for max_speed in range(10, max_rpm + 1, 10):
                # 计算加速和减速时间
                acc_time = max_speed * unit_acc_time
                dec_time = acc_time  # 假设加减速时间相等
                
                # 如果加减速时间太长，跳过
                if acc_time + dec_time >= total_time_seconds:
                    continue
                
                # 计算匀速阶段时间
                const_time = total_time_seconds - acc_time - dec_time
                
                # 计算总脉冲数（梯形面积）
                # 加速阶段脉冲 = 0.5 * max_speed * acc_time * (PULSES_PER_REVOLUTION/60)
                # 匀速阶段脉冲 = max_speed * const_time * (PULSES_PER_REVOLUTION/60)
                # 减速阶段脉冲 = 0.5 * max_speed * dec_time * (PULSES_PER_REVOLUTION/60)
                
                pulses_per_second_at_max_speed = max_speed * PULSES_PER_REVOLUTION / 60
                
                acc_pulses = 0.5 * acc_time * pulses_per_second_at_max_speed
                const_pulses = const_time * pulses_per_second_at_max_speed
                dec_pulses = 0.5 * dec_time * pulses_per_second_at_max_speed
                
                calculated_pulses = acc_pulses + const_pulses + dec_pulses
                
                # 计算误差
                error = abs(calculated_pulses - abs(pulses))
                
                if error < best_error:
                    best_error = error
                    best_solution = {
                        'speed_rpm': max_speed,
                        'acceleration_level': acc_level,
                        'pulses': pulses,
                        'calculated_pulses': int(calculated_pulses),
                        'angle_degrees': angle_degrees,
                        'total_time': total_time_seconds,
                        'acc_time': acc_time,
                        'const_time': const_time,
                        'dec_time': dec_time,
                        'motion_profile': 'trapezoidal',
                        'error_pulses': error,
                        'error_percentage': (error / abs(pulses)) * 100 if pulses != 0 else 0
                    }
                    
                    # 如果误差很小，就接受这个解
                    if error < abs(pulses) * 0.01:  # 1%误差以内
                        break
            
            if best_solution and best_solution['error_percentage'] < 1:
                break
        
        if best_solution is None:
            # 如果找不到合适的解，返回一个近似解
            avg_speed_rps = abs(pulses) / (total_time_seconds * PULSES_PER_REVOLUTION)
            required_rpm = min(avg_speed_rps * 60, max_rpm)
            
            return {
                'speed_rpm': int(required_rpm),
                'acceleration_level': 50,  # 中等加速度
                'pulses': pulses,
                'angle_degrees': angle_degrees,
                'total_time': total_time_seconds,
                'motion_profile': 'approximate',
                'warning': '无法找到精确解，返回近似参数'
            }
        
        return best_solution

    def move_to_angle(self, target_angle, move_time):
        for i in range(6):
            if not (self.motorParams['minn'][i] <= target_angle[i] <= self.motorParams['maxx'][i]):
                print(f"电机{i+1}超出允许范围{self.motorParams['minn'][i]}-{self.motorParams['maxx'][i]}")
                return False
        currentPos=self.controller.batch_read_positions(use_cache=True)
        # print(currentPos)
        angle_diff=[0]*6
        for i in range(6):
            if i<=3:
                angle_diff[i]= currentPos[i+1][1] - target_angle[i] * self.motorParams['decRate'][i]
            elif i==4:
                angle_diff[i]= (1 if (currentPos[i+1][0]==False) else -1) * currentPos[i+1][1] - target_angle[i] * self.motorParams['decRate'][i]
            elif i==5:
                angle_diff[i]= (-1 if (currentPos[i+1][0]==False) else 1) * currentPos[i+1][1] - target_angle[i] * self.motorParams['decRate'][i]
        solution=[]
        for i in range(6):
            if i<=3 or angle_diff[i]<=90:
                solution.append(self.calculate_motor_params(angle_diff[i], move_time, True, 1500, 1))
            else:
                solution.append(self.calculate_motor_params(angle_diff[i], move_time, False, 1500))
            if 'error' in solution[i]:
                print(f"电机{i+1}参数求解错误")
                return False
        self.controller.batch_pos_control({
            1: {'position': self.motorParams['dir'][0]*angle_diff[0], 'speed': solution[0]['speed_rpm'], 'acceleration': solution[0]['acceleration_level']},
            2: {'position': self.motorParams['dir'][1]*angle_diff[1], 'speed': solution[1]['speed_rpm'], 'acceleration': solution[1]['acceleration_level']},
            3: {'position': self.motorParams['dir'][2]*angle_diff[2], 'speed': solution[2]['speed_rpm'], 'acceleration': solution[2]['acceleration_level']},
            4: {'position': self.motorParams['dir'][3]*angle_diff[3], 'speed': solution[3]['speed_rpm'], 'acceleration': solution[3]['acceleration_level']},
            5: {'position': self.motorParams['dir'][4]*angle_diff[4], 'speed': solution[4]['speed_rpm'], 'acceleration': solution[4]['acceleration_level']},
            6: {'position': self.motorParams['dir'][5]*angle_diff[5], 'speed': solution[5]['speed_rpm'], 'acceleration': solution[5]['acceleration_level']}
        })
        
        return True

    def position(self, base_angle, move_time):
        ls=[0]*6
        for i in range(6):
            ls[i]=base_angle[i]+self.motorParams['offset'][i]
        
        res=self.move_to_angle(ls, move_time)
        stt=self.controller.batch_read_status([1,2,3,4,5,6])
        print(stt)
        return res

# 使用示例
if __name__ == "__main__":
    myArm=armController(motorParams=
                       {'offset':    [41.5,    0,      0,      0,      0,          0   ],
                        'minn':      [2.5,   0,      0,      0,      -90,        -180],
                        'maxx':      [78.5,  150,    180,    335,    90,         180 ],
                        'dir':       [1,     1,      -1,     1,      -1,         1   ],
                        'decRate':   [50,    50,     50,     50,     3.166667,   1   ]
                        })
    if myArm.initialize()==True:

        myArm.position([0, 0, 45, 0, 0, 90], 2)
        time.sleep(2)
        myArm.position([30, 30, 45, 90, -90, -45], 2)
        time.sleep(2)
        myArm.position([-30, 0, 45, 0, -45, 45], 2)
        time.sleep(2)
        myArm.position([-15, 30, 45, 0, 90, -90], 2)
        time.sleep(2)
        myArm.position([15, 0, 45, 0, 0, 0], 2)
        time.sleep(2)
        myArm.position([0, 0, 0, 0, 0, 0], 2)

    # if False:
    #     fuck
    # else:
    #     try:
    #         # 创建优化的电机控制器
    #         motor1 = OptimizedZDTStepperMotor(motor_id=1, cache_timeout=0.1)
    #         motor2 = OptimizedZDTStepperMotor(motor_id=2, cache_timeout=0.1)
    #         motor3 = OptimizedZDTStepperMotor(motor_id=3, cache_timeout=0.1)
    #         motor4 = OptimizedZDTStepperMotor(motor_id=4, cache_timeout=0.1)
    #         motor5 = OptimizedZDTStepperMotor(motor_id=5, cache_timeout=0.1)
    #         motor6 = OptimizedZDTStepperMotor(motor_id=6, cache_timeout=0.1)
    #         # 创建批量控制器
    #         batch_controller = BatchMotorController([motor1, motor2, motor3, motor4, motor5, motor6])
    #         # batch_controller = BatchMotorController([motor2])
            
    #         # 批量使能
    #         enable_results = batch_controller.batch_enable([1,2,3,4,5,6], True)
    #         print(f"Enable results: {enable_results}")
            
    #         # homeResult=motor0.home_collision(direction=False,
    #         #     speed=90,
    #         #     current=350,
    #         #     detect_time=10
    #         # )
    #         # print(f"Home collision result: {homeResult}")
    #         # 批量速度控制
    #         speed_commands = {
    #             # 4: {'direction': False, 'speed': 0, 'acceleration': 50},
    #             # 5: {'direction': False, 'speed': 0, 'acceleration': 50},
    #             # 6: {'direction': True, 'speed': 0, 'acceleration': 30}
    #         }
    #         print(res6)
    #         pos_commands = {
    #             # 1: {'position' : 60*50, 'speed': max(res6['speed_rpm'],1), 'acceleration': res6['acceleration_level']},
    #             3: {'position': -45*50, 'speed': max(res6['speed_rpm'],1), 'acceleration': res6['acceleration_level']},
    #             # 5: {'position': 3.166667*45, 'speed': max(res6['speed_rpm'],1), 'acceleration': res6['acceleration_level']},
    #             # 6: {'position': 45, 'speed': max(res6['speed_rpm'],1), 'acceleration': res6['acceleration_level']},
    #         }
    #         # speed_results = batch_controller.batch_speed_control(speed_commands)
    #         pos_results = batch_controller.batch_pos_control(pos_commands)
    #         # print(f"Speed control results: {speed_results}")
    #         print(f"Position control results: {pos_results}")
            
    #         # 监控状态
    #         # for i in range(100):  # 减少循环次数以便测试
    #         #     time.sleep(0.1)
                
    #         #     # 方法1：分别批量读取
    #         #     positions = batch_controller.batch_read_positions(use_cache=True)
    #         #     speeds = batch_controller.batch_read_speeds()
    #         #     statuses = batch_controller.batch_read_status(use_cache=True)
                
    #         #     print(f"=== Round {i+1} ===")
    #         #     print(f"Positions: {positions}")
    #         #     print(f"Speeds: {speeds}")
    #         #     print(f"Statuses: {statuses}\n")
            
    #         # # # 停止所有电机
    #         # # for motor in [motor1, motor2]:
    #         # #     motor.stop()
            
    #     except KeyboardInterrupt:
    #         print("Interrupted by user")
    #     except Exception as e:
    #         print(f"Error: {e}")
    #     finally:
    #         # 清理资源
    #         for motor in [motor1, motor2]:
    #             motor.close()
    #         print("Motors closed")
