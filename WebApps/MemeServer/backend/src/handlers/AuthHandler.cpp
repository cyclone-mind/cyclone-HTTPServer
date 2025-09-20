/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-20 21:28:00
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-20 21:35:45
 * @FilePath: /cyclone-HTTPServer/WebApps/MemeServer/backend/src/handlers/AuthHandler.cpp
 * @Description: 认证处理器实现
 * Copyright (c) 2025 by cyclone, All Rights Reserved.
 */
#include "../../include/handlers/AuthHandler.hpp"
#include <muduo/base/Logging.h>

namespace meme {

// 用户注册
void AuthHandler::handleRegister(const http::HttpRequest& req, http::HttpResponse* resp) {
    try {
        nlohmann::json requestBody;
        if (!JsonResponse::parseJsonBody(req.getBody(), requestBody)) {
            JsonResponse::badRequest(resp, "无效的JSON格式");
            return;
        }

        std::string userId = requestBody.value("user_id", "");
        std::string password = requestBody.value("password", "");
        std::string nickname = requestBody.value("nickname", "");

        if (userId.empty() || password.empty()) {
            JsonResponse::badRequest(resp, "用户名和密码不能为空");
            return;
        }

        // 检查用户ID是否已存在
        if (userExists(userId)) {
            JsonResponse::error(resp, 409, "用户ID已存在");
            return;
        }

        // 创建新用户
        if (!createUser(userId, password, nickname)) {
            JsonResponse::serverError(resp, "注册失败");
            return;
        }

        nlohmann::json data = {
            {"user_id", userId},
            {"nickname", nickname}
        };
        JsonResponse::success(resp, data, "注册成功");

    } catch (const std::exception& e) {
        LOG_ERROR << "Register error: " << e.what();
        JsonResponse::serverError(resp);
    }
}

// 用户登录
void AuthHandler::handleLogin(const http::HttpRequest& req, http::HttpResponse* resp) {
    try {
        nlohmann::json requestBody;
        if (!JsonResponse::parseJsonBody(req.getBody(), requestBody)) {
            JsonResponse::badRequest(resp, "无效的JSON格式");
            return;
        }

        std::string userId = requestBody.value("user_id", "");
        std::string password = requestBody.value("password", "");

        if (userId.empty() || password.empty()) {
            JsonResponse::badRequest(resp, "用户名和密码不能为空");
            return;
        }

        // 验证用户
        if (!validateUser(userId, password)) {
            JsonResponse::error(resp, 401, "用户名或密码错误");
            return;
        }

        // 获取会话
        auto session = sessionManager_->getSession(req, resp);
        session->setValue("user_id", userId);

        nlohmann::json userInfo = getUserInfo(userId);
        nlohmann::json data = {
            {"user_id", userId},
            {"nickname", userInfo.value("nickname", "")},
            {"session_id", session->getId()}
        };
        JsonResponse::success(resp, data, "登录成功");

    } catch (const std::exception& e) {
        LOG_ERROR << "Login error: " << e.what();
        JsonResponse::serverError(resp);
    }
}

// 用户登出
void AuthHandler::handleLogout(const http::HttpRequest& req, http::HttpResponse* resp) {
    try {
        auto session = sessionManager_->getSession(req, resp);
        if (session) {
            sessionManager_->destroySession(session->getId());
        }
        JsonResponse::success(resp, nullptr, "登出成功");
    } catch (const std::exception& e) {
        LOG_ERROR << "Logout error: " << e.what();
        JsonResponse::serverError(resp);
    }
}

// 检查用户ID是否已存在
bool AuthHandler::userExists(const std::string& userId) {
    try {
        auto result = http::MysqlUtil::executeQuery("SELECT id FROM users WHERE user_id = ?", userId);
        return result && result->next();
    } catch (const std::exception& e) {
        LOG_ERROR << "Check user exists error: " << e.what();
        return false;
    }
}

// 创建新用户
bool AuthHandler::createUser(const std::string& userId, const std::string& password,
                           const std::string& nickname) {
    try {
        std::string sql = "INSERT INTO users (user_id, password, nickname) VALUES (?, ?, ?)";
        int result = http::MysqlUtil::executeUpdate(sql, userId, password, nickname);
        return result > 0;
    } catch (const std::exception& e) {
        LOG_ERROR << "Create user error: " << e.what();
        return false;
    }
}

// 验证用户密码
bool AuthHandler::validateUser(const std::string& userId, const std::string& password) {
    try {
        auto result = http::MysqlUtil::executeQuery("SELECT password FROM users WHERE user_id = ?", userId);
        if (result && result->next()) {
            std::string storedPassword = result->getString("password");
            return storedPassword == password; // 明文比较
        }
        return false;
    } catch (const std::exception& e) {
        LOG_ERROR << "Validate user error: " << e.what();
        return false;
    }
}

// 获取用户信息
nlohmann::json AuthHandler::getUserInfo(const std::string& userId) {
    try {
        auto result = http::MysqlUtil::executeQuery(
            "SELECT nickname, total_score, test_count FROM users WHERE user_id = ?", userId);

        if (result && result->next()) {
            return {
                {"nickname", result->getString("nickname")},
                {"total_score", result->getInt("total_score")},
                {"test_count", result->getInt("test_count")}
            };
        }
    } catch (const std::exception& e) {
        LOG_ERROR << "Get user info error: " << e.what();
    }
    return nullptr;
}

// 获取当前登录用户ID
std::string AuthHandler::getCurrentUserId(const http::HttpRequest& req) {
    try {
        auto session = sessionManager_->getSession(req, nullptr);
        if (session) {
            return session->getValue("user_id");
        }
    } catch (const std::exception& e) {
        LOG_ERROR << "Get current user error: " << e.what();
    }
    return "";
}

} // namespace meme