#include "../../include/Cache/CachedResponse.hpp"
#include "../../include/HttpResponse.hpp"

namespace http::cache {

// CachedResponse的实现
CachedResponse::CachedResponse(const HttpResponse& resp, int ttlSeconds)
    : statusCode(resp.getStatusCode()),
      expiresAt(std::chrono::steady_clock::now() + std::chrono::seconds(ttlSeconds)) {

    // 智能 body 分配策略 - 使用getter方法
    const std::string& originalBody = resp.getBody();
    bodySize_ = originalBody.size();

    if (bodySize_ <= 256 * 1024) {  // 256KB以内用内存池
        bodyBuffer_ = (char*)Kama_memoryPool::MemoryPool::allocate(bodySize_ + 1);
        bodyFromPool_ = true;
    } else {  // 超大对象直接系统分配
        bodyBuffer_ = (char*)malloc(bodySize_ + 1);
        bodyFromPool_ = false;
    }

    if (bodyBuffer_) {
        memcpy(bodyBuffer_, originalBody.c_str(), bodySize_ + 1);
    }

    // 其他字段正常初始化 - 使用getter方法
    statusMessage = resp.getStatusMessage();
    headers = resp.getHeaders();
}

// 析构函数
CachedResponse::~CachedResponse() {
    if (bodyBuffer_) {
        if (bodyFromPool_) {
            Kama_memoryPool::MemoryPool::deallocate(bodyBuffer_, bodySize_ + 1);
        } else {
            free(bodyBuffer_);
        }
        bodyBuffer_ = nullptr;
    }
}

// 拷贝构造函数
CachedResponse::CachedResponse(const CachedResponse& other)
    : statusCode(other.statusCode),
      statusMessage(other.statusMessage),
      headers(other.headers),
      expiresAt(other.expiresAt),
      bodySize_(other.bodySize_),
      bodyFromPool_(false) {  // 拷贝的对象总是使用新分配的内存

    if (other.bodyBuffer_ && bodySize_ > 0) {
        if (bodySize_ <= 256 * 1024) {
            bodyBuffer_ = (char*)Kama_memoryPool::MemoryPool::allocate(bodySize_ + 1);
            bodyFromPool_ = true;
        } else {
            bodyBuffer_ = (char*)malloc(bodySize_ + 1);
            bodyFromPool_ = false;
        }
        memcpy(bodyBuffer_, other.bodyBuffer_, bodySize_ + 1);
    }
}

// 拷贝赋值运算符
CachedResponse& CachedResponse::operator=(const CachedResponse& other) {
    if (this != &other) {
        // 先清理现有的资源
        if (bodyBuffer_) {
            if (bodyFromPool_) {
                Kama_memoryPool::MemoryPool::deallocate(bodyBuffer_, bodySize_ + 1);
            } else {
                free(bodyBuffer_);
            }
        }

        // 拷贝数据
        statusCode = other.statusCode;
        statusMessage = other.statusMessage;
        headers = other.headers;
        expiresAt = other.expiresAt;
        bodySize_ = other.bodySize_;
        bodyFromPool_ = false;

        if (other.bodyBuffer_ && bodySize_ > 0) {
            if (bodySize_ <= 256 * 1024) {
                bodyBuffer_ = (char*)Kama_memoryPool::MemoryPool::allocate(bodySize_ + 1);
                bodyFromPool_ = true;
            } else {
                bodyBuffer_ = (char*)malloc(bodySize_ + 1);
                bodyFromPool_ = false;
            }
            memcpy(bodyBuffer_, other.bodyBuffer_, bodySize_ + 1);
        } else {
            bodyBuffer_ = nullptr;
        }
    }
    return *this;
}

// 移动构造函数
CachedResponse::CachedResponse(CachedResponse&& other) noexcept
    : statusCode(other.statusCode),
      statusMessage(std::move(other.statusMessage)),
      headers(std::move(other.headers)),
      expiresAt(other.expiresAt),
      bodyBuffer_(other.bodyBuffer_),
      bodySize_(other.bodySize_),
      bodyFromPool_(other.bodyFromPool_) {

    // 移动后清空源对象
    other.bodyBuffer_ = nullptr;
    other.bodySize_ = 0;
    other.bodyFromPool_ = false;
}

// 移动赋值运算符
CachedResponse& CachedResponse::operator=(CachedResponse&& other) noexcept {
    if (this != &other) {
        // 先清理现有的资源
        if (bodyBuffer_) {
            if (bodyFromPool_) {
                Kama_memoryPool::MemoryPool::deallocate(bodyBuffer_, bodySize_ + 1);
            } else {
                free(bodyBuffer_);
            }
        }

        // 移动数据
        statusCode = other.statusCode;
        statusMessage = std::move(other.statusMessage);
        headers = std::move(other.headers);
        expiresAt = other.expiresAt;
        bodyBuffer_ = other.bodyBuffer_;
        bodySize_ = other.bodySize_;
        bodyFromPool_ = other.bodyFromPool_;

        // 清空源对象
        other.bodyBuffer_ = nullptr;
        other.bodySize_ = 0;
        other.bodyFromPool_ = false;
    }
    return *this;
}

bool CachedResponse::isExpired() const {
    return std::chrono::steady_clock::now() > expiresAt;
}

void CachedResponse::applyTo(HttpResponse& resp) const {
    resp.setStatusCode(statusCode);
    if (!statusMessage.empty()) {
        resp.setStatusMessage(statusMessage);
    }
    if (bodyBuffer_ && bodySize_ > 0) {
        // 从内存池buffer创建string并设置到response
        std::string bodyStr(bodyBuffer_, bodySize_);
        resp.setBody(bodyStr);
    }
    // 应用缓存的响应头
    for (const auto& [key, value] : headers) {
        resp.addHeader(key, value);
    }
}

} // namespace http::cache