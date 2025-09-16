/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-16 16:08:02
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-16 17:31:00
 * @FilePath: /cyclone-HTTPServer/HttpServer/include/MiddlewareChain.hpp
 * @Description:
 * Copyright (c) 2025 by ${git_name} email: ${git_email}, All Rights Reserved.
 */
#pragma once
#include "Middleware.hpp"
#include <vector>
#include <memory>


namespace http::middleware {
class MiddlewareChain {
public:
    MiddlewareChain() = default;
    ~MiddlewareChain() = default;
    void addMiddleware(std::shared_ptr<Middleware> middleware);
    void processBefore(HttpRequest& request);
    void processAfter(HttpResponse& response);

private:
    std::vector<std::shared_ptr<Middleware>> middlewares_;
};
}  // namespace http::middleware