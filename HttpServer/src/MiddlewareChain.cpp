/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-16 16:11:08
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-16 17:22:04
 * @FilePath: /cyclone-HTTPServer/HttpServer/src/MiddlewareChain.cpp
 * @Description:
 * Copyright (c) 2025 by ${git_name} email: ${git_email}, All Rights Reserved.
 */
#include "../include/MiddlewareChain.hpp"

#include <muduo/base/Logging.h>
namespace http::middleware {
void MiddlewareChain::addMiddleware(std::shared_ptr<Middleware> middleware) {
    middlewares_.emplace_back(middleware);
}
void MiddlewareChain::processBefore(HttpRequest &req) {
    for (auto &middleware : middlewares_) {
        middleware->before(req);
    }
}
void MiddlewareChain::processAfter(HttpResponse &resp) {
    try {
        // 反向处理响应，以保持中间件的正确执行顺序
        for (auto it = middlewares_.rbegin(); it != middlewares_.rend(); ++it) {
            if (*it) {  // 添加空指针检查
                (*it)->after(resp);
            }
        }
    } catch (const std::exception &e) {
        LOG_ERROR << "Error in middleware after processing: " << e.what();
    }
}
}  // namespace http::middleware
