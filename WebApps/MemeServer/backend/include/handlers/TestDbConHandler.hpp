/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-17 14:11:25
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-17 14:11:28
 * @FilePath: /cyclone-HTTPServer/WebApps/SimpleServer/backend/include/handlers/TestDbConHandler.hpp
 * @Description:
 * Copyright (c) 2025 by cyclone, All Rights Reserved.
 */
#include "../../../../../HttpServer/include/RouterHandler.hpp"
#include "../../../../../HttpServer/include/MysqlUtil.hpp"

class MemeServer;

class TestDbConHandler : public http::router::RouterHandler {
public:
    explicit TestDbConHandler(MemeServer* server) : server_(server){};
    void handle(const http::HttpRequest& req, http::HttpResponse* resp) override;

private:
    MemeServer* server_;  // 路由处理函数需要通过服务器实例拿到相关的数据，比如访问用户登陆状态
};