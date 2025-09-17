/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-16 20:51:25
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-17 10:57:26
 * @FilePath: /cyclone-HTTPServer/HttpServer/include/SessionManager.hpp
 * @Description:
 * Copyright (c) 2025 by ${git_name} email: ${git_email}, All Rights Reserved.
 */
#pragma once
#include <memory>
#include <random>
#include <utility>

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Session.hpp"
#include "SessionStorage.hpp"
namespace http::session {
class SessionManager {
public:
    SessionManager() = default;
    ~SessionManager() = default;
    SessionManager(const SessionManager&) = delete;
    auto operator=(const SessionManager&) -> SessionManager& = delete;
    SessionManager(SessionManager&&) = delete;
    auto operator=(SessionManager&&) -> SessionManager& = delete;

    // 初始化会话管理器，设置会话存储对象和随机数生成器
    explicit SessionManager(std::unique_ptr<SessionStorage> storage)
        : storage_(std::move(storage)),
          rng_(std::random_device{}())  // 初始化随机数生成器，用于生成随机的会话ID
    {}
    auto getSession(const HttpRequest& req, HttpResponse* resp) -> std::shared_ptr<Session>;

    // 销毁会话
    void destroySession(const std::string& sessionId);

    // 清理过期会话
    void cleanExpiredSessions();
    // 会话是主题，管理者是监听对象，会话发生了改变，要通知管理者
    void updateSession(std::shared_ptr<Session> session) {
        storage_->save(std::move(session));
    }

private:
    auto static getSessionIdFromCookie(const std::string& cookie) -> std::string;
    auto generateSessionId() -> std::string;
    void setSessionCookie(const std::string& sessionId, HttpResponse* resp);
    std::unique_ptr<SessionStorage> storage_;
    std::mt19937 rng_;  // 用于生成随机会话id
};
}  // namespace http::session