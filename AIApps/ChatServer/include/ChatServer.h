#pragma once

#include <atomic>
#include <memory>
#include <tuple>
#include <unordered_map>
#include <mutex>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>


#include "../../../HttpServer/include/HttpServer.hpp"
#include "../../../HttpServer/include/MysqlUtil.hpp"
#include "../../../HttpServer/include/FileUtil.h"
#include "../../../HttpServer/include/JsonUtil.h"
#include "AIUtil/AIHelper.h"
#include "AIUtil/ImageRecognizer.h"
#include "AIUtil/base64.h"
#include "AIUtil/MQManager.h"
#include "muduo/base/Logging.h"

class ChatLoginHandler;
class ChatRegisterHandler;
class ChatLogoutHandler;
class ChatHandler;
class ChatEntryHandler;
class ChatSendHandler;
class ChatHistoryHandler;

class AIMenuHandler;
class AIUploadHandler;
class AIUploadSendHandler;



/**
 * ChatServer 聊天服务器类
 *
 * 这是一个基于 HTTP 协议的聊天服务器，支持多用户会话管理、AI 对话功能
 * 使用 MySQL 存储用户数据和聊天记录，支持多种 AI 模型策略
 */
class ChatServer {
public:
	/**
	 * 构造函数
	 * @param port 监听端口
	 * @param name 服务器名称
	 * @param option TCP服务器选项（默认不复用端口）
	 */
	ChatServer(int port,
		const std::string& name,
		muduo::net::TcpServer::Option option = muduo::net::TcpServer::kNoReusePort);

	// 设置工作线程数
	void setThreadNum(int numThreads);

	// 启动服务器
	void start();

	// 从数据库初始化聊天消息历史
	void initChatMessage();
private:
	friend class ChatLoginHandler;
	friend class ChatRegisterHandler;
	friend  ChatLogoutHandler;
	friend class ChatHandler;
	friend class ChatEntryHandler;
	friend class ChatSendHandler;
	friend class AIMenuHandler;
	friend class AIUploadHandler;
	friend class AIUploadSendHandler;
	friend class ChatHistoryHandler;

	friend class ChatCreateAndSendHandler;
	friend class ChatSessionsHandler;
	friend class ChatSpeechHandler;

private:
	// === 初始化相关方法 ===
	void initialize();                  // 总初始化入口
	void initializeSession();           // 初始化会话管理器
	void initializeRouter();            // 初始化路由表（注册所有URL处理器）
	void initializeMiddleware();        // 初始化中间件（如CORS）
	void initializeSsl();              // 初始化SSL配置

	// 从 MySQL 数据库读取所有用户的历史聊天记录并恢复到内存
	void readDataFromMySQL();

	/**
	 * 封装 HTTP 响应包
	 * @param version HTTP版本
	 * @param statusCode 状态码
	 * @param statusMsg 状态信息
	 * @param close 是否关闭连接
	 * @param contentType 内容类型
	 * @param contentLen 内容长度
	 * @param body 响应体
	 * @param resp 响应对象指针
	 */
	void packageResp(const std::string& version, http::HttpResponse::HttpStatusCode statusCode,
		const std::string& statusMsg, bool close, const std::string& contentType,
		int contentLen, const std::string& body, http::HttpResponse* resp);

	void setSessionManager(std::unique_ptr<http::session::SessionManager> manager)
	{
		httpServer_.setSessionManager(std::move(manager));
	}
	auto getSessionManager() const -> http::session::SessionManager*
	{
		return httpServer_.getSessionManager();
	}

	// === 成员变量 ===

	// 底层的 HTTP 服务器实例
	http::HttpServer	httpServer_;

	// MySQL 数据库工具类，用于用户认证和消息持久化
	http::MysqlUtil		mysqlUtil_;

	/**
	 * 在线用户映射表
	 * key: 用户ID，value: 是否在线
	 * 用于确保同一用户同一时间只能登录一次
	 */
	std::unordered_map<int, bool>	onlineUsers_;
	std::mutex	mutexForOnlineUsers_;  // 保护 onlineUsers_ 的互斥锁

	/**
	 * 用户聊天信息映射表（二级映射）
	 * 第一层 key: 用户ID
	 * 第二层 key: 会话ID (session_id)
	 * value: AIHelper 智能指针，负责管理该会话的 AI 对话历史和交互
	 *
	 * 设计说明：
	 * - 使用二级映射避免并发访问时的线程安全问题
	 * - 每个用户可以有多个独立的对话会话
	 * - 直接操作 unordered_map 可能线程不安全，需要加锁保护
	 * - 用 vector 存储会导致用户 A 对 AI 的操作可能影响用户 B
	 */
	std::unordered_map<int, std::unordered_map<std::string,std::shared_ptr<AIHelper> > > chatInformation;
	std::mutex	mutexForChatInformation;  // 保护 chatInformation 的互斥锁

	// 图像识别器映射表
	// key: 用户ID，value: 该用户的图像识别器实例
	std::unordered_map<int, std::shared_ptr<ImageRecognizer> > ImageRecognizerMap;
	std::mutex	mutexForImageRecognizerMap;  // 保护 ImageRecognizerMap 的互斥锁

	/**
	 * 用户会话ID列表映射表
	 * key: 用户ID
	 * value: 该用户的所有会话ID列表
	 * 用于快速查询某个用户的所有聊天会话
	 */
	std::unordered_map<int,std::vector<std::string> > sessionsIdsMap;
	std::mutex mutexForSessionsId;  // 保护 sessionsIdsMap 的互斥锁
};

