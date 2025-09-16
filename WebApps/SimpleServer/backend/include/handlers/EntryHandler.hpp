/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-16 13:43:03
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-16 16:05:31
 * @FilePath: /cyclone-HTTPServer/WebApps/SimpleServer/backend/include/handlers/EntryHandler.hpp
 * @Description:
 * Copyright (c) 2025 by ${git_name} email: ${git_email}, All Rights Reserved.
 */

#include "../../../../../HttpServer/include/RouterHandler.hpp"
class MemeServer;

class EntryHandler : public http::router::RouterHandler {
public:
    explicit EntryHandler(MemeServer* server) : server_(server){};
    void handle(const http::HttpRequest& req, http::HttpResponse* resp) override {
        
        std::string bufStr = "hello, world\n";
        resp->setStatusLine(req.version(), http::HttpStatusCode::C200Ok, "OK");
        resp->setCloseConnection(true);
        resp->setContentType("text");
        resp->setContentLength(bufStr.size());
        resp->setBody(bufStr);
    }
private:
MemeServer* server_;  // 路由处理函数需要通过服务器实例拿到相关的数据，比如访问用户登陆状态
};
