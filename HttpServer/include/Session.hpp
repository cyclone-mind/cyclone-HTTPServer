/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-16 20:51:16
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-17 11:14:35
 * @FilePath: /cyclone-HTTPServer/HttpServer/include/Session.hpp
 * @Description: 
 * Copyright (c) 2025 by ${git_name} email: ${git_email}, All Rights Reserved.
 */
#pragma once
#include <chrono>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

namespace http::session {
class SessionManager;
/*
std::enable_shared_from_this 的作用：
它允许对象从其成员函数中安全地获取指向自身的 shared_ptr
通过调用 shared_from_this() 方法可以获得管理当前对象的 shared_ptr
这在需要将对象自身传递给异步操作或回调函数时非常有用，确保对象在操作完成前不会被销毁
*/
class Session : public std::enable_shared_from_this<Session> {
private:
    std::string sessionId_;                              // 会话的唯一标识符
    std::unordered_map<std::string, std::string> data_;  // 存储会话数据的键值对映射
    std::chrono::system_clock::time_point expiryTime_;   // 会话过期时间点
    int maxAge_;                                         // 会话最大生存时间（秒）
    SessionManager* sessionManager_;                     // 指向会话管理器的指针
public:
    Session(std::string sessionId, SessionManager* sessionManager, int maxAge = 3600)
        : sessionId_(std::move(sessionId)), maxAge_(maxAge), sessionManager_(sessionManager) {}

    // 获取会话ID
    auto getId() const -> const std::string& {
        return sessionId_;
    }
    // 检查是否过期
    auto isExpired() const -> bool {
        return std::chrono::system_clock::now() > expiryTime_;
    }
    // 设置会话管理器
    void setManager(SessionManager* sessionManager) {
        sessionManager_ = sessionManager;
    }   
    // 获取会话管理器
    auto getManager() const -> SessionManager* {
        return sessionManager_;
    }
    // 数据存取
    void setValue(const std::string& key, const std::string& value);
    auto getValue(const std::string& key) const -> std::string {
        auto it = data_.find(key);
        return it != data_.end() ? it->second : std::string();
    };
    
    // 清空session中某个数据
    void remove(const std::string& key) {
        data_.erase(key);
    };
    // 清空session所有数据
    void clear() {
        data_.clear();
    };
    // 刷新session过期时间
    void refresh() {
        expiryTime_ = std::chrono::system_clock::now() + std::chrono::seconds(maxAge_);
    }
};
}  // namespace http::session