#include "../include/handlers/ChatLoginHandler.h"
#include "../include/handlers/ChatRegisterHandler.h"
#include "../include/handlers/ChatLogoutHandler.h"
#include"../include/handlers/ChatHandler.h"
#include"../include/handlers/ChatEntryHandler.h"
#include"../include/handlers/ChatSendHandler.h"
#include"../include/handlers/AIMenuHandler.h"
#include"../include/handlers/AIUploadSendHandler.h"
#include"../include/handlers/AIUploadHandler.h"
#include"../include/handlers/ChatHistoryHandler.h"

#include "../include/ChatServer.h"
#include "../../../HttpServer/include/HttpRequest.hpp"
#include "../../../HttpServer/include/HttpResponse.hpp"
#include "../../../HttpServer/include/HttpServer.hpp"



using namespace http;


ChatServer::ChatServer(int port,
    const std::string& name,
    muduo::net::TcpServer::Option option)
    : httpServer_(port, name, option)
{
    initialize();
}

void ChatServer::initialize() {
    std::cout << "ChatServer initialize start  ! " << std::endl;
	http::MysqlUtil::init("tcp://127.0.0.1:3306", "root", "123456", "ChatHttpServer", 5);
    // 初始化会话
    initializeSession();
    // 初始化中间件
    initializeMiddleware();
    // 初始化路由
    initializeRouter();
    // 初始化 ssl
    // initializeSsl();
}

void ChatServer::initChatMessage() {
    //从 chat_message 表中读取数据，将指定 user_id 对应的消息添加到 chatInformation 中
    std::cout << "initChatMessage start ! " << std::endl;
    readDataFromMySQL();
    std::cout << "initChatMessage success ! " << std::endl;
}

void ChatServer::readDataFromMySQL() {
    // 从 MySQL 数据库恢复所有用户的聊天历史记录到内存
    // 这个函数在服务器启动时调用，用于恢复持久化的聊天记录
    
    

    // SQL 查询
    std::string sql = "SELECT id, username,session_id, is_user, content, ts FROM chat_message ORDER BY ts ASC, id ASC";

    sql::ResultSet* res;
    try {
        res = mysqlUtil_.executeQuery(sql);
    }
    catch (const std::exception& e) {
        std::cerr << "MySQL query failed: " << e.what() << std::endl;
        return;
    }

    while (res->next()) {
        long long user_id = 0;
        std::string session_id ;
        std::string username, content;
        long long ts = 0;
        int is_user = 1;

        try {
            user_id = res->getInt64("id");
            session_id = res->getString("session_id");  
            username = res.getString("username");
            content = res.getString("content");
            ts = res->getInt64("ts");
            is_user = res.getInt("is_user");
        }
        catch (const std::exception& e) {
            std::cerr << "Failed to read row: " << e.what() << std::endl;
            continue; // 跳过异常
        }

        // 获取或创建该用户的会话映射表
        auto& userSessions = chatInformation[user_id];

        // 查找或创建该会话的 AIHelper 实例
        std::shared_ptr<AIHelper> helper;
        auto itSession = userSessions.find(user_id);
        if (itSession == userSessions.end()) {
            // 该会话不存在，创建新的 AIHelper
            helper = std::make_shared<AIHelper>();
            userSessions[session_id] = helper;
            chatInformation[user_id] = helper;
            // 记录该用户的会话ID
            sessionsIdsMap[user_id].push_back(session_id);
        }
        else {
            helper = itSession->second;
        }

        // 恢复消息到 AIHelper 的历史记录中
        helper->restoreMessage(content, ts);
    }

    std::cout << "readDataFromMySQL finished" << std::endl;

     
}



void ChatServer::setThreadNum(int numThreads) {
    httpServer_.setThreadNum(numThreads);
}


void ChatServer::start() {
    httpServer_.start();
}


