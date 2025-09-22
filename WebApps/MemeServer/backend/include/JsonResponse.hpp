/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-20 21:27:00
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-20 21:27:31
 * @FilePath: /cyclone-HTTPServer/WebApps/MemeServer/backend/include/JsonResponse.hpp
 * @Description: JSON响应工具类
 * Copyright (c) 2025 by cyclone, All Rights Reserved.
 */
#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include "../../../../HttpServer/include/HttpResponse.hpp"

namespace meme {

class JsonResponse {
public:
    // 成功响应
    static void success(http::HttpResponse* resp, const nlohmann::json& data = nullptr,
                       const std::string& message = "success") {
        nlohmann::json response = {
            {"code", 200},
            {"message", message}
        };

        if (!data.is_null()) {
            response["data"] = data;
        }

        resp->setStatusCode(http::HttpStatusCode::C200Ok);
        resp->setContentType("application/json");
        resp->setBody(response.dump(2));
        resp->setContentLength(resp->getBody().size());
    }

    // 错误响应
    static void error(http::HttpResponse* resp, int code, const std::string& message) {
        nlohmann::json response = {
            {"code", code},
            {"message", message}
        };

        resp->setStatusCode(http::HttpStatusCode::C200Ok); // 业务错误用200状态码，通过code字段区分
        resp->setContentType("application/json");
        resp->setBody(response.dump(2));
        resp->setContentLength(resp->getBody().size());
    }

    // 未授权响应
    static void unauthorized(http::HttpResponse* resp, const std::string& message = "未授权访问") {
        error(resp, 401, message);
    }

    // 参数错误响应
    static void badRequest(http::HttpResponse* resp, const std::string& message = "参数错误") {
        error(resp, 400, message);
    }

    // 服务器错误响应
    static void serverError(http::HttpResponse* resp, const std::string& message = "服务器内部错误") {
        error(resp, 500, message);
    }

    // 解析请求体JSON
    static bool parseJsonBody(const std::string& body, nlohmann::json& json) {
        try {
            json = nlohmann::json::parse(body);
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }
};

} // namespace meme