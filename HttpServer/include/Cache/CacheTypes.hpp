#pragma once

#include <functional>
#include <string>

namespace http::cache {

// 缓存策略类型枚举
enum class CacheStrategy {
    LRU,           // 最近最少使用
    LFU,           // 最少使用频率
    FIFO,          // 先进先出
    TTL_ONLY,      // 仅基于TTL
    EXTERNAL       // 外部缓存（如Redis、Memcached）
};

// Key生成策略类型
enum class KeyGenerationStrategy {
    DEFAULT,       // 默认：method + path
    CUSTOM         // 自定义函数
};

// 缓存键类型
using CacheKey = std::string;

// Key生成器函数类型
using KeyGenerator = std::function<std::string(const class HttpRequest&)>;

} // namespace http::cache
