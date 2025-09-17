/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-17 11:14:50
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-17 11:14:54
 * @FilePath: /cyclone-HTTPServer/HttpServer/src/Session.cpp
 * @Description: 
 * Copyright (c) 2025 by cyclone, All Rights Reserved. 
 */
#include "../include/Session.hpp"
#include "../include/SessionManager.hpp"

namespace http::session {

void Session::setValue(const std::string& key, const std::string& value) {
    data_[key] = value;
    // 如果设置了manager，自动保存更改
    if (sessionManager_ != nullptr) {
        sessionManager_->updateSession(shared_from_this());
    }
}

}  // namespace http::session