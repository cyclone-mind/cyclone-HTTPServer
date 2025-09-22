/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-15 14:26:04
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-20 13:49:23
 * @FilePath: /cyclone-HTTPServer/HttpServer/include/HttpResponse.hpp
 * @Description: 
 * Copyright (c) 2025 by ${git_name} email: ${git_email}, All Rights Reserved.
 */

#pragma once
#include <muduo/net/TcpServer.h>
#include <muduo/base/Logging.h>

#include <cstdio>
#include <utility>

#include "HttpTypes.hpp"

namespace http {
class HttpResponse {
private:
    static constexpr size_t STATUS_LINE_BUFFER_SIZE = 32;  // HTTP状态行缓冲区大小
    HttpVersion httpVersion_{HttpVersion::HTTP_1_1};                              // HTTP协议版本(如HTTP/1.1)
    HttpStatusCode statusCode_{};                          // HTTP状态码
    std::string statusMessage_;                            // HTTP状态消息
    bool closeConnection_;                                 // 是否是短连接
    std::map<std::string, std::string> headers_;           // HTTP响应头部字段
    std::string body_;                                     // HTTP响应体内容
    bool isFile_;                                          // 标识响应体是否为文件内容
    std::string filePath_;                                 // 响应体文件路径   
public:
    HttpResponse(bool close = true)
        : statusCode_(HttpStatusCode::CUnknown), closeConnection_(close) {}

    void setVersion(HttpVersion version) {
        httpVersion_ = version;
    }
    void setStatusCode(HttpStatusCode code) {
        statusCode_ = code;
    }

    [[nodiscard]] auto getStatusCode() const -> HttpStatusCode {
        return statusCode_;
    }

    [[nodiscard]] auto getStatusMessage() const -> const std::string& {
        return statusMessage_;
    }

    void setStatusMessage(const std::string& message) {
        statusMessage_ = message;
    }

    void setCloseConnection(bool on) {
        closeConnection_ = on;
    }

    [[nodiscard]] auto closeConnection() const -> bool {
        return closeConnection_;
    }

    void setContentType(const std::string& contentType) {
        addHeader("Content-Type", contentType);
    }

    void setContentLength(uint64_t length) {
        addHeader("Content-Length", std::to_string(length));
    }

    void addHeader(const std::string& key, const std::string& value) {
        LOG_DEBUG << "addHeader called with key: '" << key << "', value: '" << value << "'";
        headers_[key] = value;
        LOG_DEBUG << "addHeader completed successfully";
    }

    [[nodiscard]] auto getBody() const -> const std::string& {
        return body_;
    }

    [[nodiscard]] auto getHeaders() const -> const std::map<std::string, std::string>& {
        return headers_;
    }

    void setBody(const std::string& body) {
        body_ = body;
        // body_ += "\0";
    }

    void setStatusLine(HttpVersion version, HttpStatusCode statusCode,
                        const std::string& statusMessage) {
        httpVersion_ = version;
        statusCode_ = statusCode;
        statusMessage_ = statusMessage;
    }

    auto appendToBuffer(muduo::net::Buffer* outputBuf) const -> void;
    
};
}  // namespace http