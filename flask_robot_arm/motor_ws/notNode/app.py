from flask import Flask
from pose import greetPose
app = Flask(__name__)


@app.route('/greet', methods=['GET', 'POST'])
def greet_endpoint():
    greetPose()
    return "Greet done"
    
# @app.route('/move', methods=['GET', ])

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=11453, debug=True)
