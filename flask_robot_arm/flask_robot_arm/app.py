from flask import Flask, request

app = Flask(__name__)

def greet():
    pass
    return f'success\n'
    
def grasp():
    pass
    return f'success\n'

def up(distance):
    print(distance)
    pass
    return f'success\n'

def down(distance):
    print(distance)
    pass
    return f'success\n'

def left(distance):
    print(distance)
    pass
    return f'success\n'

def right(distance):
    print(distance)
    pass
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