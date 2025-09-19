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
    
    // 注意：不在这里设置 messageCallback，而是在 onConnection 中根据 SSL/非SSL 分别设置
    // server_.setMessageCallback(std::bind(&HttpServer::onMessage, this, std::placeholders::_1,
    //                                      std::placeholders::_2, std::placeholders::_3));
}
void HttpServer::setSslConfig(const ssl::SslConfig& config) {
    if (useSSL_) {
        sslCtx_ = std::make_unique<ssl::SslContext>(config);
        
        // 使用sslconfig配置ssl上下文，其实可以放到ssl上下文的构造函数中进行，不过一些设置过程可能出错，因此额外初始化
        // 初始化完成后httpServer就有了完整的ssl上下文。
        // 后续httpserver的连接回调函数HttpServer::onConnection会检查是否启用ssl。
        if (!sslCtx_->initialize()) {
            LOG_ERROR << "Failed to initialize SSL context";
            abort();
        }
    }
}

auto HttpServer::onConnection(const muduo::net::TcpConnectionPtr& conn) -> void {
    if (conn->connected()) {
        if (useSSL_) {
            // SslConnection 构造时 会把 tcp 的 MessageCallback 设置为 SslConnection::onRead，这样
            // tcp 有消息时会先到 ssl 处理
            auto sslConn = std::make_unique<ssl::SslConnection>(conn, sslCtx_.get());
            // 将 http 的消息回调 HttpServer::onMessage 交给 SSL 层，以便 ssl 处理完之后可以调用。
            sslConn->setMessageCallback(std::bind(&HttpServer::onMessage, this,
                                                  std::placeholders::_1, std::placeholders::_2,
                                                  std::placeholders::_3));
            sslConns_[conn] = std::move(sslConn);

            // 开始握手，这里其实不调用，因为内部也只是调用了 SslConnection::startHandshake()
            // 尝试推进一次握手。 肯定不会直接握手完成。在 SslConnection::onRead()中会再次调用
            // SslConnection::handleHandshake() 不断推进的 也就是只要有有数据，就会不断调用
            // SslConnection::onRead() 尝试推进握手。直到握手成功
            sslConns_[conn]->startHandshake();

            // 对于SSL连接，不设置TCP层的messageCallback，避免重复处理
        } else {
            // 对于非SSL连接，设置普通的messageCallback
            conn->setMessageCallback(std::bind(&HttpServer::onMessage, this, std::placeholders::_1,
                                               std::placeholders::_2, std::placeholders::_3));
        }

        conn->setContext(HttpContext());
    }
}
auto HttpServer::onMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buf,
                           muduo::Timestamp receiveTime) -> void {
    try {
        // 对于 SSL 连接，数据已经通过 SSL 层的 messageCallback 直接传递到这里
        // 对于非 SSL 连接，直接处理明文数据

        // HttpContext 对象用于解析出 buf 中的请求报文，并把报文的关键信息封装到 HttpRequest 对象中
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

    // 根据是否使用SSL选择发送方式
    if (useSSL_) {
        auto it = sslConns_.find(conn);
        if (it != sslConns_.end()) {
            // 使用SSL加密发送
            it->second->send(buf.peek(), buf.readableBytes());
        }
    } else {
        // 直接TCP发送
        conn->send(&buf);
    }

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