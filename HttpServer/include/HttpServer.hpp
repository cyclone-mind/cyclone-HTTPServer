
#pragma once
#include <muduo/base/Logging.h>
#include <muduo/net/Callbacks.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpServer.h>

#include <utility>

#include "HttpContext.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "HttpTypes.hpp"
#include "MiddlewareChain.hpp"
#include "Router.hpp"
#include "RouterHandler.hpp"
#include "Session.hpp"
#include "SessionManager.hpp"
#include "SslConfig.hpp"
#include "SslTypes.hpp"
#include "SslContext.hpp"
#include "SslConnection.hpp"
namespace http {
class HttpServer : public muduo::noncopyable {
public:
    using HttpCallback = std::function<void(const HttpRequest&, HttpResponse*)>;

private:
    muduo::net::InetAddress listenAddr_;
    muduo::net::TcpServer server_;
    muduo::net::EventLoop mainLoop_;
    HttpCallback httpCallback_;
    bool useSSL_;
    std::string staticRoot_;                       // 静态文件根目录
    router::Router router_;                        // 路由
    middleware::MiddlewareChain middlewareChain_;  // 中间件链
    std::unique_ptr<session::SessionManager> sessionManager_;
    std::unique_ptr<ssl::SslContext> sslCtx_;  // SSL 上下文
    std::unordered_map<muduo::net::TcpConnectionPtr,std::unique_ptr<ssl::SslConnection>> sslConns_; // SSL 连接

public:
    HttpServer(int port, const std::string& name, bool useSSL = false,
               muduo::net::TcpServer::Option option = muduo::net::TcpServer::kNoReusePort)
        : listenAddr_(port),
          server_(&mainLoop_, listenAddr_, name, option),
          useSSL_(useSSL),
          httpCallback_(std::bind(&HttpServer::HttpHandleRequest, this, std::placeholders::_1,
                                  std::placeholders::_2)) {
        initialize();
    }
    // 服务器运行函数
    auto start() -> void {
        LOG_WARN << "HttpServer[" << server_.name() << "] starts listening on" << server_.ipPort();
        server_.start();
        mainLoop_.loop();
    }
    // 提供给开发者，便于设置 muduo 线程数
    auto setThreadNums(int numThreads) -> void {
        server_.setThreadNum(numThreads);
    }

    // 设置静态文件根目录
    auto setStaticRoot(const std::string& rootPath) -> void {
        staticRoot_ = rootPath;
    }

    // 提供给开发者的静态路由注册接口-回调函数形式
    auto Get(const std::string& path, const HttpCallback& cb) -> void {
        router_.registerCallback(HttpMethod::CGet, path, cb);
    }
    // 提供给开发者的静态路由注册接口-回调函数形式（支持缓存）
    auto Get(const std::string& path, const HttpCallback& cb, const router::Router::CacheConfig& cacheConfig) -> void {
        router_.registerCallback(HttpMethod::CGet, path, cb, cacheConfig);
    }

    // 提供给开发者的静态路由注册接口-HandlerPtr形式
    auto Get(const std::string& path, router::Router::HandlerPtr handler) -> void {
        router_.registerHandler(HttpMethod::CGet, path, std::move(handler));
    }
    // 提供给开发者的静态路由注册接口-HandlerPtr形式（支持缓存）
    auto Get(const std::string& path, router::Router::HandlerPtr handler, const router::Router::CacheConfig& cacheConfig) -> void {
        router_.registerHandler(HttpMethod::CGet, path, std::move(handler), cacheConfig);
    }

    auto Post(const std::string& path, const HttpCallback& cb) -> void {
        router_.registerCallback(HttpMethod::CPost, path, cb);
    }
    // 提供给开发者的静态路由注册接口-回调函数形式（支持缓存）
    auto Post(const std::string& path, const HttpCallback& cb, const router::Router::CacheConfig& cacheConfig) -> void {
        router_.registerCallback(HttpMethod::CPost, path, cb, cacheConfig);
    }
    
    auto Post(const std::string& path, router::Router::HandlerPtr handler) -> void {
        router_.registerHandler(HttpMethod::CPost, path, std::move(handler));
    }
    // 提供给开发者的静态路由注册接口-HandlerPtr形式（支持缓存）
    auto Post(const std::string& path, router::Router::HandlerPtr handler, const router::Router::CacheConfig& cacheConfig) -> void {
        router_.registerHandler(HttpMethod::CPost, path, std::move(handler), cacheConfig);
    }

    // 提供给开发者的动态路由注册接口
    // 注册动态路由处理器（无缓存）
    void addRoute(HttpMethod method, const std::string& path, router::Router::HandlerPtr handler) {
        router_.addRegexHandler(method, path, std::move(handler));
    }

    // 注册动态路由处理器（支持缓存）
    void addRoute(HttpMethod method, const std::string& path, router::Router::HandlerPtr handler,
                  const router::Router::CacheConfig& cacheConfig) {
        router_.addRegexHandler(method, path, std::move(handler), cacheConfig);
    }

    // 注册动态路由处理函数（无缓存）
    void addRoute(HttpMethod method, const std::string& path,
                  const router::Router::HandlerCallback& callback) {
        router_.addRegexCallback(method, path, callback);
    }

    // 注册动态路由处理函数（支持缓存）
    void addRoute(HttpMethod method, const std::string& path,
                  const router::Router::HandlerCallback& callback, const router::Router::CacheConfig& cacheConfig) {
        router_.addRegexCallback(method, path, callback, cacheConfig);
    }

    // 作为第三方模块，开发者需要拥有添加中间件的接口
    auto addMiddleware(std::shared_ptr<middleware::Middleware> middleware) -> void {
        middlewareChain_.addMiddleware(std::move(middleware));
    }

    auto setSessionManager(std::unique_ptr<session::SessionManager> sessionManager) -> void {
        sessionManager_ = std::move(sessionManager);
    }
    auto getSessionManager() -> session::SessionManager* {
        return sessionManager_.get();
    }

    // 提供给开发者启用 SSL
    void enableSSL(bool enable) {
        useSSL_ = enable;
    }

    void setSslConfig(const ssl::SslConfig& config);

private:
    // HTTP服务器核心组件初始化，设置TCP连接和消息处理回调函数
    auto initialize() -> void;
    // TCP连接建立时的回调处理，负责为新连接创建HTTP解析上下文
    auto onConnection(const muduo::net::TcpConnectionPtr& conn) -> void;
    // TCP消息到达时的回调处理，负责HTTP协议解析和请求分发
    void onMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buf,
                   muduo::Timestamp receiveTime);
    // 处理完整HTTP请求的业务逻辑，执行用户定义的请求处理回调
    auto onRequest(const muduo::net::TcpConnectionPtr& conn, HttpRequest& req) -> void;

    auto HttpHandleRequest(const HttpRequest& req, HttpResponse* resp) -> void;
};
}  // namespace http