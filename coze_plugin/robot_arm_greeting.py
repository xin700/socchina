import requests
from typing import Any

"""
Each file needs to export a function named `handler`. This function is the entrance to the Tool.

Parameters:
args: parameters of the entry function.
args.input - input parameters, you can get test input value by args.input.xxx.
args.logger - logger instance used to print logs, injected by runtime.

Return:
The return data of the function, which should match the declared output parameters.
"""

class Input:
    def __init__(self):
        pass

class Output:
    def __init__(self, success: bool = False, message: str = None, error: str = None):
        self.success = success
        self.message = message
        self.error = error

def handler(args: Any) -> Output:
    """
    This function sends a request to the robot arm greeting service
    and returns the response message.
    """
    base_url = "http://xin700.top:11453/greet"
    try:
        resp = requests.get(base_url, timeout=10)
        resp.raise_for_status()
        # Return the raw text response as message
        message = resp.text.strip()
        return Output(success=True, message=message)
    except requests.exceptions.RequestException as e:
        return Output(success=False, message=None, error=f"请求失败: {str(e)}")
    except Exception as e:
        return Output(success=False, message=None, error=f"未知错误: {str(e)}")
