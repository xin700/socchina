#!/usr/bin/env python3
"""
ZDT Stepper Motor Control via CAN for RDK X5 running Ubuntu 22.04
Based on Emm42_V5.0 documentation
"""

import time
import struct
import can
from typing import Union, List, Tuple

class ZDTStepperMotor:
    """
    Class to control ZDT stepper motor via CAN bus
    """

    # Command function codes
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
    
    # Special command sub-codes
    SUB_ENABLE = 0xAB
    SUB_STOP = 0x98
    SUB_SYNC_MOVE = 0x66
    SUB_MODIFY_CONFIG = 0xD1

    # Default checksum byte
    DEFAULT_CHECKSUM = 0x6B
    
    def __init__(self, can_channel: str = 'can0', bitrate: int = 500000, 
                 motor_id: int = 1, extended_id: bool = True):
        """
        Initialize the ZDT stepper motor controller
        
        Args:
            can_channel: CAN interface name (default: 'can0')
            bitrate: CAN bitrate (default: 500000)
            motor_id: Motor ID address (default: 1)
            extended_id: Use extended CAN IDs (default: True)
        """
        self.motor_id = motor_id
        self.extended_id = extended_id
        self.can_bus = can.interface.Bus(channel=can_channel, 
                                         bustype='socketcan', 
                                         bitrate=bitrate)
        print(f"Connected to {can_channel} at {bitrate} bps")
        
        # Wait for bus to initialize
        time.sleep(0.1)
        
    def close(self):
        """Close the CAN bus connection"""
        self.can_bus.shutdown()
        
    def _send_command(self, cmd_data: List[int], expect_response: bool = True, 
                      timeout: float = 1.0) -> Union[List[int], None]:
        """
        Send a command to the motor and optionally wait for a response
        
        Args:
            cmd_data: List of command bytes
            expect_response: Whether to wait for a response
            timeout: Timeout in seconds for response
            
        Returns:
            Response data if expect_response is True, otherwise None
        """
        # Split command into packets if necessary
        if len(cmd_data) <= 8:
            # Short command, single packet
            arb_id = (self.motor_id << 8) | 0x00  # Frame ID = motor_id << 8 | packet_num (0)
            
            msg = can.Message(
                arbitration_id=arb_id,
                data=cmd_data,
                is_extended_id=self.extended_id
            )
            self.can_bus.send(msg)
        else:
            # Long command, multiple packets
            # print(len(cmd_data))
            i=0
            while True:
                if i!= 0:
                    packet_data = [cmd_data[0]]+cmd_data[i:i+7]
                else:
                    packet_data = cmd_data[i:i+8]
                if i>= len(cmd_data):
                    break
                packet_num = i // 7
                arb_id = (self.motor_id << 8) | packet_num
                
                msg = can.Message(
                    arbitration_id=arb_id,
                    data=packet_data,
                    is_extended_id=self.extended_id
                )
                # print(msg)
                self.can_bus.send(msg)
                if i==0:
                    i+=8
                else:
                    i+=7
                time.sleep(0.005)  # Small delay between packets
        
        if not expect_response:
            return None
            
        # Wait for response
        response_data = []
        start_time = time.time()
        
        # Expect first response packet
        while time.time() - start_time < timeout:
            response = self.can_bus.recv(timeout=0.1)
            if response and (response.arbitration_id >> 8) == self.motor_id:
                response_data.extend(response.data)
                break
        
        packet_num = 1
        while response and time.time() - start_time < timeout:
            response = self.can_bus.recv(timeout=0.1)
            if response and (response.arbitration_id >> 8) == self.motor_id and (response.arbitration_id & 0xFF) == packet_num:
                response_data.extend(response.data)
                packet_num += 1
            else:
                # No more packets or timeout
                break
                
        return response_data if response_data else None
    
    def enable(self, enable_state: bool = True, sync_flag: bool = False) -> bool:
        """
        Enable or disable the motor
        
        Args:
            enable_state: True to enable, False to disable
            sync_flag: Use synchronous control
            
        Returns:
            True if command successful, False otherwise
        """
        cmd = [self.CMD_ENABLE, self.SUB_ENABLE, 
               0x01 if enable_state else 0x00, 
               0x01 if sync_flag else 0x00, 
               self.DEFAULT_CHECKSUM]
               
        response = self._send_command(cmd)
        if response and len(response) >= 3:
            return response[2] == 0x02  # 0x02 means success
        return False
    
    def speed_mode(self, direction: bool = False, speed: int = 0, 
                   acceleration: int = 0, sync_flag: bool = False) -> bool:
        """
        Control motor in speed mode
        
        Args:
            direction: False for CW (0), True for CCW (1)
            speed: Speed in RPM (0-3000)
            acceleration: Acceleration level (0-255, 0 for no acceleration curve)
            sync_flag: Use synchronous control
            
        Returns:
            True if command successful, False otherwise
        """
        # Convert speed to 2-byte value
        speed_high = (speed >> 8) & 0xFF
        speed_low = speed & 0xFF
        
        cmd = [self.CMD_SPEED_MODE, 
               0x01 if direction else 0x00, 
               speed_high, speed_low, 
               acceleration, 
               0x01 if sync_flag else 0x00,
               self.DEFAULT_CHECKSUM]
               
        response = self._send_command(cmd)
        if response and len(response) >= 3:
            return response[2] == 0x02  # 0x02 means success
        return False
    
    def _angle_to_pulse(self, angle: float, microsteps: int = 16) -> int:
        """
        Convert angle in degrees to pulse count for 1.8° stepper motor
        
        Args:
            angle: Angle in degrees
            microsteps: Microstep setting (default: 16)
            
        Returns:
            Number of pulses
        """
        steps_per_rev = 200  # For 1.8° motor (360° / 1.8° = 200)
        pulses_per_rev = steps_per_rev * microsteps
        pulse_count = int((angle / 360.0) * pulses_per_rev)
        return pulse_count
    
    def position_mode(self, direction: bool = False, speed: int = 0, 
                     acceleration: int = 0, pulse_count: int = 0, 
                     is_absolute: bool = False, sync_flag: bool = False) -> bool:
        """
        Control motor in position mode
        
        Args:
            direction: False for CW (0), True for CCW (1)
            speed: Speed in RPM (0-3000)
            acceleration: Acceleration level (0-255, 0 for no acceleration curve)
            pulse_count: Number of pulses to move
            is_absolute: True for absolute position, False for relative position
            sync_flag: Use synchronous control
            
        Returns:
            True if command successful, False otherwise
        """
        # Convert speed to 2-byte value
        speed_high = (speed >> 8) & 0xFF
        speed_low = speed & 0xFF
        
        # Convert pulse count to 4-byte value
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
               
        response = self._send_command(cmd)
        if response and len(response) >= 3:
            return response[2] == 0x02  # 0x02 means success
        return False
        
    def move_angle(self, angle: float, direction: bool = None, speed: int = 300, 
                  acceleration: int = 5, is_absolute: bool = False, 
                  sync_flag: bool = False, microsteps: int = 16) -> bool:
        """
        Move motor by specified angle in degrees
        
        Args:
            angle: Angle in degrees (positive value)
            direction: False for CW (0), True for CCW (1). If None, direction is determined by angle sign
            speed: Speed in RPM (0-3000)
            acceleration: Acceleration level (0-255, 0 for no acceleration curve)
            is_absolute: True for absolute position, False for relative position
            sync_flag: Use synchronous control
            microsteps: Microstep setting (default: 16)
            
        Returns:
            True if command successful, False otherwise
        """
        # Determine direction from angle if not specified
        if direction is None:
            if angle < 0:
                direction = True  # CCW
                angle = abs(angle)
            else:
                direction = False  # CW
                
        # Convert angle to pulse count
        pulse_count = self._angle_to_pulse(angle, microsteps)
        
        # Call position_mode with calculated pulse count
        return self.position_mode(
            direction=direction,
            speed=speed,
            acceleration=acceleration,
            pulse_count=pulse_count,
            is_absolute=is_absolute,
            sync_flag=sync_flag
        )
    
    def stop(self, sync_flag: bool = False) -> bool:
        """
        Stop the motor immediately
        
        Args:
            sync_flag: Use synchronous control
            
        Returns:
            True if command successful, False otherwise
        """
        cmd = [self.CMD_STOP, self.SUB_STOP, 
               0x01 if sync_flag else 0x00, 
               self.DEFAULT_CHECKSUM]
               
        response = self._send_command(cmd)
        if response and len(response) >= 3:
            return response[2] == 0x02  # 0x02 means success
        return False
    
    def sync_move(self) -> bool:
        """
        Synchronize multiple motors to start moving together
        
        Returns:
            True if command successful, False otherwise
        """
        cmd = [self.CMD_SYNC_MOVE, self.SUB_SYNC_MOVE, 
               self.DEFAULT_CHECKSUM]
               
        response = self._send_command(cmd)
        if response and len(response) >= 3:
            return response[2] == 0x02  # 0x02 means success
        return False
    
    def read_bus_voltage(self) -> int:
        """
        Read the bus voltage
        
        Returns:
            Bus voltage in mV, or -1 if error
        """
        cmd = [self.CMD_READ_VOLTAGE, self.DEFAULT_CHECKSUM]
        response = self._send_command(cmd)
        
        if response and len(response) >= 4:
            voltage = (response[2] << 8) | response[3]
            return voltage
        return -1
    
    def read_phase_current(self) -> int:
        """
        Read the phase current
        
        Returns:
            Phase current in mA, or -1 if error
        """
        cmd = [self.CMD_READ_CURRENT, self.DEFAULT_CHECKSUM]
        response = self._send_command(cmd)
        
        if response and len(response) >= 4:
            current = (response[1] << 8) | response[2]
            return current
        return -1
    
    def read_encoder_value(self) -> int:
        """
        Read the linearized encoder value
        
        Returns:
            Encoder value (0-65535 for one revolution), or -1 if error
        """
        cmd = [self.CMD_READ_ENCODER, self.DEFAULT_CHECKSUM]
        response = self._send_command(cmd)
        
        if response and len(response) >= 4:
            encoder_value = (response[2] << 8) | response[3]
            return encoder_value
        return -1
        
    def read_real_speed(self) -> Tuple[bool, int]:
        """
        Read the real-time motor speed
        
        Returns:
            Tuple of (direction, speed)
            direction: False for CW, True for CCW
            speed: Speed in RPM, or -1 if error
        """
        cmd = [self.CMD_READ_REAL_SPEED, self.DEFAULT_CHECKSUM]
        response = self._send_command(cmd)
        
        if response and len(response) >= 5:
            direction = response[2] == 0x01
            speed = (response[3] << 8) | response[4]
            return (direction, speed)
        return (False, -1)
    
    def read_real_position(self) -> Tuple[bool, float]:
        """
        Read the real-time motor position
        
        Returns:
            Tuple of (direction, position)
            direction: False for positive, True for negative
            position: Position in degrees, or -1 if error
        """
        cmd = [self.CMD_READ_REAL_POS, self.DEFAULT_CHECKSUM]
        response = self._send_command(cmd)
        
        if response and len(response) >= 7:
            direction = response[1] == 0x01
            position_raw = (response[2] << 24) | (response[3] << 16) | (response[4] << 8) | response[5]
            position_degrees = (position_raw * 360.0) / 65536.0
            return (direction, position_degrees)
        return (False, -1)
    
    def read_motor_status(self) -> dict:
        """
        Read the motor status flags
        
        Returns:
            Dictionary of status flags, or empty dict if error
        """
        cmd = [self.CMD_READ_MOTOR_STATUS, self.DEFAULT_CHECKSUM]
        response = self._send_command(cmd)
        
        if response and len(response) >= 3:
            status_byte = response[1]
            return {
                'enabled': bool(status_byte & 0x01),
                'position_reached': bool(status_byte & 0x02),
                'stalled': bool(status_byte & 0x04),
                'stall_protection': bool(status_byte & 0x08)
            }
        return {}
    
    def read_config(self) -> dict:
        """
        Read the motor configuration parameters
        
        Returns:
            Dictionary of configuration parameters, or empty dict if error
        """
        cmd = [self.CMD_READ_CONFIG, 0x6C, self.DEFAULT_CHECKSUM]
        response = self._send_command(cmd)
        
        if not response or len(response) < 10:
            return {}
            
        # Parse configuration parameters from response
        # This is a simplification; actual implementation would parse all parameters
        config = {
            'motor_type': "1.8°" if response[4] == 0x19 else "0.9°",
            'pulse_mode': response[5],
            'serial_mode': response[6],
            'en_pin_mode': response[7],
            'dir_pin_mode': response[8],
            'microsteps': 256 if response[9] == 0x00 else response[9],
            # Add more parameters as needed
        }
        
        return config
    
    def read_system_status(self) -> dict:
        """
        Read the system status parameters
        
        Returns:
            Dictionary of system status, or empty dict if error
        """
        cmd = [self.CMD_READ_SYSTEM_STATUS, 0x7A, self.DEFAULT_CHECKSUM]
        response = self._send_command(cmd)
        
        if not response or len(response) < 20:
            return {}
            
        # Extract key status information
        status = {}
        
        # Bus voltage (mV)
        status['bus_voltage'] = (response[4] << 8) | response[5]
        
        # Phase current (mA)
        status['phase_current'] = (response[6] << 8) | response[7]
        
        # Encoder value
        status['encoder_value'] = (response[8] << 8) | response[9]
        
        # Real-time speed
        direction_speed = response[14] == 0x01
        speed = (response[15] << 8) | response[16]
        status['speed'] = -speed if direction_speed else speed
        
        # Motor status flags byte
        motor_status = response[29]
        status['enabled'] = bool(motor_status & 0x01)
        status['position_reached'] = bool(motor_status & 0x02)
        status['stalled'] = bool(motor_status & 0x04)
        status['stall_protection'] = bool(motor_status & 0x08)
        
        return status
    
    def modify_config(self, config_dict: dict, store: bool = True) -> bool:
        """
        Modify motor configuration parameters
        
        Args:
            config_dict: Dictionary of parameters to modify
            store: Whether to store parameters in non-volatile memory
            
        Returns:
            True if command successful, False otherwise
        """
        # This is a simplified implementation
        # Real implementation would need to handle all configuration parameters
        
        # Example configuration bytes (motor type, pulse mode, serial mode, etc.)
        # See manual section 6.3.5 for full parameter list
        config_bytes = [
            0x19,  # 1.8° motor type
            0x02,  # PUL_FOC mode
            0x02,  # UART_FUN mode
            0x02,  # En pin always enabled
            0x00,  # Dir pin CW direction
            0x10,  # 16 microsteps
            0x01,  # Enable microstep interpolation
            0x00,  # Disable auto screen off
            0x03, 0xE8,  # Open loop current 1000mA
            0x0B, 0xB8,  # Closed loop max current 3000mA
            0x0F, 0xA0,  # Max output voltage 4000mV
            0x05,  # UART baud rate 115200
            0x07,  # CAN baud rate 500000
            0x01,  # ID address 1
            0x00,  # Checksum type 0x6B
            0x01,  # Response mode Receive
            0x01,  # Enable stall protection
            0x00, 0x28,  # Stall protection speed threshold 40RPM
            0x09, 0x60,  # Stall protection current threshold 2400mA
            0x0F, 0xA0,  # Stall protection time threshold 4000ms
            0x00, 0x01,  # Position arrival window 0.1°
        ]
        
        # Update configuration bytes based on config_dict
        # This is where you would apply the changes from config_dict
        
        cmd = [self.CMD_MODIFY_CONFIG, self.SUB_MODIFY_CONFIG, 
               0x01 if store else 0x00] + config_bytes + [self.DEFAULT_CHECKSUM]
               
        response = self._send_command(cmd)
        if response and len(response) >= 3:
            return response[2] == 0x02  # 0x02 means success
        return False

    def trigger_homing(self, mode: int, sync_flag: bool = False) -> bool:
        """
        触发回零操作
        
        Args:
            mode: 回零模式
                0 - 单圈就近回零
                1 - 单圈方向回零
                2 - 多圈无限位碰撞回零
                3 - 多圈有限位开关回零
            sync_flag: 是否启用多机同步
                
        Returns:
            True if command successful, False otherwise
        """
        cmd = [0x9A, mode, 
            0x01 if sync_flag else 0x00, 
            self.DEFAULT_CHECKSUM]
            
        response = self._send_command(cmd)
        if response and len(response) >= 3:
            return response[2] == 0x02  # 0x02 means success
        return False

    def read_origin_status(self) -> dict:
        """
        读取回零状态标志位
        
        Returns:
            Dictionary with status flags, or empty dict if error
        """
        cmd = [0x3B, self.DEFAULT_CHECKSUM]
        response = self._send_command(cmd)
        
        if response and len(response) >= 3:
            status_byte = response[2]
            return {
                'encoder_ready': bool(status_byte & 0x01),
                'calibration_table_ready': bool(status_byte & 0x02),
                'homing_in_progress': bool(status_byte & 0x04),
                'homing_failed': bool(status_byte & 0x08)
            }
        return {}
    def read_origin_parameters(self) -> dict:
        """
        读取原点回零参数
        
        Returns:
            Dictionary with homing parameters, or empty dict if error
        """
        cmd = [0x22, self.DEFAULT_CHECKSUM]
        response = self._send_command(cmd)
        
        if response and len(response) >= 15:
            return {
                'mode': response[2],  # 回零模式
                'direction': response[3],  # 回零方向
                'speed': (response[4] << 8) | response[5],  # 回零转速
                'timeout': (response[6] << 24) | (response[7] << 16) | (response[8] << 8) | response[9],  # 回零超时
                'collision_speed': (response[10] << 8) | response[11],  # 碰撞回零检测转速
                'collision_current': (response[12] << 8) | response[13],  # 碰撞回零检测电流
                'collision_time': (response[14] << 8) | response[15],  # 碰撞回零检测时间
                'auto_home_on_power': response[16]  # 是否上电自动回零
            }
        return {}
    def abort_homing(self) -> bool:
        """
        强制中断并退出回零操作
        
        Returns:
            True if command successful, False otherwise
        """
        cmd = [0x9C, 0x48, self.DEFAULT_CHECKSUM]
            
        response = self._send_command(cmd)
        if response and len(response) >= 3:
            return response[2] == 0x02  # 0x02 means success
        return False
    def modify_origin_parameters(self, params: dict, store: bool = True) -> bool:
        """
        修改原点回零参数
        
        Args:
            params: 参数字典，可包含以下键：
                'mode': 回零模式 (0=就近, 1=方向, 2=碰撞, 3=限位)
                'direction': 回零方向 (0=CW, 1=CCW)
                'speed': 回零转速 (RPM)
                'timeout': 回零超时时间 (ms)
                'collision_speed': 碰撞回零检测转速 (RPM)
                'collision_current': 碰撞回零检测电流 (mA)
                'collision_time': 碰撞回零检测时间 (ms)
                'auto_home_on_power': 是否上电自动回零 (0=否, 1=是)
            store: 是否存储到非易失性存储器中
                
        Returns:
            True if command successful, False otherwise
        """
        # 设置默认值
        mode = params.get('mode', 0)
        direction = params.get('direction', 0)
        speed = params.get('speed', 30)
        timeout = params.get('timeout', 10000)
        collision_speed = params.get('collision_speed', 300)
        collision_current = params.get('collision_current', 800)
        collision_time = params.get('collision_time', 60)
        auto_home = params.get('auto_home_on_power', 0)
        
        # 构建参数字节
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
        hex_nums = [format(x, '02x') for x in cmd]
        # print(hex_nums) 
        response = self._send_command(cmd)
        # print(response)
        if response and len(response) >= 3:
            return response[1] == 0x02  # 0x02 means success
        return False

    def home_collision(self, direction: bool = False, 
                    speed: int = 100, current: int = 800, 
                    detect_time: int = 60, wait_timeout: float = 30.0) -> bool:
        """
        执行多圈无限位碰撞回零 - 电机朝指定方向运动直到碰撞检测条件满足
        
        Args:
            direction: False为CW方向, True为CCW方向
            speed: 回零速度(RPM)
            current: 碰撞检测电流阈值(mA)
            detect_time: 碰撞检测时间阈值(ms)
            wait_timeout: 等待回零完成的超时时间(秒)
                
        Returns:
            True if homing successful, False otherwise
        """
        # 设置碰撞回零参数
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
        print("回零参数设置成功")
        # 触发多圈无限位碰撞回零
        if not self.trigger_homing(mode=2):
            return False
            
        # 等待回零完成
        start_time = time.time()
        while time.time() - start_time < wait_timeout:
            status = self.read_origin_status()
            if not status:
                return False
                
            # 检查回零是否在进行中
            if not status.get('homing_in_progress', False):
                # 回零已完成，检查是否失败
                return not status.get('homing_failed', True)
                
            time.sleep(0.5)  # 等待一段时间再检查
            
        # 超时，中断回零
        self.abort_homing()
        return False

