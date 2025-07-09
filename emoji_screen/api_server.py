#!/usr/bin/env python3
import http.server
import socketserver
import json
import os
import time
from urllib.parse import urlparse, parse_qs

# 配置
PORT = 11451
EMOTIONS = ['normal', 'happy', 'sad', 'angry', 'surprised', 'sleepy']
HTML_DIR = os.path.dirname(os.path.abspath(__file__))

# 全局状态
current_emotion = "normal"
emotion_change_time = time.time()
emotion_reset_time = None  # 表情重置时间

class EmotionHandler(http.server.SimpleHTTPRequestHandler):
    def do_GET(self):
        global current_emotion, emotion_change_time, emotion_reset_time
        
        # 解析URL参数
        url_components = urlparse(self.path)
        path = url_components.path
        query_params = parse_qs(url_components.query)
        
        # API路由处理
        if path == "/api/emotion" and "type" in query_params:
            emotion = query_params["type"][0]
            if emotion in EMOTIONS:
                current_emotion = emotion
                emotion_change_time = time.time()
                
                # 如果设置为正常表情，清除重置时间
                if emotion == "normal":
                    emotion_reset_time = None
                else:
                    # 否则设置3秒后重置
                    emotion_reset_time = emotion_change_time + 3
                
                # 返回JSON响应
                self.send_response(200)
                self.send_header('Content-Type', 'application/json')
                self.send_header('Access-Control-Allow-Origin', '*')
                self.end_headers()
                
                response = {
                    "success": True,
                    "emotion": emotion,
                    "message": f"表情已设置为: {emotion}",
                    "time": emotion_change_time,
                    "resetTime": emotion_reset_time
                }
                
                self.wfile.write(json.dumps(response).encode())
            else:
                # 无效的表情类型
                self.send_response(400)
                self.send_header('Content-Type', 'application/json')
                self.send_header('Access-Control-Allow-Origin', '*')
                self.end_headers()
                
                response = {
                    "success": False,
                    "message": "无效的表情类型",
                    "validEmotions": EMOTIONS
                }
                
                self.wfile.write(json.dumps(response).encode())
            return
            
        # 获取当前表情状态
        elif path == "/api/status":
            # 自动重置表情到正常状态（服务器端实现）
            current_time = time.time()
            
            # 如果当前表情不是正常，并且还没有设置重置时间，则设置重置时间
            if current_emotion != "normal" and emotion_reset_time is None:
                emotion_reset_time = emotion_change_time + 3  # 设置为从现在开始3秒后
            
            # 如果已过重置时间，则重置表情
            if emotion_reset_time is not None and current_time >= emotion_reset_time:
                current_emotion = "normal"
                emotion_reset_time = None  # 重置完成，清除重置时间
            
            self.send_response(200)
            self.send_header('Content-Type', 'application/json')
            self.send_header('Access-Control-Allow-Origin', '*')
            self.end_headers()
            
            response = {
                "currentEmotion": current_emotion,
                "lastChangeTime": emotion_change_time,
                "availableEmotions": EMOTIONS
            }
            
            self.wfile.write(json.dumps(response).encode())
            return
            
        # 静态文件处理 - 返回HTML页面但注入当前表情状态
        elif path == "/" or path == "/index.html":
            self.send_response(200)
            self.send_header('Content-Type', 'text/html')
            self.end_headers()
            
            with open(os.path.join(HTML_DIR, "index.html"), "r") as f:
                html_content = f.read()
                
                # 注入当前表情状态
                inject_script = f"""
                <script>
                    // 当页面加载时设置表情为当前服务器状态
                    window.onload = function() {{
                        // 页面加载后，设置初始表情
                        setTimeout(() => {{
                            if (window.setRobotEmotion) {{
                                window.setRobotEmotion("{current_emotion}");
                                console.log("从服务器初始化表情: {current_emotion}");
                            }}
                        }}, 200);
                        
                        // 保存当前端口号
                        const apiPort = {PORT};
                        const apiBaseUrl = `http://${{window.location.hostname}}:${{apiPort}}`;
                        
                        // 设置轮询，每3秒检查一次服务器状态
                        setInterval(async () => {{
                            try {{
                                const response = await fetch(`${{apiBaseUrl}}/api/status`);
                                const data = await response.json();
                                if (data.currentEmotion && window.setRobotEmotion && 
                                    window.currentEmotion !== data.currentEmotion && 
                                    !window.isMoving) {{ // 只在没有移动时更新
                                    window.setRobotEmotion(data.currentEmotion);
                                    console.log("表情更新为:", data.currentEmotion);
                                }}
                            }} catch (e) {{
                                console.error("获取状态失败:", e);
                            }}
                        }}, 3000);
                    }};
                </script>
                """
                
                # 在</head>标签之前插入脚本
                html_content = html_content.replace("</head>", f"{inject_script}</head>")
                
                self.wfile.write(html_content.encode())
            return
            
        # 其他静态文件请求 (CSS, JS等)
        return http.server.SimpleHTTPRequestHandler.do_GET(self)
        
    def log_message(self, format, *args):
        # 自定义日志格式
        print(f"[{time.strftime('%Y-%m-%d %H:%M:%S')}] {self.address_string()} - {format % args}")

# 启动服务器
with socketserver.TCPServer(("", PORT), EmotionHandler) as httpd:
    print(f"API服务器运行在 http://localhost:{PORT}")
    print(f"可用表情: {', '.join(EMOTIONS)}")
    print("控制表情方法:")
    print(f"curl http://localhost:{PORT}/api/emotion?type=happy")
    print(f"curl http://localhost:{PORT}/api/emotion?type=angry")
    print(f"curl http://localhost:{PORT}/api/status")
    print("按Ctrl+C退出")
    
    try:
        print("注意: 非正常表情将在3秒后自动恢复到正常状态")
        httpd.serve_forever()
    except KeyboardInterrupt:
        print("\n服务器已停止")
