def greetPose():
    from ctrlClass import StepperMotorController
    import time
    joint=[]*6
    joint.append(StepperMotorController(can_channel='can0', motor_id=1, offset=40.5, minn=2.5, maxx=78.5, dir=1, decRate=50))
    joint.append(StepperMotorController(can_channel='can0', motor_id=2, offset=0, minn=0, maxx=150, dir=1, decRate=50))
    joint.append(StepperMotorController(can_channel='can0', motor_id=3, offset=0, minn=0, maxx=180, dir=-1, decRate=50))
    joint.append(StepperMotorController(can_channel='can0', motor_id=4, offset=0, minn=0, maxx=335, dir=1, decRate=50))
    joint.append(StepperMotorController(can_channel='can0', motor_id=5, offset=0, minn=-90, maxx=90, dir=-1, decRate=3.166667))
    joint.append(StepperMotorController(can_channel='can0', motor_id=6, offset=0, minn=-180, maxx=180, dir=1, decRate=1))
    # time.sleep(4)
    point=[[5.0983694795698025, 41.725066029316714, 90.03560637522808, 72.13557461848507, -28.643692208945573, -63.2377692643779],[-24.715979679484928, 28.235466132391082, 72.23480422348729, 45.87336714727658, -5.553941912567986, -28.88494779588235],[24.48608252877673, 40.69247678028678, 86.47427383281212, 96.98384405030876, -18.532201001201404, -74.42386233648396]]

    k=0
    for i in range(5):
        joint[i].position(point[k][i], 2)
    time.sleep(1)
    k=1 
    for i in range(5):
        joint[i].position(point[k][i], 1.5)
    time.sleep(0.5)
    k=2
    for i in range(5):
        joint[i].position(point[0][i], 1.5)
    time.sleep(0.5)
    k=1 
    for i in range(5):
        joint[i].position(point[k][i], 1.5)
    time.sleep(0.5)
    k=2
    for i in range(5):
        joint[i].position(point[0][i], 1.5)
    time.sleep(0.5)
    k=1
    # for i in range(5):
    #     joint[i].position(point[k][i], 2)
    # time.sleep(0.7)
    # k=2
    # for i in range(5):
    #     joint[i].position(point[0][i], 2)
    # time.sleep(0.7)
    # k=0
    for i in range(5):
        joint[i].position(point[k][i], 1.5)
    time.sleep(0.5)
    for i in range(5):
        joint[i].position(0, 3)
if __name__ == '__main__':
    greetPose()