# Example usage
if __name__ == "__main__":
    try:
        # Initialize motor
        motor = ZDTStepperMotor(can_channel='can0', bitrate=500000, motor_id=1)
        motor2 = ZDTStepperMotor(can_channel='can0', bitrate=500000, motor_id=2)
        motor3 = ZDTStepperMotor(can_channel='can0', bitrate=500000, motor_id=3)
        motor4 = ZDTStepperMotor(can_channel='can0', bitrate=500000, motor_id=4)
        motor5 = ZDTStepperMotor(can_channel='can0', bitrate=500000, motor_id=5)
        # Read motor status
        # print("Reading motor status...")
        # status = motor.read_motor_status()
        # print(f"Motor status: {status}")
        
        # # Enable motor
        # print("Enabling motor...")
        # if motor.enable(True):
        #     print("Motor enabled successfully")
        # else:
        #     print("Failed to enable motor")

        result = motor.home_collision(
            direction=False,    # CCW方向
            speed=90,         # 回零速度300RPM
            current=350,       # 碰撞检测电流阈值800mA
            detect_time=20    # 碰撞检测时间阈值60ms
        )

        # result = motor4.home_collision(
        #     direction=False,    # CCW方向
        #     speed=90,         # 回零速度300RPM
        #     current=350,       # 碰撞检测电流阈值800mA
        #     detect_time=20    # 碰撞检测时间阈值60ms
        # )

        # result = motor2.home_collision(
        #     direction=False,    # CCW方向
        #     speed=90,         # 回零速度300RPM
        #     current=220,       # 碰撞检测电流阈值800mA
        #     detect_time=20    # 碰撞检测时间阈值60ms
        # )

        # result = motor3.home_collision(
        #     direction=True,    # CCW方向
        #     speed=90,         # 回零速度300RPM
        #     current=100,       # 碰撞检测电流阈值800mA
        #     detect_time=20    # 碰撞检测时间阈值60ms
        # )

        # if result:
        #     print("碰撞回零成功！")
        # else:
        #     print("碰撞回零失败！")
        #-为true,+为false

        # motor.move_angle(angle=-20*50, speed=90, acceleration=0)
        #从上往下看，+为顺时针，-为逆时针
        # motor2.move_angle(angle=20*50, speed=90, acceleration=0)
        #-为往外转，+为往里转
        # motor3.move_angle(angle=-45*50, speed=3000, acceleration=220)
        #-向前，+向后
        # motor4.move_angle(angle=-90*50, speed=90, acceleration=0)

        # motor5.move_angle(angle=-90*3, speed=3000, acceleration=150)
        #+
        # motor3.enable(True)
        time.sleep(2)  # Wait for movement to complete
        status=False
        while not status:
            st = motor5.read_motor_status()
            print(f"Motor status: {st}")
            status = st['position_reached']
            time.sleep(0.5)
        # # Move in position mode (CW direction, 300 RPM, 3200 pulses = 1 revolution at 16 microsteps)
        # print("Moving in position mode...")
        # motor.position_mode(direction=False, speed=300, acceleration=0, pulse_count=3200)
        # time.sleep(3)  # Wait for movement to complete
        
        # # Read real-time speed
        # direction, speed = motor.read_real_speed()
        # print(f"Current speed: {'-' if direction else ''}{speed} RPM")
        
        # Read real-time position
        # direction, position = motor.read_real_position()
        # print(f"Current position: {'-' if direction else ''}{position:.2f} degrees")
        
        # # Read bus voltage
        # voltage = motor.read_bus_voltage()
        # print(f"Bus voltage: {voltage} mV")
        
        # Read phase current
        # current = motor.read_phase_current()
        # print(f"Phase current: {current} mA")
        
        # # Read system status
        # system_status = motor.read_system_status()
        # print(f"System status: {system_status}")
        
        # # Disable motor
        # print("Disabling motor...")
        # motor.enable(False)
        
    except KeyboardInterrupt:
        print("Program interrupted by user")
    except Exception as e:
        print(f"Error: {e}")
    finally:
        motor.close()  # Close the CAN connection
        print("Program terminated")