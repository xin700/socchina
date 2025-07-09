class StepperMotorController:
    def __init__(self, can_channel='can0', bitrate=500000, motor_id=1, offset=45.5, minn=7.5, maxx=83.5, dir=1, decRate=50):
        from ZTD import ZDTStepperMotor
        """
        初始化关节控制器
        :param motor_id: 电机ID
        :param offset: 角度偏移
        :param minn: 最小角度
        :param maxx: 最大角度
        :param dir: 方向(指示安全方向，当安全方向为负时取1，当安全方向为正时取-1)
        :param decRate: 减速比
        """
        self.motor = ZDTStepperMotor(
            can_channel=can_channel,
            bitrate=bitrate,
            motor_id=motor_id
        )
        self.angle_offset = offset
        self.min_angle = minn
        self.max_angle = maxx
        self.dir=dir
        self.decRate=decRate
        self.id = motor_id

    def initialize(self, homing_speed=90, homing_current=300):
        self.motor.enable(True)
        return self.motor.home_collision(
            direction=False,
            speed=homing_speed,
            current=homing_current,
            detect_time=10
        )

    def _calculate_acceleration(self, time, ang):
        vm=ang*2/time
        vrpm=vm*60/360
        t1=(time/2)*1000*1000/vrpm
        # print(time, ang, vrpm)
        acc=max(min(int(256-t1/50),240),1)
        # print(acc)
        return acc

    def move_to_angle(self, target_angle, move_time):
        if not (self.min_angle <= target_angle <= self.max_angle):
            print(f"超出允许范围{self.min_angle}-{self.max_angle}")
            return False
        current_pos = self.motor.read_real_position()
        if self.id == 5 or self.id == 6:
            if current_pos[0]==False:
                angle_diff = current_pos[1] - target_angle * self.decRate
            else:
                angle_diff = -current_pos[1] - target_angle * self.decRate
        else:
            angle_diff = current_pos[1] - target_angle * self.decRate
        # print(current_pos, angle_diff)
        self.motor.move_angle(
            angle=self.dir*angle_diff,
            speed=3000,
            acceleration=self._calculate_acceleration(move_time, abs(angle_diff))
        )
        return True

    def position(self, base_angle, move_time):
        return self.move_to_angle(base_angle + self.angle_offset, move_time)
    
    def position2(self, base_angle, move_vel):
        return self.move_to_angle2(base_angle + self.angle_offset, move_vel)

    def calcSpeed(self, angle_diff, move_time):
        v=angle_diff/move_time
        rpm=v*60/360
        return abs(int(rpm))

    def move_to_angle2(self, target_angle, move_vel):
        if not (self.min_angle <= target_angle <= self.max_angle):
            print(f"超出允许范围{self.min_angle}-{self.max_angle}")
            return False
        current_pos = self.motor.read_real_position()
        if self.id == 5:
            print("fuck")
            if current_pos[0]==False:
                angle_diff = current_pos[1] - target_angle * self.decRate
            else:
                angle_diff = -current_pos[1] - target_angle * self.decRate
        elif self.id == 6:
            if current_pos[0]==False:
                angle_diff = current_pos[1] + target_angle * self.decRate
            else:
                angle_diff = -current_pos[1] + target_angle * self.decRate
        else:
            angle_diff = current_pos[1] - target_angle * self.decRate
        # print(current_pos, angle_diff)
        if self.id == 5 or self.id==6:
            self.motor.move_angle(
                angle=self.dir*angle_diff,
                speed=move_vel,
                acceleration=1
            )
        else:
            self.motor.move_angle(
                angle=self.dir*angle_diff,
                speed=move_vel,
                acceleration=30
            )
        return True

