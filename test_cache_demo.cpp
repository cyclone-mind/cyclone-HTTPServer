/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-19 21:41:10
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-19 21:41:16
 * @FilePath: /cyclone-HTTPServer/test_cache_demo.cpp
 * @Description: 
 * Copyright (c) 2025 by cyclone, All Rights Reserved. 
 */
#include <iostream>
#include <string>
#include <memory>
#include <chrono>
#include <thread>
#include "HttpServer/include/Router.hpp"
#include "HttpServer/include/HttpRequest.hpp"
#include "HttpServer/include/HttpResponse.hpp"
#include "HttpServer/include/HttpTypes.hpp"

// 测试用的简单处理器
class TestHandler : public http::router::RouterHandler {
private:
    std::string responseText;
    int callCount = 0;

public:
    TestHandler(std::string text) : responseText(std::move(text)) {}

    void handle(const http::HttpRequest& req, http::HttpResponse* resp) override {
        callCount++;
        std::cout << "Handler called " << callCount << " times for path: " << req.path() << std::endl;

        resp->setStatusCode(http::HttpStatusCode::C200Ok);
        resp->setBody(responseText + " (call #" + std::to_string(callCount) + ")");
    }
};

int main() {
    std::cout << "=== HTTP Server Response Cache Demo ===" << std::endl;

    // 创建路由器
    http::router::Router router;

    // 创建处理器
    auto cachedHandler = std::make_shared<TestHandler>("Cached Response");
    auto normalHandler = std::make_shared<TestHandler>("Normal Response");

    // 注册路由：一个带缓存，一个不带缓存
    router.registerHandler(http::HttpMethod::CGet, "/cached",
                          cachedHandler,
                          http::router::Router::CacheConfig(10)); // 缓存10秒

    router.registerHandler(http::HttpMethod::CGet, "/normal",
                          normalHandler);

    // 创建测试请求
    http::HttpRequest reqCached;
    const char* getStr = "GET";
    reqCached.setMethod(getStr, getStr + 3);
    reqCached.setPath("/cached");

    http::HttpRequest reqNormal;
    reqNormal.setMethod(getStr, getStr + 3);
    reqNormal.setPath("/normal");

    std::cout << "\n--- Testing Cached Route (/cached) ---" << std::endl;

    // 第一次请求 - 应该执行handler并缓存
    http::HttpResponse resp1;
    bool result1 = router.route(reqCached, &resp1);
    std::cout << "First request result: " << (result1 ? "SUCCESS" : "FAILED") << std::endl;
    std::cout << "Status: " << static_cast<int>(resp1.getStatusCode()) << std::endl;

    // 第二次请求 - 应该从缓存获取
    http::HttpResponse resp2;
    bool result2 = router.route(reqCached, &resp2);
    std::cout << "Second request result: " << (result2 ? "SUCCESS" : "FAILED") << std::endl;
    std::cout << "Status: " << static_cast<int>(resp2.getStatusCode()) << std::endl;

    // 第三次请求 - 仍然应该从缓存获取
    http::HttpResponse resp3;
    bool result3 = router.route(reqCached, &resp3);
    std::cout << "Third request result: " << (result3 ? "SUCCESS" : "FAILED") << std::endl;
    std::cout << "Status: " << static_cast<int>(resp3.getStatusCode()) << std::endl;

    std::cout << "\n--- Testing Normal Route (/normal) ---" << std::endl;

    // 测试普通路由（每次都应该执行handler）
    http::HttpResponse resp4;
    bool result4 = router.route(reqNormal, &resp4);
    std::cout << "First normal request result: " << (result4 ? "SUCCESS" : "FAILED") << std::endl;

    http::HttpResponse resp5;
    bool result5 = router.route(reqNormal, &resp5);
    std::cout << "Second normal request result: " << (result5 ? "SUCCESS" : "FAILED") << std::endl;

    std::cout << "\n=== Cache Demo Completed ===" << std::endl;
    std::cout << "Notice: Cached route should only call handler once," << std::endl;
    std::cout << "while normal route calls handler every time." << std::endl;

    return 0;
}