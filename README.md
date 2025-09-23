# cyclone-HTTPServer

一个基于muduo库的轻量级HTTP服务框架，基于此架构写了一个AI聊天和图像识别的小应用。

## 项目介绍

cyclone-HTTPServer是一个高性能的C++ HTTP服务器框架，基于muduo网络库构建，提供了完整的Web服务能力。

### 主要特性

- 🚀 **高性能**：基于muduo网络库，支持高并发连接
- 🤖 **AI集成**：集成阿里云通义千问AI对话功能
- 🖼️ **图像识别**：支持ONNX模型进行图像分类识别
- 🔐 **会话管理**：完整的用户认证和会话管理机制
- 💾 **数据库支持**：MySQL数据库集成，支持异步操作
- 📡 **消息队列**：RabbitMQ消息队列，实现流量削峰
- 🛡️ **安全特性**：支持SSL/TLS加密通信
- 🔧 **模块化设计**：清晰的中间件和路由架构

## 整体框架

### 项目结构

```
cyclone-HTTPServer/
├── HttpServer/           # HTTP服务器核心框架
│   ├── include/         # 头文件
│   │   ├── HttpServer.hpp      # HTTP服务器主类
│   │   ├── Router.hpp          # 路由系统
│   │   ├── Middleware.hpp      # 中间件系统
│   │   ├── Session.hpp         # 会话管理
│   │   └── Cache/              # 缓存模块
│   └── src/            # 实现文件
├── AIApps/             # AI应用模块
│   └── ChatServer/     # 聊天服务器
│       ├── include/    # 头文件
│       │   ├── ChatServer.h           # 聊天服务器主类
│       │   ├── handlers/              # 请求处理器
│       │   └── AIUtil/                # AI工具类
│       ├── src/        # 实现文件
│       └── resource/   # 静态资源文件
└── CMakeLists.txt      # 构建配置
```

### 核心组件

#### 1. HttpServer - 服务器主类
**功能**: HTTP服务器核心，管理TCP连接和请求分发
- 基于muduo网络库，支持高并发连接
- 提供SSL/TLS加密通信支持
- 管理会话

#### 2. SSL模块 - 安全通信层
**功能**: 提供HTTPS加密通信支持
- **SslConfig**: SSL配置管理（证书、协议版本、加密套件）
- **SslContext**: SSL上下文管理，处理证书加载和协议配置
- **SslConnection**: SSL连接封装，处理握手和数据加解密
- 支持双向认证和会话复用

#### 3. Router - 路由系统
**功能**: 请求路由分发，支持多种路由注册方式
- **静态路由**: 精确路径匹配
- **动态路由**: 正则表达式路径匹配
- **缓存支持**: 可配置的路由级缓存
- **方法支持**: GET、POST等HTTP方法

#### 4. MiddlewareChain - 中间件链
**功能**: 请求预处理管道，支持插件式扩展
- 链式处理，支持请求/响应修改
- 可添加自定义中间件
- 支持CORS、认证等通用功能

#### 5. SessionManager - 会话管理
**功能**: 用户会话状态管理
- 基于Cookie的会话标识
- 可配置的会话存储后端
- 支持会话过期和续期

#### 6. Cache模块
**功能**: 响应缓存系统
- **KLruCache**: LRU缓存策略实现
- **CachePolicy**: 可扩展的缓存策略接口
- **CachedResponse**: 缓存响应对象管理

#### 7. 数据库连接池 (DbConnectionPool)
**功能**: MySQL数据库连接管理
- 连接池管理，避免频繁创建连接
- 线程安全的连接获取和释放
- 支持连接健康检查

#### 8. 内存池 (MemoryPool)
**功能**: 高效内存管理
- 多级缓存架构（ThreadCache → CentralCache → PageCache）
- 减少内存碎片，提高分配效率
- 线程本地存储优化

## 项目环境

### 系统要求
- Linux
- C++17
- CMake 3.10+

### 依赖库
```bash
# 核心依赖
- muduo网络库
- OpenSSL
- CURL

# 数据库
- MySQL Connector/C++

# AI功能
- OpenCV (图像处理)
- ONNX Runtime (模型推理)

# 消息队列
- RabbitMQ
- SimpleAmqpClient
```

### 环境变量配置
```bash
# 阿里云API密钥
export DASHSCOPE_API_KEY="your-api-key"

# 模型路径
export MODEL_PATH="/path/to/model.onnx"
export LABEL_PATH="/path/to/labels.txt"
```

## 编译

### 1. 克隆项目
```bash
git clone <repository-url>
cd cyclone-HTTPServer
```

### 2. 创建构建目录
```bash
mkdir build && cd build
```

### 3. 配置和编译
```bash
cmake ..
make 
```

### 4. 安装依赖库
确保系统中已安装以下库：
```bash
# Ubuntu/Debian
sudo apt-get install libmuduo-dev libssl-dev libcurl4-openssl-dev
sudo apt-get install libmysqlcppconn-dev libopencv-dev
sudo apt-get install rabbitmq-server librabbitmq-dev

# 安装ONNX Runtime
# 从官网下载并安装: https://onnxruntime.ai/
```

## 运行

### 1. 启动服务
```bash
# 进入构建目录
cd build

# 运行服务器
./ai_server -p 8080
```

### 2. 服务参数
- `-p`: 指定端口号（默认80）

### 3. 服务初始化
服务器启动时会：
- 初始化数据库连接
- 启动RabbitMQ线程池处理异步任务
- 加载AI模型和配置


## 总结

cyclone-HTTPServer是一个功能完整的高性能HTTP服务器框架，具有以下优势：

### 技术亮点
1. **架构清晰**: 模块化设计，易于扩展和维护
2. **性能优异**: 基于muduo网络库，支持高并发
3. **功能丰富**: 集成AI对话、图像识别等现代Web功能
4. **企业级特性**: 支持会话管理、数据库、消息队列等
5. **安全性强**: 完善的错误处理和SSL支持

### 适用场景
- AI聊天机器人服务
- 图像识别应用
- 高并发Web服务
- 微服务架构中的API网关

### 扩展方向
- 支持更多AI模型和服务
- 添加WebSocket支持
- 实现负载均衡和集群部署
- 增加监控和日志系统

