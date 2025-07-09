# Coze Callback HTTP Server

一个简单的HTTP服务器，用于监听11452端口上的HTTP请求，并打印和保存这些请求。支持签名验证以确保回调请求的真实性。

## 功能特点

- 监听11452端口的HTTP请求（可配置）
- 支持GET、POST、PUT、DELETE等HTTP方法
- 自动记录请求日志并打印到控制台
- 将完整请求数据（包括头信息和请求体）保存到JSON文件中
- **支持SHA-256签名验证**，确保回调请求的真实性和完整性
- 多线程处理请求，支持并发
- 模块化设计，易于扩展
- 命令行参数配置，支持自定义端口、密钥和日志级别

## 目录结构

```
coze_callback/
│— server.py          # 主程序入口
│— http_server.py     # HTTP服务器实现
│— request_handler.py # 请求处理器实现
│— signature_verifier.py # 签名验证实现
└— data/              # 保存的请求数据目录
```

## 使用方法

### 启动服务器

基本用法：

```bash
cd coze_callback
python server.py
```

使用签名验证功能：

```bash
# 直接指定密钥
python server.py --secret-key "your_secret_key"

# 或从文件读取密钥
echo "your_secret_key" > secret.key
python server.py --secret-key-file secret.key
```

更多启动选项：

```bash
# 自定义端口和数据保存目录
python server.py --port 8080 --data-dir callback_data

# 设置更详细的日志级别
python server.py --log-level DEBUG

# 查看所有可用选项
python server.py --help
```

### 测试服务器

可以使用curl、Postman或其他HTTP客户端工具发送请求到服务器：

```bash
# 发送GET请求
curl http://localhost:11452/test

# 发送POST请求
curl -X POST -d '{"message": "Hello World"}' http://localhost:11452/api/data
```

### 测试签名验证

使用以下Python脚本生成带有正确签名的请求：

```python
import hashlib
import json
import requests
from datetime import datetime
import uuid

# 配置
secret_key = "your_secret_key"  # 与服务器使用相同的密钥
url = "http://localhost:11452/callback"

# 创建事件数据
event = {
    "EventType": "RoomCreate",
    "EventData": json.dumps({"RoomId": "room1", "Timestamp": 1679383924691}),
    "EventTime": datetime.now().isoformat(),
    "EventId": str(uuid.uuid4()),
    "AppId": "appId",
    "Version": "2020-12-01",
    "Nonce": "aaBc"
}

# 计算签名
data = [
    event["EventType"],
    event["EventData"],
    event["EventTime"],
    event["EventId"],
    event["AppId"],
    event["Version"],
    event["Nonce"],
    secret_key
]
data.sort()
payload = "".join(data)
signature = hashlib.sha256(payload.encode()).hexdigest()

# 添加签名到事件数据
event["Signature"] = signature

# 发送请求
response = requests.post(url, json=event)
print(f"Status Code: {response.status_code}")
print(f"Response: {response.text}")
```

### 查看保存的请求

所有请求将保存在`data`目录下（或自定义目录），文件名格式为`YYYYMMDD_HHMMSS_fff_METHOD.json`。

每个保存的文件中包含完整的请求信息，包括：
- HTTP 方法、路径和头信息
- 请求体内容
- 签名验证结果（如果启用了签名验证）

## 命令行选项

服务器支持以下命令行选项：

| 选项 | 说明 |
|------|------|
| `-p, --port PORT` | 监听端口（默认：11452） |
| `-s, --secret-key KEY` | 签名验证的密钥 |
| `-k, --secret-key-file FILE` | 包含签名密钥的文件路径 |
| `-d, --data-dir DIR` | 数据保存目录（默认：data） |
| `-l, --log-level LEVEL` | 日志级别：DEBUG, INFO, WARNING, ERROR, CRITICAL（默认：INFO） |
| `--help` | 显示帮助信息 |
