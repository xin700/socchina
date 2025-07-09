# 系统麦克风控制

这个模块提供了控制macOS系统麦克风输入状态的功能。

## 功能特点

- 启用麦克风输入
- 禁用麦克风输入 
- 切换麦克风状态
- 查询当前麦克风状态

## 文件结构

- `mic_control.py` - 核心功能模块，提供麦克风控制的类和方法
- `cli.py` - 命令行界面，方便用户使用命令行控制麦克风

## 使用方法

### 作为模块使用

```python
from mic_control import MicrophoneController

# 创建控制器实例
mic = MicrophoneController()

# 检查当前状态
state = mic.get_state()
print(f"麦克风状态: {'启用' if state else '禁用'}")

# 禁用麦克风
mic.disable_microphone()

# 启用麦克风
mic.enable_microphone()

# 切换状态
mic.toggle_microphone()
```

### 命令行使用

```bash
# 查看当前状态
python cli.py status

# 启用麦克风
python cli.py enable

# 禁用麦克风
python cli.py disable

# 切换麦克风状态
python cli.py toggle
```

## 实现原理

该模块使用macOS的AppleScript功能通过命令行来控制系统输入音量，从而实现对麦克风的控制。
```