void ChatServer::initializeRouter() {
    // === 路由初始化：注册所有 HTTP 路由和对应的处理器 ===

    // 入口页面路由
    httpServer_.Get("/", std::make_shared<ChatEntryHandler>(this));
    httpServer_.Get("/entry", std::make_shared<ChatEntryHandler>(this));

    // 用户认证相关路由
    httpServer_.Post("/login", std::make_shared<ChatLoginHandler>(this));
    httpServer_.Post("/register", std::make_shared<ChatRegisterHandler>(this));
    httpServer_.Post("/user/logout", std::make_shared<ChatLogoutHandler>(this));

    // 聊天功能路由
    httpServer_.Get("/chat", std::make_shared<ChatHandler>(this));
    httpServer_.Post("/chat/send", std::make_shared<ChatSendHandler>(this));           // 发送消息（已有会话）
    httpServer_.Post("/chat/history", std::make_shared<ChatHistoryHandler>(this));     // 获取历史记录

    // 多会话管理路由
    httpServer_.Post("/chat/send-new-session", std::make_shared<ChatCreateAndSendHandler>(this));  // 新建会话并发送
    httpServer_.Get("/chat/sessions", std::make_shared<ChatSessionsHandler>(this));                // 获取所有会话列表

    // 语音功能路由
    httpServer_.Post("/chat/tts", std::make_shared<ChatSpeechHandler>(this));

    // AI 菜单和文件上传路由
    httpServer_.Get("/menu", std::make_shared<AIMenuHandler>(this));
    httpServer_.Get("/upload", std::make_shared<AIUploadHandler>(this));
    httpServer_.Post("/upload/send", std::make_shared<AIUploadSendHandler>(this));

}

void ChatServer::initializeSession() {
    // 创建会话存储
    auto sessionStorage = std::make_unique<http::session::MemorySessionStorage>();
    // 创建会话管理器
    auto sessionManager = std::make_unique<http::session::SessionManager>(std::move(sessionStorage));
    // 设置会话管理器
    setSessionManager(std::move(sessionManager));
}

void ChatServer::initializeMiddleware() {
    // 创建中间件
    auto corsMiddleware = std::make_shared<http::middleware::CorsMiddleware>();
    // 添加中间件
    httpServer_.addMiddleware(corsMiddleware);
}


void ChatServer::packageResp(const std::string& version,
    http::HttpResponse::HttpStatusCode statusCode,
    const std::string& statusMsg,
    bool close,
    const std::string& contentType,
    int contentLen,
    const std::string& body,
    http::HttpResponse* resp)
{
    if (resp == nullptr)
    {
        LOG_ERROR << "Response pointer is null";
        return;
    }

    try
    {
        resp->setVersion(version);
        resp->setStatusCode(statusCode);
        resp->setStatusMessage(statusMsg);
        resp->setCloseConnection(close);
        resp->setContentType(contentType);
        resp->setContentLength(contentLen);
        resp->setBody(body);

        LOG_INFO << "Response packaged successfully";
    }
    catch (const std::exception& e)
    {
        LOG_ERROR << "Error in packageResp: " << e.what();
        // 设置一个简单的错误响应
        resp->setStatusCode(http::HttpStatusCode::C500InternalServerError);
        resp->setStatusMessage("Internal Server Error");
        resp->setCloseConnection(true);
    }
}

void ChatServer::initializeSsl() {
        httpServer_.enableSSL(true);
        ssl::SslConfig sslConfig;
        const std::string certPath = "/opt/cyclone/ssl";

        std::string certFile = certPath + "/kktui.dpdns.org_bundle.crt";
        std::string keyFile = certPath + "/kktui.dpdns.org.key";
        sslConfig.setCertificateFile(certFile);
        sslConfig.setPrivateKeyFile(keyFile);
        

        sslConfig.setProtocolVersion(ssl::SSLVersion::TLS_1_2);
        sslConfig.setCipherList("ECDHE-RSA-AES256-GCM-SHA384:ECDHE-RSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-SHA384:ECDHE-RSA-AES128-SHA256:AES256-GCM-SHA384:AES128-GCM-SHA256:AES256-SHA256:AES128-SHA256:HIGH:!aNULL:!eNULL:!EXPORT:!DES:!RC4:!MD5:!PSK:!SRP:!CAMELLIA");
        
        sslConfig.setVerifyClient(false);
        sslConfig.setVerifyDepth(1);
        
        sslConfig.setSessionTimeout(300); // seconds
        sslConfig.setSessionCacheSize(10240);
        httpServer_.setSslConfig(sslConfig);
    }
