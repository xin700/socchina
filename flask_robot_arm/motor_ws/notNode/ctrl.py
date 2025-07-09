from ZTD import ZDTStepperMotor
import math

motor = ZDTStepperMotor(can_channel='can0', bitrate=500000, motor_id=1)

def motor1Init():
    motor.enable(True)
    result = motor.home_collision(
        direction=False,    # CCW方向
        speed=90,         # 回零速度300RPM
        current=300,       # 碰撞检测电流阈值800mA
        detect_time=10    # 碰撞检测时间阈值60ms
    )

def calcAcc(time, ang):
    vm=ang*2/time
    vrpm=vm*60/360
    t1=(time/2)*1000*1000/vrpm
    acc=max(min(int(256-t1/50),240),1)
    return acc

def motor1Ctrl(angle, time):
    if angle<7.5 or angle>83.5:
        print("出界")
        return False
    posNow=motor.read_real_position()
    err=posNow[1]-angle*50
    motor.move_angle(
        angle=err,
        speed=3000,
        acceleration=calcAcc(time, abs(err))
    )
    return True

def angle1(ang1):
    return motor1Ctrl(ang1+45.5, 3)
# motor1Init()
# motor.move_angle(angle=-30*50, speed=1000, acceleration=calcAcc(4, 30*50))

# print(motor.read_real_position())
# print(calcAcc(3, 30*50))
#从上往下看，+为顺时针，-为逆时针
# motor2.move_angle(angle=20*50, speed=90, acceleration=0)
#-为往外转，+为往里转
# motor3.move_angle(angle=-45*50, speed=3000, acceleration=220)
#-向前，+向后