/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-16 16:02:51
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-16 16:11:15
 * @FilePath: /cyclone-HTTPServer/HttpServer/include/Middleware.hpp
 * @Description: 
 * Copyright (c) 2025 by ${git_name} email: ${git_email}, All Rights Reserved.
 */

#pragma once
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "HttpTypes.hpp"

namespace http::middleware {
    class Middleware {
    public:
        virtual ~Middleware() = default;
        virtual void before(HttpRequest& req) = 0;
        virtual void after(HttpResponse& res) = 0;
    };
}