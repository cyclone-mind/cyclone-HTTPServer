
#pragma once
#include <muduo/base/Timestamp.h>
#include <muduo/net/Buffer.h>
#include <muduo/net/TcpServer.h>

#include <string>

#include "HttpRequest.hpp"
#include "HttpTypes.hpp"
namespace http {
/*
在TCP层面，连续的HTTP请求会作为字节流传输，可能出现以下情况：
    粘包：多个请求在一个TCP包中到达
    拆包：一个请求被拆分到多个TCP包中
    半包：请求不完整，需要等待更多数据
我们使用状态机来解决这个问题
*/

class HttpContext {
private:
    HttpRequestParseState state_{};  // 当前解析状态
    HttpRequest request_;            // 当前上下文的请求
public:
    HttpContext() : state_(HttpRequestParseState::CExpectRequestLine) {}
    // 解析可能出错因此设置返回值 bool 
    auto parseRequest(muduo::net::Buffer* buf, muduo::Timestamp recvTime) -> bool;

    // 当前请求是否完成
    auto isComplete() const -> bool {
        return state_ == HttpRequestParseState::CGotAll;
    }

    // 清空 HTTP 上下文
    auto reset() -> void {
        state_ = HttpRequestParseState::CExpectRequestLine;
        HttpRequest dummyData;
        request_.swap(dummyData);
    }

    // 获取 HTTP 请求
    auto request() const -> const HttpRequest& {
        return request_;
    }

    // 获取 HTTP 请求
    auto request() -> HttpRequest& {
        return request_;
    }

private:
    // 解析请求行
    auto parseRequestLine(muduo::net::Buffer* buf, muduo::Timestamp recvTime, bool& hasMore) -> bool;
    // 解析请求头
    auto parseHeaders(muduo::net::Buffer* buf, bool& hasMore) -> bool;
    // 解析请求体
    auto parseBody(muduo::net::Buffer* buf, bool& hasMore) -> bool;
    // 处理单个头部行
    auto processHeaderLine(muduo::net::Buffer* buf, const char* crlf, bool& hasMore) -> bool;
    // 头部解析完成后的状态转换
    auto transitionAfterHeaders(bool& hasMore) -> bool;
    // 判断是否需要解析请求体
    auto needsRequestBody() const -> bool;
    // 设置请求体解析
    auto setupBodyParsing(bool& hasMore) -> bool;
};
}  // namespace http
