/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-16 16:25:29
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-22 16:55:58
 * @FilePath: /cyclone-HTTPServer/HttpServer/include/CorsMiddleware.hpp
 * @Description: 
 * Copyright (c) 2025 by ${git_name} email: ${git_email}, All Rights Reserved.
 */
#pragma once

#include "Middleware.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "CorsConfig.hpp"

namespace http::middleware 
{

class CorsMiddleware : public Middleware
{
public:
     // 构造函数，使用默认或自定义的CORS配置
     explicit CorsMiddleware(CorsConfig config = CorsConfig::defaultConfig());

     // 请求预处理，处理CORS预检请求
     void before(HttpRequest& request) override;
     // 响应后处理，添加CORS响应头
     void after(HttpResponse& response) override;

     // 辅助函数：将字符串数组用指定分隔符连接
     auto static join(const std::vector<std::string>& strings, const std::string& delimiter) -> std::string;

private:
     // 检查请求源是否在允许列表中
     [[nodiscard]] auto isOriginAllowed(const std::string& origin) const -> bool;
     // 处理OPTIONS预检请求
     void handlePreflightRequest(const HttpRequest& request, HttpResponse& response);
     // 添加CORS相关的响应头
     void addCorsHeaders(HttpResponse& response, const std::string& origin);

     CorsConfig config_; // 跨域资源共享配置
};

} // namespace http