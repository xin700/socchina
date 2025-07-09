const express = require('express');
const cors = require('cors');
const path = require('path');
const app = express();
const PORT = 3000;

// 支持的表情类型
const VALID_EMOTIONS = ['normal', 'happy', 'sad', 'angry', 'surprised', 'sleepy'];

// 当前表情状态
let currentEmotion = 'normal';
let lastEmotionChangeTime = Date.now();
let autoResetTimeout = null;

// 中间件
app.use(express.json());
app.use(cors());
app.use(express.static(path.join(__dirname)));

// API接口 - 设置表情
app.get('/api/emotion/:emotion', (req, res) => {
    const emotion = req.params.emotion;
    
    if (!VALID_EMOTIONS.includes(emotion)) {
        return res.status(400).json({
            success: false,
            message: '无效的表情类型',
            validEmotions: VALID_EMOTIONS
        });
    }
    
    // 更新当前表情
    currentEmotion = emotion;
    lastEmotionChangeTime = Date.now();
    
    // 清除之前的自动重置计时器
    if (autoResetTimeout) {
        clearTimeout(autoResetTimeout);
    }
    
    // 如果不是normal状态，设置3秒后自动恢复
    if (emotion !== 'normal') {
        autoResetTimeout = setTimeout(() => {
            currentEmotion = 'normal';
            console.log('表情自动重置为normal');
        }, 3000);
    }
    
    console.log(`表情已设置为: ${emotion}`);
    
    return res.json({
        success: true,
        emotion: emotion,
        message: `表情已设置为: ${emotion}`,
        resetTime: emotion !== 'normal' ? 3000 : null
    });
});

// API接口 - 获取当前表情状态
app.get('/api/status', (req, res) => {
    res.json({
        currentEmotion: currentEmotion,
        lastChangeTime: lastEmotionChangeTime,
        availableEmotions: VALID_EMOTIONS
    });
});

// 主页面 - 修改为根据请求参数渲染不同初始表情的页面
app.get('/', (req, res) => {
    const emotion = req.query.emotion;
    
    // 如果有emotion参数且有效，更新当前状态
    if (emotion && VALID_EMOTIONS.includes(emotion)) {
        currentEmotion = emotion;
        lastEmotionChangeTime = Date.now();
        
        // 重置自动计时器
        if (autoResetTimeout) {
            clearTimeout(autoResetTimeout);
        }
        
        if (emotion !== 'normal') {
            autoResetTimeout = setTimeout(() => {
                currentEmotion = 'normal';
                console.log('表情自动重置为normal');
            }, 3000);
        }
    }
    
    // 读取并发送HTML文件
    const htmlPath = path.join(__dirname, 'index.html');
    res.sendFile(htmlPath);
});

// 启动服务器
app.listen(PORT, () => {
    console.log(`表情API服务器运行在 http://localhost:${PORT}`);
    console.log('可用表情:', VALID_EMOTIONS.join(', '));
    console.log('控制表情示例:');
    console.log(`curl http://localhost:${PORT}/api/emotion/happy`);
});