def action():
    joint4.initialize(homing_current=350)
    joint1.position(30,2)
    joint2.position(75,2)
    joint3.position(120,2)
    joint4.position(60,2)
    joint5.position(-45,2)
    time.sleep(2)
    joint1.position(-30,2)
    joint2.position(45,2)
    joint3.position(90,2)
    joint4.position(30,2)
    joint5.position(-15,2)
    time.sleep(2)
    joint1.position(-15,2)
    joint2.position(60,2)
    joint3.position(135,2)
    joint4.position(90,2)
    joint5.position(-90,2)
    time.sleep(2)
    joint1.position(0,3)
    joint2.position(50,3)
    joint3.position(45,3)
    joint4.position(310,3)
    joint5.position(-60,3)
    time.sleep(3)
    joint1.position(20,3)
    joint2.position(45,3)
    joint3.position(60,3)
    joint4.position(30,3)
    joint5.position(25,3)
    time.sleep(3)
    joint1.position(0,2)
    joint2.position(0,2)
    joint3.position(0,2)
    joint4.position(0,2)
    joint5.position(0,2)

if __name__ == '__main__':
    # joint1=StepperMotorController(can_channel='can0', motor_id=1, offset=40.5, minn=2.5, maxx=78.5, dir=1, decRate=50)
    # joint1.initialize(homing_current=350)
    # joint1.position(0, 2)
    # joint2=StepperMotorController(can_channel='can0', motor_id=2, offset=0, minn=-90, maxx=90, dir=1, decRate=50)
    # joint3=StepperMotorController(can_channel='can0', motor_id=3, offset=0, minn=0, maxx=180, dir=-1, decRate=50)
    # joint4=StepperMotorController(can_channel='can0', motor_id=4, offset=0, minn=0, maxx=315, dir=1, decRate=50)
    # joint5=StepperMotorController(can_channel='can0', motor_id=5, offset=0, minn=-90, maxx=90, dir=-1, decRate=3.166667)
    # time.sleep(4)
    # joint6=StepperMotorController(can_channel='can0', motor_id=6, offset=0, minn=-180, maxx=180, dir=-1, decRate=1)

    import time
    joint1=StepperMotorController(can_channel='can0', motor_id=1, offset=40.5, minn=2.5, maxx=78.5, dir=1, decRate=50)
    # joint1.initialize(homing_current=350)
    # joint1.position(0, 2)
    joint2=StepperMotorController(can_channel='can0', motor_id=2, offset=0, minn=-90, maxx=90, dir=1, decRate=50)
    joint3=StepperMotorController(can_channel='can0', motor_id=3, offset=0, minn=0, maxx=180, dir=-1, decRate=50)
    joint4=StepperMotorController(can_channel='can0', motor_id=4, offset=0, minn=0, maxx=315, dir=1, decRate=50)
    joint5=StepperMotorController(can_channel='can0', motor_id=5, offset=0, minn=-90, maxx=90, dir=-1, decRate=3.166667)
    # time.sleep(4)
    joint6=StepperMotorController(can_channel='can0', motor_id=6, offset=0, minn=-180, maxx=180, dir=-1, decRate=1)
    # action()
    joint1.position2(-35,1)
    time.sleep(1)
    joint1.position2(-20,0.5)
    time.sleep(0.3)
    joint1.position2(-10,0.5)
    time.sleep(0.3)
    joint1.position2(-2.5,0.5)
    time.sleep(0.3)
    joint1.position2(-0,0.5)
    joint5.position(45,2)
# time.sleep(0.1)

# joint4.position(0, 4)
# joint5.position(0, 5)
# print(joint1.motor.read_real_position())
# joint1.position(0,3)
# joint2.position(0,3)
# joint3.position(0,3)
# time.sleep(4)
# joint1.position(30,3)
# joint2.position(75,3)
# joint3.position(120,3)
# time.sleep(4)
# joint1.position(-30,3)
# joint2.position(45,3)
# joint3.position(90,3)
# time.sleep(4)
# joint1.position(-15,3)
# joint2.position(60,3)
# joint3.position(165,3)
# time.sleep(4)
# joint1.position(0,3)
# joint2.position(0,3)
# joint3.position(0,3)
