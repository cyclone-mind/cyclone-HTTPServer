/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-16 13:47:42
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-17 11:18:12
 * @FilePath: /cyclone-HTTPServer/WebApps/SimpleServer/backend/src/handlers/EntryHandler.cpp
 * @Description: 
 * Copyright (c) 2025 by ${git_name} email: ${git_email}, All Rights Reserved.
 */
#include "../../include/handlers/EntryHandler.hpp"
#include "../../include/MemeServer.hpp"
#include <sstream>

void EntryHandler::handle(const http::HttpRequest& req, http::HttpResponse* resp) {
    // 会话管理Demo测试
    auto* sessionManager = server_->getSessionManager();
    
    std::ostringstream htmlResponse;
    htmlResponse << "<!DOCTYPE html>\n";
    htmlResponse << "<html><head><title>会话管理测试</title>";
    htmlResponse << "<style>body{font-family:Arial;margin:40px;} .info{background:#e8f4fd;padding:15px;border-radius:5px;margin:10px 0;} .success{color:#28a745;} .error{color:#dc3545;}</style>";
    htmlResponse << "</head><body>\n";
    htmlResponse << "<h1>🚀 会话管理Demo</h1>\n";
    
    if (sessionManager != nullptr) {
        try {
            // 获取或创建会话
            auto session = sessionManager->getSession(req, resp);
            
            if (session) {
                htmlResponse << "<div class='info success'>✅ 会话管理正常工作!</div>\n";
                htmlResponse << "<p><strong>会话ID:</strong> " << session->getId() << "</p>\n";
                
                // 访问计数器测试
                std::string visitCountStr = session->getValue("visit_count");
                int visitCount = 1;
                if (!visitCountStr.empty()) {
                    visitCount = std::stoi(visitCountStr) + 1;
                }
                session->setValue("visit_count", std::to_string(visitCount));
                
                // 最后访问时间
                std::string lastVisit = session->getValue("last_visit_time");
                session->setValue("last_visit_time", std::to_string(time(nullptr)));
                
                htmlResponse << "<div class='info'>\n";
                htmlResponse << "<p><strong>访问次数:</strong> " << visitCount << "</p>\n";
                if (!lastVisit.empty()) {
                    htmlResponse << "<p><strong>上次访问:</strong> " << lastVisit << " (时间戳)</p>\n";
                }
                htmlResponse << "<p><strong>当前时间:</strong> " << time(nullptr) << " (时间戳)</p>\n";
                htmlResponse << "</div>\n";
                
                // 自定义数据测试
                std::string customData = session->getValue("user_preference");
                if (customData.empty()) {
                    session->setValue("user_preference", "default_theme");
                    customData = "default_theme";
                }
                htmlResponse << "<p><strong>用户偏好:</strong> " << customData << "</p>\n";
                
                // 操作指南
                htmlResponse << "<div class='info'>\n";
                htmlResponse << "<h3>测试说明:</h3>\n";
                htmlResponse << "<ul>\n";
                htmlResponse << "<li>🔄 刷新页面可以看到访问次数增加</li>\n";
                htmlResponse << "<li>🍪 会话数据通过Cookie保持</li>\n";
                htmlResponse << "<li>🔍 打开开发者工具查看Cookie设置</li>\n";
                htmlResponse << "<li>🗑️ 清除浏览器Cookie后再访问会创建新会话</li>\n";
                htmlResponse << "</ul>\n";
                htmlResponse << "</div>\n";
                
            } else {
                htmlResponse << "<div class='info error'>❌ 会话创建失败</div>\n";
            }
        } catch (const std::exception& e) {
            htmlResponse << "<div class='info error'>❌ 会话管理异常: " << e.what() << "</div>\n";
        }
    } else {
        htmlResponse << "<div class='info error'>❌ 会话管理器未配置</div>\n";
    }
    
    // 请求信息
    htmlResponse << "<hr>\n";
    htmlResponse << "<h3>📋 请求信息</h3>\n";
    htmlResponse << "<p><strong>路径:</strong> " << req.path() << "</p>\n";
    htmlResponse << "<p><strong>Cookie:</strong> " << req.getHeader("Cookie") << "</p>\n";
    
    htmlResponse << "</body></html>\n";
    
    std::string responseStr = htmlResponse.str();
    resp->setStatusLine(req.version(), http::HttpStatusCode::C200Ok, "OK");
    resp->setCloseConnection(false);
    resp->setContentType("text/html; charset=utf-8");
    resp->setContentLength(responseStr.size());
    resp->setBody(responseStr);
}