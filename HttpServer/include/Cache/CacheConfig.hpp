#pragma once

#include <string>
#include "CacheTypes.hpp"

namespace http::cache {

// 响应缓存配置
struct CacheConfig {
    bool enabled = false;
    int ttlSeconds = 300;           // 默认5分钟
    int maxSize = 1000;             // 默认最大缓存1000个响应

    // 新增：缓存策略
    CacheStrategy strategy = CacheStrategy::LRU;

    // 新增：Key生成规则
    KeyGenerationStrategy keyStrategy = KeyGenerationStrategy::DEFAULT;
    KeyGenerator customKeyGenerator;

    // 新增：外部缓存配置（当strategy为EXTERNAL时使用）
    std::string externalHost = "localhost";
    int externalPort = 6379;
    std::string externalPassword;

    // 新增：缓存分片配置（针对高并发场景）
    bool enableSharding = false;
    int shardCount = 4;

    // 新增：缓存统计和监控
    bool enableMetrics = false;

    // 构造函数
    CacheConfig() = default;
    CacheConfig(int ttl) : enabled(true), ttlSeconds(ttl) {}
    CacheConfig(int ttl, int size) : enabled(true), ttlSeconds(ttl), maxSize(size) {}

    // 新增：带策略的构造函数
    CacheConfig(int ttl, int size, CacheStrategy strat)
        : enabled(true), ttlSeconds(ttl), maxSize(size), strategy(strat) {}

    // 新增：外部缓存构造函数
    CacheConfig(const std::string& host, int port, const std::string& pwd = "")
        : enabled(true), strategy(CacheStrategy::EXTERNAL),
          externalHost(host), externalPort(port), externalPassword(pwd) {}
};

} // namespace http::cache