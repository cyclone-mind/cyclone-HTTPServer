#pragma once

#include <cstddef>
#include <functional>
#include <regex>
#include <unordered_map>
#include <utility>
#include <vector>
#include <memory>
#include <optional>
#include <chrono>

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "HttpTypes.hpp"
#include "RouterHandler.hpp"
#include "KICachePolicy.hpp"
#include "KLruCache.hpp"
#include "Cache/CacheTypes.hpp"
#include "Cache/CacheConfig.hpp"
#include "Cache/CachedResponse.hpp"

namespace http::router {

class Router {
public:
    using HandlerCallback = std::function<void(const HttpRequest& req, HttpResponse* resp)>;
    // 多个路由可能共享同一个 Handler
    using HandlerPtr = std::shared_ptr<RouterHandler>;

    // 使用Cache模块的类型
    using CacheConfig = cache::CacheConfig;
    using CachedResponse = cache::CachedResponse;
    using CacheKey = cache::CacheKey;

    struct RouterKey {
        HttpMethod method;
        std::string path;
        auto operator==(const RouterKey& other) const -> bool {
            return method == other.method && path == other.path;
        }
    };

    struct RouterHash {
        auto operator()(const RouterKey& key) const -> size_t {
            size_t methodHash = std::hash<int>{}(static_cast<int>(key.method));
            size_t pathHash = std::hash<std::string>{}(key.path);
            return methodHash * 31 + pathHash;
        }
    };

    // 注册静态路由-回调函数式（无缓存）
    auto registerCallback(HttpMethod method, const std::string& path, HandlerCallback callback)
        -> void {
        callbacks_[RouterKey{method, path}] = std::move(callback);
    }

    // 注册静态路由-回调函数式（支持缓存）
    auto registerCallback(HttpMethod method, const std::string& path, HandlerCallback callback,
                         const CacheConfig& cacheConfig) -> void {
        RouterKey key{method, path};
        callbacks_[key] = std::move(callback);
        if (cacheConfig.enabled) {
            routeCacheConfigs_[key] = cacheConfig;
            // 初始化响应缓存（如果还没有初始化）
            if (!responseCache_) {
                responseCache_ = createCacheImplementation(cacheConfig);
            }
        }
    }

    // 注册静态路由-对象式（无缓存）
    auto registerHandler(HttpMethod method, const std::string& path, HandlerPtr handler) {
        handlers_[RouterKey{method, path}] = std::move(handler);
    }

    // 注册静态路由-对象式（支持缓存）
    auto registerHandler(HttpMethod method, const std::string& path, HandlerPtr handler,
                        const CacheConfig& cacheConfig) {
        RouterKey key{method, path};
        handlers_[key] = std::move(handler);
        if (cacheConfig.enabled) {
            routeCacheConfigs_[key] = cacheConfig;
            // 初始化响应缓存（如果还没有初始化）
            if (!responseCache_) {
                responseCache_ = createCacheImplementation(cacheConfig);
            }
        }
    }


    // 注册动态路由处理器（无缓存）
    void addRegexHandler(HttpMethod method, const std::string& path, HandlerPtr handler) {
        std::regex pathRegex = convertToRegex(path);
        regexHandlers_.emplace_back(method, pathRegex, handler, CacheConfig{});
    }

    // 注册动态路由处理器（支持缓存）
    void addRegexHandler(HttpMethod method, const std::string& path, HandlerPtr handler,
                        const CacheConfig& cacheConfig) {
        std::regex pathRegex = convertToRegex(path);
        regexHandlers_.emplace_back(method, pathRegex, handler, cacheConfig);
        if (cacheConfig.enabled && !responseCache_) {
            responseCache_ = createCacheImplementation(cacheConfig);
        }
    }

    // 注册动态路由处理函数（无缓存）
    void addRegexCallback(HttpMethod method, const std::string& path,
                          const HandlerCallback& callback) {
        std::regex pathRegex = convertToRegex(path);
        regexCallbacks_.emplace_back(method, pathRegex, callback, CacheConfig{});
    }

    // 注册动态路由处理函数（支持缓存）
    void addRegexCallback(HttpMethod method, const std::string& path,
                         const HandlerCallback& callback, const CacheConfig& cacheConfig) {
        std::regex pathRegex = convertToRegex(path);
        regexCallbacks_.emplace_back(method, pathRegex, callback, cacheConfig);
        if (cacheConfig.enabled && !responseCache_) {
            responseCache_ = createCacheImplementation(cacheConfig);
        }
    }


