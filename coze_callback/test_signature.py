#!/usr/bin/env python3
"""
测试脚本，用于发送带签名的HTTP请求到回调服务器。
"""

import hashlib
import json
import requests
from datetime import datetime
import uuid
import argparse
import sys

def parse_args():
    parser = argparse.ArgumentParser(description='发送带签名验证的测试请求')
    parser.add_argument('-u', '--url', default='http://xin700.top:11452/callback',
                      help='服务器地址 (默认: http://localhost:11452/callback)')
    parser.add_argument('-k', '--secret-key', required=True,
                      help='签名密钥，必须与服务器使用相同的密钥')
    parser.add_argument('-t', '--event-type', default='RoomCreate',
                      help='事件类型 (默认: RoomCreate)')
    parser.add_argument('-d', '--event-data', default='{"RoomId":"room1","Timestamp":1679383924691}',
                      help='事件数据，JSON格式 (默认: {"RoomId":"room1","Timestamp":1679383924691})')
    return parser.parse_args()

def generate_signature(event_data, secret_key):
    # 提取事件数据
    data = [
        event_data['EventType'],
        event_data['EventData'],
        event_data['EventTime'],
        event_data['EventId'],
        event_data['AppId'],
        event_data['Version'],
        event_data['Nonce'],
        secret_key
    ]
    
    # 排序
    data.sort()
    
    # 拼接
    payload = "".join(data)
    
    # 计算SHA-256哈希
    return hashlib.sha256(payload.encode('utf-8')).hexdigest()

def main():
    args = parse_args()
    
    # 准备事件数据
    event = {
        "EventType": args.event_type,
        "EventData": args.event_data,
        "EventTime": datetime.now().isoformat(),
        "EventId": str(uuid.uuid4()),
        "AppId": "test_app_id",
        "Version": "2020-12-01",
        "Nonce": "test" + datetime.now().strftime('%f')
    }
    
    # 计算签名
    event["Signature"] = generate_signature(event, args.secret_key)
    
    # 打印请求信息
    print(f"发送请求到: {args.url}")
    print(f"事件类型: {event['EventType']}")
    print(f"事件ID: {event['EventId']}")
    print(f"签名: {event['Signature']}")
    
    # 发送请求
    try:
        response = requests.post(args.url, json=event, timeout=5)
        print(f"\n状态码: {response.status_code}")
        print(f"响应内容: {response.text}")
    except requests.exceptions.RequestException as e:
        print(f"请求错误: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()
