/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-16 16:10:49
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-22 16:56:25
 * @FilePath: /cyclone-HTTPServer/HttpServer/src/CorsMiddleware.cpp
 * @Description:
 * Copyright (c) 2025 by ${git_name} email: ${git_email}, All Rights Reserved.
 */

#include "../include/CorsMiddleware.hpp"

#include <muduo/base/Logging.h>

#include <sstream>

namespace http::middleware {

CorsMiddleware::CorsMiddleware(CorsConfig config) : config_(std::move(config)) {}

void CorsMiddleware::before(HttpRequest& request) {
    LOG_DEBUG << "CorsMiddleware::before - Processing request";

    // 对于options预检请求，不需要路由处理，直接在此处构造请求返回,通过throw返回到handleRequest的catch块。
    if (request.method() == HttpMethod::COptions) {
        LOG_INFO << "Processing CORS preflight request";
        HttpResponse response;
        handlePreflightRequest(request, response);
        throw response;
    }
}
auto CorsMiddleware::isOriginAllowed(const std::string& origin) const -> bool {
    return config_.allowedOrigins.empty() ||
           std::find(config_.allowedOrigins.begin(), config_.allowedOrigins.end(), "*") !=
               config_.allowedOrigins.end() ||
           std::find(config_.allowedOrigins.begin(), config_.allowedOrigins.end(), origin) !=
               config_.allowedOrigins.end();
}

void CorsMiddleware::handlePreflightRequest(const HttpRequest& request, HttpResponse& response) {
    // 预检请求就检查对方的源我们是否允许，不允许返回状态码403，允许就添加跨域请求头五大件,这样对方浏览器就知道了我们允许访问
    const std::string& origin = request.getHeader("Origin");
    // 如果没有Origin头，使用通配符处理
    std::string effectiveOrigin = origin.empty() ? "*" : origin;

    if (!origin.empty() && !isOriginAllowed(origin)) {
        LOG_WARN << "Origin not allowed: " << origin;
        response.setStatusLine(request.version(), HttpStatusCode::C403Forbidden, "Forbidden");
        response.setBody("Origin not allowed");
        response.setContentType("text/plain");
        return;
    }

    addCorsHeaders(response, effectiveOrigin);
    response.setStatusCode(HttpStatusCode::C204NoContent);
    response.setStatusMessage("No Content");
    LOG_INFO << "Preflight request processed successfully";
}

void CorsMiddleware::addCorsHeaders(HttpResponse& response, const std::string& origin) {
    try {
        // 检查origin是否为空
        if (!origin.empty()) {
            response.addHeader("Access-Control-Allow-Origin", origin);
        }

        if (config_.allowCredentials) {
            response.addHeader("Access-Control-Allow-Credentials", "true");
        }

        if (!config_.allowedMethods.empty()) {
            response.addHeader("Access-Control-Allow-Methods", join(config_.allowedMethods, ", "));
        }

        if (!config_.allowedHeaders.empty()) {
            response.addHeader("Access-Control-Allow-Headers", join(config_.allowedHeaders, ", "));
        }

        response.addHeader("Access-Control-Max-Age", std::to_string(config_.maxAge));

        LOG_DEBUG << "CORS headers added successfully";
    } catch (const std::exception& e) {
        LOG_ERROR << "Error adding CORS headers: " << e.what();
    }
}
// 工具函数：将字符串数组连接成单个字符串
auto CorsMiddleware::join(const std::vector<std::string>& strings, const std::string& delimiter)
    -> std::string {
    std::ostringstream result;
    for (size_t i = 0; i < strings.size(); ++i) {
        if (i > 0) {
            result << delimiter;
        }
        result << strings[i];
    }
    return result.str();
}
// 设计目的：在响应处理完成后添加CORS头部信息
// 这是CORS中间件的后置处理方法，用于在实际业务逻辑处理完响应后，
// 为跨域请求添加必要的CORS响应头，确保浏览器能够正确处理跨域响应
auto CorsMiddleware::after(HttpResponse& response) -> void {
    LOG_DEBUG << "CorsMiddleware::after - Processing response";

    // 对于预检请求（204），已经添加了CORS头，不需要再次添加
    if (response.getStatusCode() == HttpStatusCode::C204NoContent) {
        return;
    }

    // 直接添加CORS头，简化处理逻辑
    if (!config_.allowedOrigins.empty()) {
        // 如果允许所有源
        if (std::find(config_.allowedOrigins.begin(), config_.allowedOrigins.end(), "*") !=
            config_.allowedOrigins.end()) {
            addCorsHeaders(response, "*");
        } else {
            // 添加第一个允许的源（确保索引有效）
            if (!config_.allowedOrigins.empty()) {
                addCorsHeaders(response, config_.allowedOrigins[0]);
            }
        }
    }
}
}  // namespace http::middleware
