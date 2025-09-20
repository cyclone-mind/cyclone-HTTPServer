#pragma once

#include <map>
#include <string>
#include <chrono>
#include <memory>
#include "../HttpTypes.hpp"
#include "../HttpResponse.hpp"
#include "../MemoryPool/MemoryPool.h"

namespace http::cache {

class CachedResponse {
private:
    // 池化管理的body
    char* bodyBuffer_ = nullptr;
    size_t bodySize_ = 0;
    bool bodyFromPool_ = false;

public:
    // 标准分配的字段
    HttpStatusCode statusCode;
    std::string statusMessage;
    std::map<std::string, std::string> headers;
    std::chrono::steady_clock::time_point expiresAt;

    CachedResponse() = default;
    CachedResponse(const HttpResponse& resp, int ttlSeconds);
    ~CachedResponse();

    // 支持拷贝，用于 LRU 缓存
    CachedResponse(const CachedResponse& other);
    CachedResponse& operator=(const CachedResponse& other);

    // 移动语义
    CachedResponse(CachedResponse&& other) noexcept;
    CachedResponse& operator=(CachedResponse&& other) noexcept;

    bool isExpired() const;
    void applyTo(HttpResponse& resp) const;

    // 访问body的接口
    const char* getBody() const { return bodyBuffer_; }
    size_t getBodySize() const { return bodySize_; }
};

} // namespace http::cache