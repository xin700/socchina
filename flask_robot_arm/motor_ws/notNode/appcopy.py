from flask import Flask, request
from pose import greetPose
import subprocess
import time
import json


def publish_custom_pose(position, orientation, topic="/pose"):
    # 构造消息字符串
    pose_data = (
        f'\"{{position: {{x: {position["x"]}, y: {position["y"]}, z: {position["z"]}}}, '
        f'orientation: {{x: {orientation["x"]}, y: {orientation["y"]}, '
        f'z: {orientation["z"]}, w: {orientation["w"]}}}}}\"'
    )

    command = (
        'source ~/.bashrc && source /opt/ros/humble/setup.bash && '
        f'ros2 topic pub --once /pose geometry_msgs/msg/Pose '
        f'{pose_data}'
    )
    print(command)
    subprocess.run(command, capture_output=True, executable="/bin/bash", shell=True)
    # try:
    #     result = subprocess.run(command, check=True, text=True, capture_output=True)
    #     print(f"成功发布消息到 {topic}")
    #     return True
    # except subprocess.CalledProcessError as e:
    #     print(f"发布失败: {e}\n错误详情: {e.stderr}")
    #     return False

initX = 0.20
initY = 0.0
initZ = 0.15

publish_custom_pose(
    position={"x": initX, "y": initY, "z": initZ},
    orientation={"x": 1.0, "y": 0.0, "z": 0.0, "w": 0.0}
)

time.sleep(2)
# cmd = f'ros2 topic pub -1 /pose geometry_msgs/msg/Pose \'{{"data": "{message}"}}\''
app = Flask(__name__)

def greet():
    greetPose()
    global initX, initY, initZ
    initX = 0.20
    initY = 0.0
    initZ = 0.15
    publish_custom_pose(
        position={"x": initX, "y": initY, "z": initZ},
        orientation={"x": 1.0, "y": 0.0, "z": 0.0, "w": 0.0}
    )
    return f'success\n'
    
def grasp():
    publish_custom_pose(
        position={"x": 20, "y": 10, "z": 10},
        orientation={"x": 0.9576622, "y": 0.25660481, "z": -0.12607862, "w": 0.03378266}        
    )
    return f'success\n'

def up(distance):
    global initZ
    print(distance)
    initZ+= distance/100.0
    publish_custom_pose(
        position={"x": initX, "y": initY, "z": initZ},
        orientation={"x": 1.0, "y": 0.0, "z": 0.0, "w": 0.0}
    )
    return f'success\n'

def down(distance):
    global initZ
    print(distance)
    initZ-= distance/100.0
    publish_custom_pose(
        position={"x": initX, "y": initY, "z": initZ},
        orientation={"x": 1.0, "y": 0.0, "z": 0.0, "w": 0.0}
    )
    return f'success\n'

def left(distance):
    global initY
    print(distance)
    initY+= distance/100.0
    publish_custom_pose(
        position={"x": initX, "y": initY, "z": initZ},
        orientation={"x": 1.0, "y": 0.0, "z": 0.0, "w": 0.0}
    )
    return f'success\n'

def right(distance):
    global initY
    print(distance)
    initY-= distance/100.0
    publish_custom_pose(
        position={"x": initX, "y": initY, "z": initZ},
        orientation={"x": 1.0, "y": 0.0, "z": 0.0, "w": 0.0}
    )
    return f'success\n'

def forward(distance):
    global initX
    print(distance)
    initX+= distance/100.0
    publish_custom_pose(
        position={"x": initX, "y": initY, "z": initZ},
        orientation={"x": 1.0, "y": 0.0, "z": 0.0, "w": 0.0}
    )
    return f'success\n'

def backward(distance):
    global initX
    print(distance)
    initX-= distance/100.0
    publish_custom_pose(
        position={"x": initX, "y": initY, "z": initZ},
        orientation={"x": 1.0, "y": 0.0, "z": 0.0, "w": 0.0}
    )
    return f'success\n'

@app.route('/greet', methods=['GET', 'POST'])
def greet_endpoint():
    return greet()

@app.route('/grasp', methods=['GET', 'POST'])
def grasp_endpoint():
    return grasp()

@app.route('/up', methods=['GET', 'POST'])
def up_endpoint():
    distance = request.args.get('dis', default=0, type=int)
    return up(distance)

@app.route('/down', methods=['GET', 'POST'])
def down_endpoint():
    distance = request.args.get('dis', default=0, type=int)
    return down(distance)

@app.route('/left', methods=['GET', 'POST'])
def left_endpoint():
    distance = request.args.get('dis', default=0, type=int)
    return left(distance)

@app.route('/right', methods=['GET', 'POST'])
def right_endpoint():
    distance = request.args.get('dis', default=0, type=int)
    return right(distance)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=11453, debug=True)