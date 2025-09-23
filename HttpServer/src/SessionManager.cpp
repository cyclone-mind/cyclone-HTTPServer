
#include "../include/SessionManager.hpp"

#include <cstddef>
#include <sstream>

namespace http::session {
auto SessionManager::getSession(const HttpRequest& req, HttpResponse* resp)
    -> std::shared_ptr<Session> {
    // 从请求中获取Cookie头部信息
    std::string cookie = req.getHeader("Cookie");

    std::shared_ptr<Session> session;
    if (!cookie.empty()) {
        // 从Cookie中解析出会话ID
        std::string sessionId = getSessionIdFromCookie(cookie);
        // 尝试从存储中加载现有会话
        session = storage_->load(sessionId);
        if (session == nullptr || session->isExpired()) {
            // 会话不存在或已过期，创建新会话
            sessionId = generateSessionId();
            session = std::make_shared<Session>(sessionId, this);
            // 将新会话ID设置到响应Cookie中（如果resp不为null）
            if (resp != nullptr) {
                setSessionCookie(sessionId, resp);
            }
        } else {
            // 为现有有效会话设置管理器
            session->setManager(this);
        }
    } else {
        // 没有 Cookie，创建新会话
        std::string sessionId = generateSessionId();
        session = std::make_shared<Session>(sessionId, this);
        // 将新会话ID设置到响应Cookie中（如果resp不为null）
        if (resp != nullptr) {
            setSessionCookie(sessionId, resp);
        }
    }
    // 刷新会话的最后访问时间
    session->refresh();
    // 将会话保存到存储中
    storage_->save(session);
    return session;
}

auto SessionManager::getSessionIdFromCookie(const std::string& cookie) -> std::string {
    std::string sessionId;
    if (!cookie.empty()) {
        size_t pos = cookie.find("sessionId=");
        if (pos != std::string::npos) {
            pos += 10;  // 跳过"sessionId="
            size_t end = cookie.find(';', pos);
            if (end != std::string::npos) {
                sessionId = cookie.substr(pos, end - pos);
            } else {
                sessionId = cookie.substr(pos);
            }
        }
    }

    return sessionId;
}
// 生成唯一的会话标识符，确保会话的唯一性和安全性
auto SessionManager::generateSessionId() -> std::string {
    std::stringstream ss;
    std::uniform_int_distribution<> dist(0, 15);

    // 生成32个字符的会话ID，每个字符是一个十六进制数字
    for (int i = 0; i < 32; ++i) {
        ss << std::hex << dist(rng_);
    }
    return ss.str();
}

void SessionManager::setSessionCookie(const std::string& sessionId, HttpResponse* resp)
{
    // 设置会话ID到响应头中，作为Cookie
    std::string cookie = "sessionId=" + sessionId + "; Path=/; HttpOnly";
    resp->addHeader("Set-Cookie", cookie);
}
void SessionManager::destroySession(const std::string& sessionId)
{
    storage_->remove(sessionId);
}
}  // namespace http::session
