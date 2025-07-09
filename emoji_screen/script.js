// 表情定义模块
const EmotionDefinitions = {
    // 定义各种表情的眼睛参数
    normal: {
        eyes: {
            width: '45px',
            height: '70px',
            borderRadius: '50%',
            transform: 'rotate(0deg)',
            background: '#333',
            top: '0px',
            left: '0px'
        },
        eyebrows: {
            visible: false,
            leftTransform: 'rotate(0deg)',
            rightTransform: 'rotate(0deg)',
            top: '0px'
        },
        emojis: {
            visible: []
        }
    },
    happy: {
        eyes: {
            width: '45px',
            height: '15px',
            borderRadius: '50px 50px 0 0',
            transform: 'rotate(0deg) translateY(15px)',
            background: '#333',
            top: '-15px',
            left: '0px'
        },
        eyebrows: {
            visible: false,
            leftTransform: 'rotate(-10deg)',
            rightTransform: 'rotate(10deg)',
            top: '-5px'
        },
        emojis: {
            visible: ['heart']
        }
    },
    sad: {
        eyes: {
            width: '45px',
            height: '15px',
            borderRadius: '0 0 50px 50px',
            transform: 'rotate(0deg) translateY(-15px)',
            background: '#333',
            top: '15px',
            left: '0px'
        },
        eyebrows: {
            visible: true,
            leftTransform: 'rotate(20deg) translateY(5px)',
            rightTransform: 'rotate(-20deg) translateY(5px)',
            top: '-5px'
        },
        emojis: {
            visible: ['sweat-drop']
        }
    },
    angry: {
        eyes: {
            width: '50px',
            height: '25px',
            borderRadius: '50%',
            transform: 'rotate(-30deg)',
            background: '#ff3333',
            top: '0px',
            left: '0px'
        },
        eyebrows: {
            visible: true,
            leftTransform: 'rotate(-30deg) translateY(-5px)',
            rightTransform: 'rotate(30deg) translateY(-5px)',
            top: '-10px'
        },
        emojis: {
            visible: ['angry-mark']
        }
    },
    surprised: {
        eyes: {
            width: '60px',
            height: '70px',
            borderRadius: '50%',
            transform: 'scale(1.1)',
            background: '#333',
            top: '0px',
            left: '0px'
        },
        eyebrows: {
            visible: true,
            leftTransform: 'rotate(-5deg) translateY(-10px)',
            rightTransform: 'rotate(5deg) translateY(-10px)',
            top: '-5px'
        },
        emojis: {
            visible: ['exclamation-mark', 'question-mark']
        }
    },
    sleepy: {
        eyes: {
            width: '45px',
            height: '5px',
            borderRadius: '50%',
            transform: 'rotate(0deg)',
            background: '#333',
            top: '0px',
            left: '0px'
        },
        eyebrows: {
            visible: false,
            leftTransform: 'rotate(0deg)',
            rightTransform: 'rotate(0deg)',
            top: '0px'
        },
        emojis: {
            visible: ['zzz']
        }
    }
};

