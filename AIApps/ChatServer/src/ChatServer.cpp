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
}

void ChatServer::initChatMessage() {
    //从chat_message表中读取数据，将指定user_id对应的消息添加到chatInformation中
    std::cout << "initChatMessage start ! " << std::endl;
    readDataFromMySQL();
    std::cout << "initChatMessage success ! " << std::endl;
}

void ChatServer::readDataFromMySQL() {
    
    
    const char* apiKey = std::getenv("DASHSCOPE_API_KEY");
    if (!apiKey) {
        std::cerr << "Error: DASHSCOPE_API_KEY not found in environment!" << std::endl;
        return;
    }

    // SQL 查询
    std::string sql = "SELECT id, username, is_user, content, ts FROM chat_message ORDER BY ts ASC, id ASC";

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
        std::string username, content;
        long long ts = 0;
        int is_user = 1;

        try {
            user_id = res->getInt64("id");
            username = res.getString("username");
            content = res.getString("content");
            ts = res->getInt64("ts");
            is_user = res.getInt("is_user");
        }
        catch (const std::exception& e) {
            std::cerr << "Failed to read row: " << e.what() << std::endl;
            continue; // 跳过异常
        }

        // 找到或创建 AIHelper
        std::shared_ptr<AIHelper> helper;
        auto it = chatInformation.find(user_id);
        if (it == chatInformation.end()) {
            helper = std::make_shared<AIHelper>(apiKey);
            chatInformation[user_id] = helper;
        }
        else {
            helper = it->second;
        }

        // 恢复消息
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
    // 注册url回调函数
    // 登录注册首页
    httpServer_.Get("/", std::make_shared<ChatEntryHandler>(this));
    httpServer_.Get("/entry", std::make_shared<ChatEntryHandler>(this));
    // 登录
    httpServer_.Post("/login", std::make_shared<ChatLoginHandler>(this));
    // 注册
    httpServer_.Post("/register", std::make_shared<ChatRegisterHandler>(this));
    //注销
    httpServer_.Post("/user/logout", std::make_shared<ChatLogoutHandler>(this));
    //聊天页面请求
    httpServer_.Get("/chat", std::make_shared<ChatHandler>(this));
    //发送消息
    httpServer_.Post("/chat/send", std::make_shared<ChatSendHandler>(this));
    //菜单页面
    httpServer_.Get("/menu", std::make_shared<AIMenuHandler>(this));
    //上传页面请求
    httpServer_.Get("/upload", std::make_shared<AIUploadHandler>(this));
    //上传发送
    httpServer_.Post("/upload/send", std::make_shared<AIUploadSendHandler>(this));
    //同步历史数据，第一次登录时将数据返回前端渲染
    httpServer_.Post("/chat/history", std::make_shared<ChatHistoryHandler>(this));


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
        resp->setStatusCode(http::HttpResponse::k500InternalServerError);
        resp->setStatusMessage("Internal Server Error");
        resp->setCloseConnection(true);
    }
}
