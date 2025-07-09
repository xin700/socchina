from flask import Flask, request, jsonify
from ctrlClass import StepperMotorController
import time
import can

app = Flask(__name__)
joint=[]*6
joint.append(StepperMotorController(can_channel='can0', motor_id=1, offset=40.5, minn=2.5, maxx=78.5, dir=1, decRate=50))
joint.append(StepperMotorController(can_channel='can0', motor_id=2, offset=0, minn=0, maxx=150, dir=1, decRate=50))
joint.append(StepperMotorController(can_channel='can0', motor_id=3, offset=0, minn=0, maxx=180, dir=-1, decRate=50))
joint.append(StepperMotorController(can_channel='can0', motor_id=4, offset=0, minn=0, maxx=335, dir=1, decRate=50))
joint.append(StepperMotorController(can_channel='can0', motor_id=5, offset=0, minn=-90, maxx=90, dir=-1, decRate=3.166667))
joint.append(StepperMotorController(can_channel='can0', motor_id=6, offset=0, minn=-180, maxx=180, dir=1, decRate=1))
decR=[50, 50, 50, 50, 3.166667, 1]
k=[1, 1, 1, 1, 2, 6]
# joint[2]=StepperMotorController(can_channel='can0', motor_id=3, offset=0, minn=0, maxx=180, dir=-1, decRate=50)
# joint[3]=StepperMotorController(can_channel='can0', motor_id=4, offset=0, minn=0, maxx=315, dir=1, decRate=50)
# joint[4]=StepperMotorController(can_channel='can0', motor_id=5, offset=0, minn=-90, maxx=90, dir=-1, decRate=3.166667)
# joint[5]=StepperMotorController(can_channel='can0', motor_id=6, offset=0, minn=-180, maxx=180, dir=-1, decRate=1)

# for i in range(1,6):
#     joint[i].position(0, 6)

# time.sleep(6)
joint[0].initialize(homing_current=350)
time.sleep(8)
joint[2].position(45, 2)
joint[3].position(180, 4)
joint[0].position(0, 2)
time.sleep(2)
joint[2].position(0, 2)
time.sleep(2)

def flush_can_buffer(bus: can.Bus):
    """清理 CAN 总线接收缓冲区的所有待处理消息"""
    while True:
        try:
            # 设置超时为0，立即返回（非阻塞）
            msg = bus.recv(timeout=0)
            if msg is None:
                break
        except can.CanError as e:
            print(f"清理缓冲区失败: {e}")
            break

@app.route('/data', methods=['POST'])
def receive_data():
    data = request.json
    print("Received data:", data)
    posNow=[]
    toterr=0
    for i in range(5):
        posNow.append(joint[i].motor.read_real_position()[1]/decR[i])
        time.sleep(0.1)
        if i==0:
            continue
        toterr+=abs(abs(posNow[i])-abs(data['start'][i]))
    print(posNow)
    print(toterr)
    if toterr>5:
        print("初始位置不一致")
        # return jsonify({"status": "error", "message": "初始位置不一致"})
    for i in range(5):
        joint[i].position2(data['end'][i], int(data['vel'][i]*decR[i]/k[i]))

    flush_can_buffer(joint[0].motor.can_bus)

    return jsonify({"status": "success"})

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)