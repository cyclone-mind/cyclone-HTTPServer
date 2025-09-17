/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-16 20:52:19
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-17 10:50:31
 * @FilePath: /cyclone-HTTPServer/HttpServer/include/SessionStorage.hpp
 * @Description:
 * Copyright (c) 2025 by ${git_name} email: ${git_email}, All Rights Reserved.
 */
#pragma once

#include <memory>
#include <unordered_map>

#include "Session.hpp"
namespace http::session {
class SessionStorage {
public:
    virtual ~SessionStorage() = default;
    virtual void save(std::shared_ptr<Session> session) = 0;
    virtual std::shared_ptr<Session> load(const std::string& sessionId) = 0;
    virtual void remove(const std::string& sessionId) = 0;
};

/**
 * @brief 基于内存的会话存储实现
 */
class MemorySessionStorage : public SessionStorage {
public:

    void save(std::shared_ptr<Session> session) override {
        // 使用会话ID作为键，将会话对象存储到哈希表中
        sessions_[session->getId()] = session;
    }
    
    /**
     * @brief 从内存存储中加载会话
     * @param sessionId 会话ID
     * @return 返回会话对象，如果不存在或已过期则返回nullptr
     */
    auto load(const std::string& sessionId) -> std::shared_ptr<Session> override {
        // 查找指定ID的会话
        auto it = sessions_.find(sessionId);
        if (it != sessions_.end()) {
            // 检查会话是否已过期
            if (!it->second->isExpired()) {
                // 会话有效，返回会话对象
                return it->second;
            }  
            // 如果会话已过期，则从存储中移除
            sessions_.erase(it);
        }

        // 如果会话不存在或已过期，则返回nullptr
        return nullptr;
    };
    

    void remove(const std::string& sessionId) override {
        sessions_.erase(sessionId);
    };

private:
    /**
     * @brief 会话存储容器
     * 使用无序哈希表存储会话，键为会话ID，值为会话对象的智能指针
     */
    std::unordered_map<std::string, std::shared_ptr<Session>> sessions_; 
};
}  // namespace http::session