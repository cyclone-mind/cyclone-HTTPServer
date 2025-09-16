
#include "../include/HttpContext.hpp"
#include <algorithm>

namespace http {
auto HttpContext::parseRequest(muduo::net::Buffer *buf, muduo::Timestamp recvTime) -> bool {
    bool ok = true;
    bool hasMore = true;
    
    while (hasMore && ok) {
        switch (state_) {
            case HttpRequestParseState::CExpectRequestLine:
                ok = parseRequestLine(buf, recvTime, hasMore);
                break;
            case HttpRequestParseState::CExpectHeaders:
                ok = parseHeaders(buf, hasMore);
                break;
            case HttpRequestParseState::CExpectBody:
                ok = parseBody(buf, hasMore);
                break;
            case HttpRequestParseState::CGotAll:
                hasMore = false;
                break;
        }
    }
    return ok;
}

auto HttpContext::parseRequestLine(muduo::net::Buffer* buf, muduo::Timestamp recvTime, bool& hasMore) -> bool {
    const char* crlf = buf->findCRLF();
    if (crlf == nullptr) {
        hasMore = false;
        return true; // 等待更多数据
    }
    
    bool ok = request_.parseRequestLine(buf->peek(), crlf);
    if (ok) {
        request_.setRecvTime(recvTime);
        state_ = HttpRequestParseState::CExpectHeaders;
        buf->retrieveUntil(crlf + 2); // 跳过 \r\n
    } else {
        hasMore = false;
    }
    return ok;
}

auto HttpContext::parseHeaders(muduo::net::Buffer* buf, bool& hasMore) -> bool {
    const char* crlf = buf->findCRLF();
    if (crlf == nullptr) {
        hasMore = false;
        return true; // 等待更多数据
    }
    
    return processHeaderLine(buf, crlf, hasMore);
}

auto HttpContext::processHeaderLine(muduo::net::Buffer* buf, const char* crlf, bool& hasMore) -> bool {
    const char* colon = std::find(buf->peek(), crlf, ':');
    
    if (colon < crlf) {
        // 普通的头部行
        request_.addHeader(buf->peek(), colon, crlf);
        buf->retrieveUntil(crlf + 2);
        return true;
    } 
    
    if (buf->peek() == crlf) {
        // 空行，头部解析完毕
        buf->retrieveUntil(crlf + 2);
        return transitionAfterHeaders(hasMore);
    }
    
    // 格式错误的头部行
    hasMore = false;
    return false;
}

auto HttpContext::transitionAfterHeaders(bool& hasMore) -> bool {
    if (needsRequestBody()) {
        return setupBodyParsing(hasMore);
    } 
    
    // GET/HEAD/DELETE 等方法直接完成
    state_ = HttpRequestParseState::CGotAll;
    hasMore = false;
    return true;
}

auto HttpContext::needsRequestBody() const -> bool {
    return request_.method() == HttpMethod::CPost || 
           request_.method() == HttpMethod::CPut;
}

auto HttpContext::setupBodyParsing(bool& hasMore) -> bool {
    std::string contentLength = request_.getHeader("Content-Length");
    if (contentLength.empty()) {
        // POST/PUT 请求没有 Content-Length，是HTTP语法错误
        hasMore = false;
        return false;
    }
    
    request_.setContentLength(std::stoi(contentLength));
    if (request_.contentLength() > 0) {
        state_ = HttpRequestParseState::CExpectBody;
    } else {
        state_ = HttpRequestParseState::CGotAll;
        hasMore = false;
    }
    return true;
}

auto HttpContext::parseBody(muduo::net::Buffer* buf, bool& hasMore) -> bool {
    // 检查缓冲区中是否有足够的数据
    if (buf->readableBytes() < request_.contentLength()) {
        hasMore = false;
        return true; // 等待更多数据
    }
    
    // 读取指定长度的请求体
    std::string body(buf->peek(), buf->peek() + request_.contentLength());
    request_.setBody(body);
    buf->retrieve(request_.contentLength());
    
    state_ = HttpRequestParseState::CGotAll;
    hasMore = false;
    return true;
}

}  // namespace http