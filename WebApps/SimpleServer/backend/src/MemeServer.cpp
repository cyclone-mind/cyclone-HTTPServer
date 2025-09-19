/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-16 13:29:16
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-19 21:40:11
 * @FilePath: /cyclone-HTTPServer/WebApps/SimpleServer/backend/src/MemeServer.cpp
 * @Description: 
 * Copyright (c) 2025 by cyclone, All Rights Reserved. 
 */
#include "../include/MemeServer.hpp"
#include "../include/handlers/EntryHandler.hpp"
#include "../include/handlers/TestDbConHandler.hpp"
auto MemeServer::initializeRouter() -> void {
    // 普通路由（无缓存）
    httpServer_.Get("/", std::make_shared<EntryHandler>(this));

    // 带缓存的路由（缓存5分钟）
    httpServer_.Get("/entry", std::make_shared<EntryHandler>(this),
                   http::router::Router::CacheConfig(300));

    // 另一个普通路由
    httpServer_.Get("/test-db", std::make_shared<TestDbConHandler>(this));
}