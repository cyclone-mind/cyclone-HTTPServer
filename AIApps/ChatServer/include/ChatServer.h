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



class ChatServer {
public:
	ChatServer(int port,
		const std::string& name,
		muduo::net::TcpServer::Option option = muduo::net::TcpServer::kNoReusePort);

	void setThreadNum(int numThreads);
	void start();
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

private:
	void initialize();
	void initializeSession();
	void initializeRouter();
	void initializeMiddleware();
	void initializeSsl();

	void readDataFromMySQL();

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
	//所属的httpServer
	http::HttpServer	httpServer_;
	//数据库交互
	http::MysqlUtil		mysqlUtil_;
	//验证一个用户只能在同一时间登录一次
	std::unordered_map<int, bool>	onlineUsers_;
	std::mutex	mutexForOnlineUsers_;
	//每一个人都有相应的对话 userid->AIHelper
	//注意：这里的设计是为了避免chatInformation[key]的并发访问
	//直接操作可能会导致unordered_map线程不安全，需要加锁
	//如果用vector来存储，那么用户A对AI的操作可能会影响到用户B
	std::unordered_map<int, std::shared_ptr<AIHelper>> chatInformation;
	std::mutex	mutexForChatInformation;

	std::unordered_map<int, std::shared_ptr<ImageRecognizer> > ImageRecognizerMap;
	std::mutex	mutexForImageRecognizerMap;
};

