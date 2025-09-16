#include "../include/HttpServer.hpp"

#include <muduo/base/Timestamp.h>
#include <muduo/net/Buffer.h>
#include <muduo/net/Callbacks.h>

#include <boost/any.hpp>

namespace http {
auto HttpServer::initialize() -> void {
    // 设置回调函数
    server_.setConnectionCallback(
        std::bind(&HttpServer::onConnection, this, std::placeholders::_1));
    server_.setMessageCallback(std::bind(&HttpServer::onMessage, this, std::placeholders::_1,
                                         std::placeholders::_2, std::placeholders::_3));
}
auto HttpServer::onConnection(const muduo::net::TcpConnectionPtr& conn) -> void {
    if (conn->connected()) {
        conn->setContext(HttpContext());
        // 访问成员变量以避免 clang-tidy 静态化警告
        (void)useSSL_;  // 防止未使用变量警告
    }
}
auto HttpServer::onMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buf,
                           muduo::Timestamp receiveTime) -> void {
    try {
        auto* context = boost::any_cast<HttpContext>(conn->getMutableContext());
        if (!context->parseRequest(buf, receiveTime)) {
            // 解析过程中出错
            conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
            conn->shutdown();
        }
        if (context->isComplete()) {
            // 如果解析没有完成，会重新走到 onMessage,再次检查解析有无完成
            // 如果解析完成，则调用回调函数处理请求
            onRequest(conn, context->request());
            context->reset();
        }
    } catch (std::exception& e) {
        LOG_ERROR << "Exception: in onMessage: " << e.what();
        conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->shutdown();
    }
}
auto HttpServer::onRequest(const muduo::net::TcpConnectionPtr& conn, HttpRequest& req) -> void {
    // 管理长短连接，根据长短连接构造响应
    const auto& connection = req.getHeader("Connection");
    bool close = ((connection == "close") ||
                  (req.version() == HttpVersion::HTTP_1_0 && connection != "Keep-Alive"));
    HttpResponse response(close);


    httpCallback_(req, &response);

    muduo::net::Buffer buf;
    response.appendToBuffer(&buf);

    // 打印完整的响应内容用于调试

    std::string content = buf.toStringPiece().as_string();
    if (content.length() > 1024) {
        LOG_DEBUG << "Sending response (truncated):\n"
                  << content.substr(0, 1024) << "... [truncated]";
    } else {
        LOG_DEBUG << "Sending response:\n" << content;
    }

    conn->send(&buf);

    // 如果是短连接的话，返回响应报文后就断开连接
    if (response.closeConnection()) {
        conn->shutdown();
    }
}

// 执行请求对应的路由处理函数，这个函数会在构造函数中被设置为 httpCallback_ 成员
// 函数。对任何一个请求都要走到这里。寻找对应的路由
auto HttpServer::HttpHandleRequest(const HttpRequest& req, HttpResponse* resp) -> void {
    try {
        HttpRequest mutableReq = req;
        
        middlewareChain_.processBefore(mutableReq);
        // 路由处理，请求进到路由系统，在里面寻找对应的路由器或处理函数。
        // 如果是路由器，需要开发者自定义handle方法，如果是处理函数，则直接调用
        if (!router_.route(mutableReq, resp)) {
            LOG_INFO << "请求url:" << methodToString(req.method()) << " " << req.path();
            LOG_INFO << "未找到路由,返回404";
            resp->setStatusCode(HttpStatusCode::C404NotFound);
            resp->setStatusMessage("Not Found");
            resp->setCloseConnection(true);
        }
        // 处理响应后的中间件
        middlewareChain_.processAfter(*resp);
    } catch (const HttpResponse& res) {
        // 处理中间件抛出的响应（如CORS预检请求），
        // 预检请求的throw response 会由这里接收
        *resp = res;
    } catch (const std::exception& e) {
        // 错误处理
        resp->setStatusCode(HttpStatusCode::C500InternalServerError);
        resp->setBody(e.what());
    }
}
}  // namespace http