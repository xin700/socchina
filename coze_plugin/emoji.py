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
    def __init__(self, type: str):
        self.type = type

class Output:
    def __init__(self, success: bool = False, message: str = None, error: str = None):
        self.success = success
        self.message = message
        self.error = error

def handler(args: Any) -> Output:
    """
    Args.input.type: 表情类型 (如 sad, happy, angry, surprised)
    """
    base_url = "http://43.156.242.14:11451/api/emotion"
    params = {"type": getattr(args.input, 'type', None)}
    try:
        resp = requests.get(base_url, params=params, timeout=10)
        resp.raise_for_status()
        data = resp.json()
        success = data.get("success", False)
        message = data.get("message", "无返回信息")
        return Output(success=success, message=message)
    except requests.exceptions.RequestException as e:
        return Output(success=False, message=None, error=f"请求失败: {str(e)}")
    except Exception as e:
        return Output(success=False, message=None, error=f"未知错误: {str(e)}")