    auto route(const HttpRequest& request, HttpResponse* response) -> bool;

private:
    // 缓存工厂方法：根据配置创建合适的缓存实现
    auto createCacheImplementation(const CacheConfig& config)
        -> std::unique_ptr<cache::KICachePolicy<CacheKey, CachedResponse>> {
        // 暂时只支持LRU缓存，未来可以扩展支持其他策略
        switch (config.strategy) {
            case cache::CacheStrategy::LRU:
            default:
                // 使用LRU缓存（支持分片配置）
                if (config.enableSharding && config.shardCount > 1) {
                    // 暂时使用普通LRU，未来可以实现分片LRU
                    return std::make_unique<cache::KLruCache<CacheKey, CachedResponse>>(config.maxSize);
                } else {
                    return std::make_unique<cache::KLruCache<CacheKey, CachedResponse>>(config.maxSize);
                }
        }
    }
    auto convertToRegex(const std::string& pathPattern)
        -> std::regex {  // 将路径模式转换为正则表达式，支持匹配任意路径参数
        // 例如将 "/user/:id" 转换为 "^/user/([^/]+)$"
        std::string regexPattern =
            "^" + std::regex_replace(pathPattern, std::regex(R"(/:([^/]+))"), R"(/([^/]+))") + "$";
        return std::regex(regexPattern);
    }
    // 提取路径参数
    void extractPathParameters(const std::smatch& match, HttpRequest& request) {
        // Assuming the first match is the full path, parameters start from index 1
        // 自动将匹配的组提取为 param1, param2, ...
        for (size_t i = 1; i < match.size(); ++i) {
            request.setPathParams("param" + std::to_string(i), match[i].str());
        }
    }
    // 动态路由对象-回调函数形式
    struct RouteCallbackObj {
        HttpMethod method_;
        std::regex pathRegex_;
        HandlerCallback callback_;
        CacheConfig cacheConfig_;
        RouteCallbackObj(HttpMethod method, std::regex pathRegex, HandlerCallback callback)
            : method_(method), pathRegex_(std::move(pathRegex)), callback_(std::move(callback)), cacheConfig_{} {}
        RouteCallbackObj(HttpMethod method, std::regex pathRegex, HandlerCallback callback, const CacheConfig& cacheConfig)
            : method_(method), pathRegex_(std::move(pathRegex)), callback_(std::move(callback)), cacheConfig_(cacheConfig) {}
    };

    // 动态路由对象-handler对象形式
    struct RouteHandlerObj {
        HttpMethod method_;
        std::regex pathRegex_;
        HandlerPtr handler_;
        CacheConfig cacheConfig_;
        RouteHandlerObj(HttpMethod method, std::regex pathRegex, HandlerPtr handler)
            : method_(method), pathRegex_(std::move(pathRegex)), handler_(std::move(handler)), cacheConfig_{} {}
        RouteHandlerObj(HttpMethod method, std::regex pathRegex, HandlerPtr handler, const CacheConfig& cacheConfig)
            : method_(method), pathRegex_(std::move(pathRegex)), handler_(std::move(handler)), cacheConfig_(cacheConfig) {}
    };
    auto buildCacheKey(const HttpRequest& req, const CacheConfig& config) -> std::string;
    auto isRouteCacheable(const Router& router, const HttpRequest& req) -> bool;

    // 静态路由缓存配置映射：路由键 -> 缓存配置
    std::unordered_map<RouterKey, CacheConfig, RouterHash> routeCacheConfigs_;

    // 响应缓存：缓存键 -> 缓存的响应
    std::unique_ptr<cache::KICachePolicy<CacheKey, CachedResponse>> responseCache_;

    std::unordered_map<RouterKey, HandlerCallback, RouterHash> callbacks_;
    std::unordered_map<RouterKey, HandlerPtr, RouterHash> handlers_;
    std::vector<RouteHandlerObj> regexHandlers_;
    std::vector<RouteCallbackObj> regexCallbacks_;


};
}  // namespace http::router

// 对于map类型中的 key，若非标准结构（比如自定义结构体类型），
// 则需要在第三个参数位置上提供自定义的哈希函数，这里以仿函数的形式提供。