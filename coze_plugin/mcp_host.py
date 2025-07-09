from runtime import Args
from typings.request_function.request_function import Input, Output

"""
Each file needs to export a function named `handler`. This function is the entrance to the Tool.

Parameters:
args: parameters of the entry function.
args.input - input parameters, you can get test input value by args.input.xxx.
args.logger - logger instance used to print logs, injected by runtime.

Remember to fill in input/output in Metadata, it helps LLM to recognize and use tool.

Return:
The return data of the function, which should match the declared output parameters.
"""
import requests
from typing import Any

# 假设 Input/Output 是自定义类型（根据实际需求调整）
class Input:
    pass  # 可添加需要的字段

class Output:
    def __init__(self, data: str = None, error: str = None):
        self.data = data
        self.error = error

def handler(args: Any) -> Output:
    host = 's7.tunnelfrp.com'
    port = 10040
    url = f"http://{host}:{port}/input?prompt={args.input.prompt}"  # 默认访问根路径

    try:
        # 发送 GET 请求（可自定义超时时间）
        response = requests.get(url, timeout=40)
        response.raise_for_status()  # 自动检查 HTTP 错误状态码（4xx/5xx）
        return Output(data=response.text.encode('utf-8').decode('unicode-escape'))
    
    except requests.exceptions.RequestException as e:
        # 捕获所有 requests 相关异常（连接错误、超时等）
        return Output(error=f"请求失败: {str(e)}")
    
    except Exception as e:
        # 处理其他未知异常
        return Output(error=f"未知错误: {str(e)}")