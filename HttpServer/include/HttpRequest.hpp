#pragma once
#include <muduo/base/Timestamp.h>

#include <string>
#include <unordered_map>

#include "HttpTypes.hpp"

namespace http {
class HttpRequest {
public:
private:
    HttpMethod method_{};                                                // HTTP请求方法（GET, POST, PUT, DELETE等）
    HttpVersion version_{};                                              // HTTP协议版本
    std::string path_;                                                   // 请求路径
    std::unordered_map<std::string, std::string> pathParams_;          // 路径参数，如RESTful API中的参数
    std::unordered_map<std::string, std::string> queryParams_;         // 查询参数，URL中?后面的参数
    muduo::Timestamp recvTime_;                                         // 请求接收时间戳
    std::unordered_map<std::string, std::string> headers_;             // HTTP请求头集合
    std::string content_;                                               // 请求体内容
    uint64_t contentLength_;                                            // 请求体长度

public:
    HttpRequest() : method_(HttpMethod::CInvalid), version_(HttpVersion::UNKNOWN) {}

    // 1. 如果传入参数不一定合法，则返回 bool 方便验证
    // 2. 这个方法在协议解析阶段使用，使用指针范围参数 性能高
    auto setMethod(const char* start, const char* end) -> bool {
        assert(method_ == HttpMethod::CInvalid);
        std::string method(start, end);
        if (method == "GET") {
            method_ = HttpMethod::CGet;
        } else if (method == "POST") {
            method_ = HttpMethod::CPost;
        } else if (method == "PUT") {
            method_ = HttpMethod::CPut;
        } else if (method == "DELETE") {
            method_ = HttpMethod::CDelete;
        } else if (method == "OPTIONS") {
            method_ = HttpMethod::COptions;
        } else {
            method_ = HttpMethod::CInvalid;
        }
        return method_ != HttpMethod::CInvalid;
    }
    auto method() const -> HttpMethod {
        return method_;
    }

    // 设置HTTP版本
    auto setVersion(HttpVersion version) -> void {
        version_ = version;
    }
    auto version() const -> HttpVersion {
        return version_;
    }

    // 设置路径
    auto setPath(std::string path) -> void {
        path_ = std::move(path);
    }
    auto path() const -> std::string {
        return path_;
    }

    // 设置路径参数
    auto setPathParams(std::string key, std::string value) -> void {
        pathParams_[std::move(key)] = std::move(value);
    }
    auto getPathParam(const std::string& key) const -> std::string{
        auto item = pathParams_.find(key);
        return (item == pathParams_.end()) ? std::string() : item->second;
    }

    // 设置查询参数
    auto setQueryParams(std::string key, std::string value) -> void {
        queryParams_[std::move(key)] = std::move(value);
    }
    auto getQueryParam(const std::string& key) -> std::string{
        auto item = queryParams_.find(key);
        return (item == queryParams_.end()) ? std::string() : item->second;
    }

    auto setRecvTime(muduo::Timestamp recvTime) -> void {
        recvTime_ = recvTime;
    }

    // 解析请求行
    auto parseRequestLine(const char* begin, const char* end) -> bool;
    
    // 解析查询参数
    auto parseQueryParams(const char* begin, const char* end) -> void;

    // 添加请求头
    auto addHeader(const char* start, const char* colon, const char* end) -> void;
    auto getHeader(const std::string& field) const -> std::string {
        auto item = headers_.find(field);
        return (item == headers_.end()) ? std::string() : item->second;
    }
    auto headers() const -> const std::unordered_map<std::string, std::string>& {
        return headers_;
    }

    // 两种方式设置 请求体,字符串 和 字节流
    void setBody(const std::string& body) {
        content_ = body;
    }
    void setBody(const char* start, const char* end) {
        if (end >= start) {
            content_.assign(start, end - start);
        }
    }
    auto getBody() const -> std::string {
        return content_;
    }

    // 设置请求体长度
    auto setContentLength(uint64_t length) -> void {
        contentLength_ = length;
    }
    auto contentLength() const -> uint64_t {
        return contentLength_;
    }

    // 用来重置 请求 内容。在一个TCP连接中请求只有一个，不断被清空，重复使用
    auto swap(HttpRequest& other) -> void {
        std::swap(method_, other.method_);
        std::swap(path_, other.path_);
        std::swap(pathParams_, other.pathParams_);
        std::swap(queryParams_, other.queryParams_);
        std::swap(version_, other.version_);
        std::swap(headers_, other.headers_);
        std::swap(recvTime_, other.recvTime_);
    }
};
}  // namespace http