// 表情控制器模块
const EmotionController = {
    // 当前表情
    currentEmotion: 'normal',
    
    // DOM元素
    leftEye: null,
    rightEye: null,
    leftEyebrow: null,
    rightEyebrow: null,
    emojis: {},
    
    // 状态变量
    isMoving: false,          // 眼球是否正在移动
    isBlinking: false,        // 眼球是否正在眨眼
    lastBlinkTime: 0,         // 上次眨眼时间
    
    // 定时器
    blinkInterval: null,      // 眨眼定时器
    lookInterval: null,       // 合成移动定时器
    directionLookTimeout: null,  // 定向移动定时器
    randomLookTimeout: null,     // 随机移动定时器
    autoResetTimeout: null,      // 表情自动重置定时器
    
    // 初始化
    init() {
        // 获取眼睛元素
        this.leftEye = document.querySelector('.left-eye');
        this.rightEye = document.querySelector('.right-eye');
        
        // 获取眉毛元素
        this.leftEyebrow = document.querySelector('.left-eyebrow');
        this.rightEyebrow = document.querySelector('.right-eyebrow');
        
        // 获取所有emoji元素
        document.querySelectorAll('.emoji').forEach(emoji => {
            const className = Array.from(emoji.classList).find(cls => cls !== 'emoji');
            if (className) {
                this.emojis[className] = emoji;
            }
        });
        
        // 设置初始表情
        this.setEmotion('normal');
        
        // 启动普通状态下的眼球移动
        this.startEyeMovement();
        
        // 将API函数暴露到全局，以便通过URL参数调用
        window.setRobotEmotion = (emotion) => {
            // 判断表情是否有效
            if (!EmotionDefinitions[emotion]) {
                console.error('无效的表情类型:', emotion);
                this.showApiResponse({
                    success: false,
                    message: '无效的表情类型',
                    validEmotions: Object.keys(EmotionDefinitions)
                });
                return false;
            }
            
            // 设置表情
            this.setEmotion(emotion);
            
            // 显示API响应
            this.showApiResponse({
                success: true,
                emotion: emotion,
                message: `表情已设置为: ${emotion}`
            });
            
            return true;
        };
    },
    
    // 显示API响应（用于curl请求）
    showApiResponse(data) {
        // 获取响应区域
        const responseArea = document.getElementById('api-response');
        const responseContent = document.getElementById('response-content');
        
        if (responseArea && responseContent) {
            // 如果是通过curl访问，则显示响应内容
            const userAgent = navigator.userAgent.toLowerCase();
            const isCurl = userAgent.includes('curl') || this.isCurlRequest();
            
            if (isCurl) {
                // 将响应转换为JSON字符串
                responseContent.textContent = JSON.stringify(data, null, 2);
                responseArea.style.display = 'block';
                
                // 隐藏机器人脸部，只显示API响应
                document.querySelector('.robot-face').style.display = 'none';
                document.body.style.display = 'block';
                document.body.style.backgroundColor = 'white';
                document.body.style.color = 'black';
                document.body.style.padding = '20px';
                document.body.style.fontFamily = 'monospace';
            }
        }
    },
    
    // 检测是否是curl请求
    isCurlRequest() {
        // 通过URL参数检测是否是API模式
        const urlParams = new URLSearchParams(window.location.search);
        return urlParams.has('api') || urlParams.has('json');
    },
    
    // 设置表情
    setEmotion(emotion) {
        if (!EmotionDefinitions[emotion]) {
            console.error('未定义的表情:', emotion);
            return;
        }
        
        // 清除所有定时器
        if (this.lookInterval) {
            clearTimeout(this.lookInterval);
            this.lookInterval = null;
        }
        
        if (this.directionLookTimeout) {
            clearTimeout(this.directionLookTimeout);
            this.directionLookTimeout = null;
        }
        
        if (this.randomLookTimeout) {
            clearTimeout(this.randomLookTimeout);
            this.randomLookTimeout = null;
        }
        
        // 清除自动重置计时器
        if (this.autoResetTimeout) {
            clearTimeout(this.autoResetTimeout);
            this.autoResetTimeout = null;
        }
        
        // 清除眨眼计时器
        if (this.blinkInterval) {
            clearTimeout(this.blinkInterval);
            this.blinkInterval = null;
        }
        
        // 重置移动状态
        this.isMoving = false;
        
        this.currentEmotion = emotion;
        const emotionConfig = EmotionDefinitions[emotion];
        
        // 重置过渡效果
        this.leftEye.style.transition = 'all 0.3s ease';
        this.rightEye.style.transition = 'all 0.3s ease';
        
        // 应用眼睛样式
        this.applyStyle(this.leftEye, emotionConfig.eyes);
        this.applyStyle(this.rightEye, emotionConfig.eyes);
        
        // 应用眉毛样式
        this.applyEyebrowsStyle(emotionConfig.eyebrows);
        
        // 应用表情符号
        this.updateEmojis(emotionConfig.emojis.visible);
        
        // 特殊表情的额外处理
        this.handleSpecialEmotions(emotion);
        
        // 如果是普通状态，启动眼球移动
        if (emotion === 'normal') {
            this.startEyeMovement();
        } else {
            // 如果不是普通状态，设置3秒后自动恢复
            this.scheduleResetToNormal();
        }
        
        console.log(`表情已设置为: ${emotion}`);
    },
    
    // 应用样式到元素
    applyStyle(element, style) {
        for (const [property, value] of Object.entries(style)) {
            element.style[property] = value;
        }
    },
    
    // 应用眉毛样式
    applyEyebrowsStyle(eyebrowsConfig) {
        // 设置可见性
        this.leftEyebrow.style.opacity = eyebrowsConfig.visible ? '1' : '0';
        this.rightEyebrow.style.opacity = eyebrowsConfig.visible ? '1' : '0';
        
        // 设置位置和变形
        this.leftEyebrow.style.transform = eyebrowsConfig.leftTransform;
        this.rightEyebrow.style.transform = eyebrowsConfig.rightTransform;
        this.leftEyebrow.style.top = eyebrowsConfig.top;
        this.rightEyebrow.style.top = eyebrowsConfig.top;
    },
    
    // 更新表情符号
    updateEmojis(visibleEmojis) {
        // 先隐藏所有表情
        Object.values(this.emojis).forEach(emoji => {
            emoji.style.opacity = '0';
        });
        
        // 显示需要的表情
        visibleEmojis.forEach(emojiClass => {
            if (this.emojis[emojiClass]) {
                this.emojis[emojiClass].style.opacity = '1';
            }
        });
    },
    
    // 处理特殊表情
    handleSpecialEmotions(emotion) {
        // 重置所有特殊效果
        clearInterval(this.blinkInterval);
        
        // 根据表情类型添加特殊效果
        switch(emotion) {
            case 'sleepy':
                // 困倦状态下的眨眼效果
                this.startBlinking();
                break;
            case 'surprised':
                // 震惊效果
                this.applyPulseEffect();
                break;
            case 'angry':
                // 生气效果 - 轻微抖动
                this.applyShakeEffect();
                break;
        }
    },
    
    // 开始眨眼效果 (用于困倦表情)
    startBlinking() {
        let isClosed = false;
        this.blinkInterval = setInterval(() => {
            if (isClosed) {
                // 恢复睡眼状态
                const sleepyEyes = EmotionDefinitions.sleepy.eyes;
                this.applyStyle(this.leftEye, sleepyEyes);
                this.applyStyle(this.rightEye, sleepyEyes);
            } else {
                // 完全闭眼
                this.applyStyle(this.leftEye, {
                    width: '45px',
                    height: '0px',
                    borderRadius: '0'
                });
                this.applyStyle(this.rightEye, {
                    width: '45px',
                    height: '0px',
                    borderRadius: '0'
                });
            }
            isClosed = !isClosed;
        }, 2000);
    },
    
    // 应用脉冲效果 (用于惊讶表情)
    applyPulseEffect() {
        // 添加简单的缩放动画
        this.leftEye.style.animation = 'pulse 0.5s ease-in-out';
        this.rightEye.style.animation = 'pulse 0.5s ease-in-out';
        
        // 移除后再添加，以便重复触发
        setTimeout(() => {
            this.leftEye.style.animation = '';
            this.rightEye.style.animation = '';
        }, 500);
    },
    
    // 应用抖动效果 (用于生气表情)
    applyShakeEffect() {
        // 添加抖动动画
        document.querySelector('.robot-face').style.animation = 'shake 0.5s ease-in-out';
        
        // 移除后再添加，以便重复触发
        setTimeout(() => {
            document.querySelector('.robot-face').style.animation = '';
            
            // 每2秒重复一次抖动
            const shakeInterval = setInterval(() => {
                // 如果不再是生气状态，则停止抖动
                if (this.currentEmotion !== 'angry') {
                    clearInterval(shakeInterval);
                    return;
                }
                
                document.querySelector('.robot-face').style.animation = 'shake 0.5s ease-in-out';
                setTimeout(() => {
                    document.querySelector('.robot-face').style.animation = '';
                }, 500);
            }, 2000);
        }, 500);
    },
    
    // 开始眼球移动 (用于普通状态)
    startEyeMovement() {
        // 清除之前可能存在的定时器
        if (this.lookInterval) {
            clearTimeout(this.lookInterval);
            this.lookInterval = null;
        }
        if (this.blinkInterval) {
            clearTimeout(this.blinkInterval);
            this.blinkInterval = null;
        }
        
        // 获取当前眼睛状态的引用
        const originalEyeStyle = { ...EmotionDefinitions.normal.eyes };
        
        // 眼球眨眼函数 - 防止快速重复眨眼
        const blink = () => {
            // 如果当前正在眨眼，则不执行
            if (this.isBlinking) {
                console.log('眨眼: 已进行中，跳过');
                return false;
            }
            
            // 标记当前正在眨眼
            this.isBlinking = true;
            
            // 保存当前眼睛样式，用于恢复
            const currentLeftEyeHeight = this.leftEye.style.height;
            const currentLeftEyeBorderRadius = this.leftEye.style.borderRadius;
            const currentRightEyeHeight = this.rightEye.style.height;
            const currentRightEyeBorderRadius = this.rightEye.style.borderRadius;
            
            // 设置零值过渡时间，确保眨眼动作迅速
            this.leftEye.style.transition = 'height 0.1s ease, border-radius 0.1s ease';
            this.rightEye.style.transition = 'height 0.1s ease, border-radius 0.1s ease';
            
            console.log('眨眼: 开始眨眼');
            
            // 闭眼
            this.applyStyle(this.leftEye, {
                height: '0px',
                borderRadius: '0'
            });
            this.applyStyle(this.rightEye, {
                height: '0px',
                borderRadius: '0'
            });
            
            // 睁眼 - 确保使用保存的原始值恢复
            setTimeout(() => {
                // 如果此时已经不是normal状态，不执行恢复操作
                if (this.currentEmotion !== 'normal') {
                    this.isBlinking = false;
                    return;
                }
                
                // 恢复到原始高度和边框
                this.leftEye.style.height = currentLeftEyeHeight || originalEyeStyle.height;
                this.leftEye.style.borderRadius = currentLeftEyeBorderRadius || originalEyeStyle.borderRadius;
                this.rightEye.style.height = currentRightEyeHeight || originalEyeStyle.height;
                this.rightEye.style.borderRadius = currentRightEyeBorderRadius || originalEyeStyle.borderRadius;
                
                // 恢复正常过渡时间
                setTimeout(() => {
                    this.leftEye.style.transition = 'all 0.3s ease';
                    this.rightEye.style.transition = 'all 0.3s ease';
                    
                    // 标记眨眼完成
                    this.isBlinking = false;
                    console.log('眨眼: 完成');
                }, 200);
            }, 200);
            
            return true;
        };
        
        // 随机生成眨眼间隔时间 (20-30秒)
        const getRandomBlinkInterval = () => {
            return 20000 + Math.random() * 10000; // 20000-30000毫秒
        };
        
        // 设置眨眼定时器，使用随机间隔，严格控制频率
        const scheduleNextBlink = () => {
            // 先清除任何现有的眨眼计时器
            if (this.blinkInterval) {
                clearTimeout(this.blinkInterval);
                this.blinkInterval = null;
            }
            
            // 计算距离上次眨眼的时间
            const now = Date.now();
            const timeSinceLastBlink = now - this.lastBlinkTime;
            
            // 如果距离上次眨眼不足10秒，则延长间隔
            let nextBlinkInterval;
            if (timeSinceLastBlink < 10000) {
                // 间隔不足10秒，补充不足的时间再加10-15秒
                nextBlinkInterval = (10000 - timeSinceLastBlink) + 10000 + Math.random() * 5000;
                console.log(`眨眼: 距离上次眨眼只有${timeSinceLastBlink/1000}秒，延长至${nextBlinkInterval/1000}秒后`);
            } else {
                // 够长了，使用正常间隔
                nextBlinkInterval = getRandomBlinkInterval();
                console.log(`眨眼: 计划${nextBlinkInterval/1000}秒后眨眼`);
            }
            
            // 设置下一次眨眼的定时器
            this.blinkInterval = setTimeout(() => {
                // 只在正常状态且不在移动时眨眼
                if (this.currentEmotion === 'normal' && !this.isMoving && !this.isBlinking) {
                    // 执行眨眼
                    const success = blink();
                    
                    // 如果眨眼成功，更新上次眨眼时间
                    if (success) {
                        this.lastBlinkTime = Date.now();
                    }
                }
                
                // 安排下一次眨眼
                scheduleNextBlink();
            }, nextBlinkInterval);
        };
        
        // 初始化上次眨眼时间
        this.lastBlinkTime = Date.now();
        
        // 开始眨眼计划
        scheduleNextBlink();
        
        // 定向眼球移动函数 (每15-20秒向左或向右看一次)
        const lookDirectionally = () => {
            // 清除所有可能的定时器
            if (this.directionLookTimeout) {
                clearTimeout(this.directionLookTimeout);
                this.directionLookTimeout = null;
            }
            
            // 如果不是normal状态，暂时不执行定向移动
            if (this.currentEmotion !== 'normal') {
                // 重新安排定向移动
                const nextInterval = 15000 + Math.random() * 5000; // 15-20秒
                this.directionLookTimeout = setTimeout(lookDirectionally, nextInterval);
                return;
            }
            
            // 检查是否在移动中
            if (this.isMoving) {
                // 如果正在移动，延迟并重新尝试
                this.directionLookTimeout = setTimeout(lookDirectionally, 5000);
                return;
            }
            
            // 标记当前正在移动
            this.isMoving = true;
            
            // 中心位置
            const center = { x: 0, y: 0 };
            // 随机选择看左边或者看右边
            const direction = Math.random() > 0.5 ? 'left' : 'right';
            
            // 目标位置
            const targetX = direction === 'left' ? -12 : 12; // 水平移动范围
            
            // 缓慢看向新位置
            this.leftEye.style.transition = 'all 1.2s ease-out';
            this.rightEye.style.transition = 'all 1.2s ease-out';
            
            // 设置眼睛位置 - 只改变水平方向，保持垂直方向不变
            this.leftEye.style.left = `${targetX}px`;
            this.rightEye.style.left = `${targetX}px`;
            
            // 停留一段时间 (2-3秒)
            const stayDuration = 2000 + Math.random() * 1000;
            
            console.log(`眼球移动: 看向${direction === 'left' ? '左边' : '右边'}, 停留${stayDuration/1000}秒`);
            
            // 一段时间后回到中心位置
            setTimeout(() => {
                // 如果此时已经不是normal状态，不执行恢复操作
                if (this.currentEmotion !== 'normal') {
                    this.isMoving = false;
                    return;
                }
                
                // 缓慢回到中心位置
                this.leftEye.style.transition = 'all 1s ease-in-out';
                this.rightEye.style.transition = 'all 1s ease-in-out';
                
                // 回到中心位置
                this.leftEye.style.left = `${center.x}px`;
                this.rightEye.style.left = `${center.x}px`;
                
                console.log('眼球移动: 回到中心位置');
                
                // 在过渡完成后重置移动标志
                setTimeout(() => {
                    this.isMoving = false;
                }, 1200); // 稍微超过过渡时间确保过渡完成
                
            }, stayDuration);
            
            // 设置下一次看左右的时间 (15-20秒)
            const nextInterval = 15000 + Math.random() * 5000;
            this.directionLookTimeout = setTimeout(lookDirectionally, nextInterval + stayDuration + 1200);
        };
        
        // 随机微小眼球移动 (极少使用，仅通过定向移动控制)
        const lookRandomly = () => {
            // 先清除所有可能的随机定时器
            if (this.randomLookTimeout) {
                clearTimeout(this.randomLookTimeout);
                this.randomLookTimeout = null;
            }
            
            // 如果不是normal状态或者正在定向移动，则延迟重试
            if (this.currentEmotion !== 'normal' || this.isMoving) {
                // 很长时间后再尝试
                const delay = 25000 + Math.random() * 10000; // 25-35秒
                this.randomLookTimeout = setTimeout(lookRandomly, delay);
                return;
            }
            
            // 非常低概率决定是否执行微小移动
            if (Math.random() > 0.3) { // 只有30%的几率执行移动
                // 如果不移动，延期再试
                const delay = 20000 + Math.random() * 10000; // 20-30秒
                this.randomLookTimeout = setTimeout(lookRandomly, delay);
                return;
            }
            
            // 标记当前正在移动
            this.isMoving = true;
            
            // 中心位置
            const center = { x: 0, y: 0 };
            
            // 生成微小随机位置
            const maxRange = 3; // 更小的范围
            const randomDirection = () => (Math.random() * 2 - 1);
            const x = Math.round(randomDirection() * maxRange);
            const y = Math.round(randomDirection() * maxRange * 0.2); // 高度很小
            
            // 缓慢看向新位置
            this.leftEye.style.transition = 'all 1.5s ease-out';
            this.rightEye.style.transition = 'all 1.5s ease-out';
            
            // 设置眼睛位置
            this.leftEye.style.left = `${x}px`;
            this.leftEye.style.top = `${y}px`;
            this.rightEye.style.left = `${x}px`;
            this.rightEye.style.top = `${y}px`;
            
            // 停留较长时间 (3-4秒)
            const stayDuration = 3000 + Math.random() * 1000;
            
            // 一段时间后回到中心位置
            setTimeout(() => {
                // 如果此时已经不是normal状态，不执行恢复操作
                if (this.currentEmotion !== 'normal') {
                    this.isMoving = false;
                    return;
                }
                
                // 缓慢回到中心位置
                this.leftEye.style.transition = 'all 1.2s ease-in-out';
                this.rightEye.style.transition = 'all 1.2s ease-in-out';
                
                // 回到中心位置
                this.leftEye.style.left = `${center.x}px`;
                this.leftEye.style.top = `${center.y}px`;
                this.rightEye.style.left = `${center.x}px`;
                this.rightEye.style.top = `${center.y}px`;
                
                // 在过渡完成后重置移动标志
                setTimeout(() => {
                    this.isMoving = false;
                }, 1300);
                
            }, stayDuration);
            
            // 设置下一次随机微小移动的时间 - 非常少发生
            const nextInterval = 30000 + Math.random() * 15000; // 30-45秒
            this.randomLookTimeout = setTimeout(lookRandomly, nextInterval + stayDuration + 1300);
        };
        
        // 先安全地清除所有可能存在的定时器
        this.isMoving = false;
        this.isBlinking = false;
        
        if (this.directionLookTimeout) {
            clearTimeout(this.directionLookTimeout);
            this.directionLookTimeout = null;
        }
        
        if (this.randomLookTimeout) {
            clearTimeout(this.randomLookTimeout);
            this.randomLookTimeout = null;
        }
        
        if (this.blinkInterval) {
            clearTimeout(this.blinkInterval);
            this.blinkInterval = null;
        }
        
        // 首次加载时初始化上次眨眼时间
        this.lastBlinkTime = Date.now();
        
        // 稍等一会，等框架稳定后再启动计时器
        setTimeout(() => {
            // 启动定向眼球移动(左右看) - 主要移动方式，15秒后开始
            console.log('初始化: 设置主要定向眼球移动事件');
            this.directionLookTimeout = setTimeout(() => {
                lookDirectionally();
            }, 15000);
            
            // 如果需要启动随机微小移动，可以在一分钟后尝试
            console.log('初始化: 设置辅助微小移动事件');
            this.randomLookTimeout = setTimeout(() => {
                lookRandomly();
            }, 60000);  // 延长到一分钟后
            
            // 启动眨眼计划
            console.log('初始化: 设置眨眼进程');
            scheduleNextBlink();
            
            // 先执行一次眨眼，确保眨眼机制能正常工作，30秒后执行
            this.firstBlinkTimeout = setTimeout(() => {
                if (!this.isMoving && !this.isBlinking) {
                    console.log('初始化: 执行第一次眨眼');
                    if (blink()) {
                        this.lastBlinkTime = Date.now();
                    }
                }
            }, 30000);
        }, 5000); // 等7秒后初始化所有动作
    },
    
    // 设置3秒后恢复到普通状态
    scheduleResetToNormal() {
        if (this.autoResetTimeout) {
            clearTimeout(this.autoResetTimeout);
        }
        
        this.autoResetTimeout = setTimeout(() => {
            this.setEmotion('normal');
        }, 3000);
    }
};

