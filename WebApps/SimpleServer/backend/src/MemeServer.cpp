/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-16 13:29:16
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-17 14:33:27
 * @FilePath: /cyclone-HTTPServer/WebApps/SimpleServer/backend/src/MemeServer.cpp
 * @Description: 
 * Copyright (c) 2025 by cyclone, All Rights Reserved. 
 */
#include "../include/MemeServer.hpp"
#include "../include/handlers/EntryHandler.hpp"
#include "../include/handlers/TestDbConHandler.hpp"
auto MemeServer::initializeRouter() -> void {
    httpServer_.Get("/", std::make_shared<EntryHandler>(this));
    httpServer_.Get("/entry", std::make_shared<EntryHandler>(this));
    httpServer_.Get("/test-db", std::make_shared<TestDbConHandler>(this));

}