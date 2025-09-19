/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-16 10:20:55
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-19 21:38:07
 * @FilePath: /cyclone-HTTPServer/HttpServer/src/Router.cpp
 * @Description:
 * Copyright (c) 2025 by ${git_name} email: ${git_email}, All Rights Reserved.
 */
#include "../include/Router.hpp"
#include <muduo/base/Logging.h>


namespace http::router {



auto Router::route(const HttpRequest& req, HttpResponse* resp) -> bool {
    RouterKey key = {req.method(), req.path()};

    // 检查是否启用了响应缓存
    auto cacheConfigIt = routeCacheConfigs_.find(key);
    if (cacheConfigIt != routeCacheConfigs_.end() && responseCache_) {
        CacheKey cacheKey = std::to_string(static_cast<int>(req.method())) + ":" + req.path();

        // 尝试从缓存获取响应
        CachedResponse cachedResp;
        if (responseCache_->get(cacheKey, cachedResp) && !cachedResp.isExpired()) {
            // 缓存命中，直接应用缓存的响应
            cachedResp.applyTo(*resp);
            return true;
        }
    }

    // 第一步：查找静态路由回调函数
    // 通过请求方法和路径构成的key，在callbacks_映射中查找对应的回调函数
    auto callbackIt = callbacks_.find(key);
    if (callbackIt != callbacks_.end()) {
        // 找到匹配的静态路由回调函数，直接执行并返回成功
        callbackIt->second(req, resp);

        // 如果启用了缓存，缓存响应
        if (cacheConfigIt != routeCacheConfigs_.end() && responseCache_) {
            CacheKey cacheKey = std::to_string(static_cast<int>(req.method())) + ":" + req.path();
            CachedResponse cachedResp(*resp, cacheConfigIt->second.ttlSeconds);
            responseCache_->put(cacheKey, cachedResp);
        }

        return true;
    }

    // 第二步：查找静态路由处理器
    // 如果没有找到回调函数，则查找对应的处理器对象
    auto handlerIt = handlers_.find(key);
    if (handlerIt != handlers_.end()) {
        // 找到匹配的静态路由处理器，调用其handle方法并返回成功
        handlerIt->second->handle(req, resp);

        // 如果启用了缓存，缓存响应
        if (cacheConfigIt != routeCacheConfigs_.end() && responseCache_) {
            CacheKey cacheKey = std::to_string(static_cast<int>(req.method())) + ":" + req.path();
            CachedResponse cachedResp(*resp, cacheConfigIt->second.ttlSeconds);
            responseCache_->put(cacheKey, cachedResp);
        }

        return true;
    }

    // 第三步：查找动态路由回调函数
    // 遍历所有注册的正则表达式回调函数，查找匹配的动态路由
    for (const auto& [method, pathRegex, callback] : regexCallbacks_) {
        std::smatch match;                // 存储正则表达式匹配结果
        std::string pathStr(req.path());  // 将请求路径转换为字符串

        // 检查HTTP方法是否匹配，并且路径是否符合正则表达式模式
        if (method == req.method() && std::regex_match(pathStr, match, pathRegex)) {
            // 提取路径参数并添加到请求对象中
            // 创建新的请求对象副本，用于添加动态路径参数
            HttpRequest newReq(req);  // 因为这里需要用这一次所以是可以改的
            extractPathParameters(match, newReq);

            // 执行匹配的动态路由回调函数
            callback(newReq, resp);
            return true;
        }
    }

    // 第四步：查找动态路由处理器
    // 遍历所有注册的正则表达式处理器，查找匹配的动态路由
    for (const auto& [method, pathRegex, handler] : regexHandlers_) {
        std::smatch match;                // 存储正则表达式匹配的结果，初始为空
        std::string pathStr(req.path());  // 将请求路径转换为字符串进行匹配

        // 检查HTTP方法是否匹配，并且路径是否符合正则表达式模式
        // 例如：pathRegex是从"/user/:id"转换来的^/user/([^/]+)$，而pathStr是"/user/123"
        // 匹配成功时，match对象包含：match[0] = "/user/123" (完整匹配) match[1] = "123"
        // (第一个捕获组，即用户ID)
        if (method == req.method() && std::regex_match(pathStr, match, pathRegex)) {
            // 提取路径参数并添加到请求对象中
            // 创建新的请求对象副本，用于添加动态路径参数
            HttpRequest newReq(req);  // 因为这里需要用这一次所以是可以改的
            extractPathParameters(match, newReq);

            // 执行匹配的动态路由处理器
            handler->handle(newReq, resp);
            return true;
        }
    }
    return false;
}

// CachedResponse的实现
Router::CachedResponse::CachedResponse(const HttpResponse& resp, int ttlSeconds)
    : body(""),  // 暂时为空，后面需要从resp获取
      statusCode(resp.getStatusCode()),
      statusMessage(""),  // 暂时为空
      expiresAt(std::chrono::steady_clock::now() + std::chrono::seconds(ttlSeconds)) {
    // 注意：这里需要访问HttpResponse的私有成员，暂时简化实现
}

bool Router::CachedResponse::isExpired() const {
    return std::chrono::steady_clock::now() > expiresAt;
}

void Router::CachedResponse::applyTo(HttpResponse& resp) const {
    resp.setStatusCode(statusCode);
    if (!statusMessage.empty()) {
        // resp.setStatusMessage(statusMessage); // 如果HttpResponse有这个方法
    }
    if (!body.empty()) {
        resp.setBody(body);
    }
    // 应用缓存的响应头
    for (const auto& [key, value] : headers) {
        resp.addHeader(key, value);
    }
}

}  // namespace http::router