// 添加动画样式
const addAnimationStyles = () => {
    const styleSheet = document.createElement('style');
    styleSheet.textContent = `
        @keyframes pulse {
            0% { transform: scale(1); }
            50% { transform: scale(1.2); }
            100% { transform: scale(1); }
        }
        
        @keyframes shake {
            0% { transform: translate(0, 0) rotate(0deg); }
            10% { transform: translate(-2px, -2px) rotate(-1deg); }
            20% { transform: translate(2px, -2px) rotate(1deg); }
            30% { transform: translate(-2px, 2px) rotate(0deg); }
            40% { transform: translate(2px, 2px) rotate(1deg); }
            50% { transform: translate(-2px, -2px) rotate(-1deg); }
            60% { transform: translate(2px, -2px) rotate(0deg); }
            70% { transform: translate(-2px, 2px) rotate(-1deg); }
            80% { transform: translate(-2px, -2px) rotate(1deg); }
            90% { transform: translate(2px, -2px) rotate(0deg); }
            100% { transform: translate(0, 0) rotate(0deg); }
        }
        
        .eye {
            position: relative;
            transition: all 0.2s ease;
        }
        
        .eyebrow {
            transition: all 0.3s ease;
        }
    `;
    document.head.appendChild(styleSheet);
};

// 当页面加载完成时初始化
document.addEventListener('DOMContentLoaded', () => {
    addAnimationStyles();
    EmotionController.init();
    
    console.log('机器人表情API使用说明:');
    console.log('通过curl请求控制表情:');
    console.log('curl "http://localhost:8081/?emotion=happy&json=1"');
    console.log('curl "http://localhost:8081/?emotion=angry&json=1"');
    console.log('curl "http://localhost:8081/?emotion=sad&json=1"');
    console.log('curl "http://localhost:8081/?emotion=surprised&json=1"');
    console.log('curl "http://localhost:8081/?emotion=sleepy&json=1"');
    console.log('curl "http://localhost:8081/?emotion=normal&json=1"');
    console.log('可用表情:', Object.keys(EmotionDefinitions).join(', '));
});
