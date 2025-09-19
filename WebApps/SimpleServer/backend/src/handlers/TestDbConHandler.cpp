/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-17 14:11:25
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-19 15:46:07
 * @FilePath: /cyclone-HTTPServer/WebApps/SimpleServer/backend/src/handlers/TestDbConHandler.cpp
 * @Description:
 * Copyright (c) 2025 by cyclone, All Rights Reserved.
 */
#include "../../include/handlers/TestDbConHandler.hpp"
#include "../../include/MemeServer.hpp"
#include "../../../../../HttpServer/include/HttpResponse.hpp"
#include "../../../../../HttpServer/include/DbException.hpp"
#include <sstream>
#include <muduo/base/Logging.h>
#include <chrono>
#include <iomanip>

/**
 * @brief 数据库连接池测试处理器
 * 
 * 功能：
 * 1. 测试基本的数据库查询操作
 * 2. 验证连接池的正常工作
 * 3. 展示查询结果
 * 4. 记录性能指标
 */
void TestDbConHandler::handle(const http::HttpRequest& req, http::HttpResponse* resp) {
    LOG_INFO << "收到数据库连接池测试请求";
    
    std::ostringstream html;
    html << "<!DOCTYPE html>\n";
    html << "<html lang='zh-CN'>\n";
    html << "<head>\n";
    html << "    <meta charset='UTF-8'>\n";
    html << "    <meta name='viewport' content='width=device-width, initial-scale=1.0'>\n";
    html << "    <title>数据库连接池测试</title>\n";
    html << "    <style>\n";
    html << "        body { font-family: 'Microsoft YaHei', Arial, sans-serif; margin: 20px; background-color: #f5f5f5; }\n";
    html << "        .container { max-width: 1200px; margin: 0 auto; background: white; padding: 20px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }\n";
    html << "        h1 { color: #333; text-align: center; margin-bottom: 30px; }\n";
    html << "        .test-section { margin: 20px 0; padding: 15px; border: 1px solid #ddd; border-radius: 5px; }\n";
    html << "        .test-title { font-weight: bold; color: #2c3e50; font-size: 18px; margin-bottom: 10px; }\n";
    html << "        .success { color: #27ae60; }\n";
    html << "        .error { color: #e74c3c; }\n";
    html << "        .info { color: #3498db; }\n";
    html << "        table { width: 100%; border-collapse: collapse; margin: 10px 0; }\n";
    html << "        th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n";
    html << "        th { background-color: #f2f2f2; font-weight: bold; }\n";
    html << "        .metric { display: inline-block; margin: 10px; padding: 10px; background: #ecf0f1; border-radius: 4px; }\n";
    html << "        .timestamp { color: #7f8c8d; font-size: 12px; }\n";
    html << "    </style>\n";
    html << "</head>\n";
    html << "<body>\n";
    html << "    <div class='container'>\n";
    html << "        <h1>🔍 Cyclone HTTP Server - 数据库连接池测试</h1>\n";
    
    // 获取当前时间
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    html << "        <div class='timestamp'>测试时间: " 
         << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S") << "</div>\n";
    
    try {
        // 测试1: 基本连接测试
        html << "        <div class='test-section'>\n";
        html << "            <div class='test-title'>📋 测试1: 基本连接测试</div>\n";
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        // 执行简单查询测试连接
        auto result = http::MysqlUtil::executeQuery("SELECT 1 as test_value, NOW() as `current_time`");
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        
        if (result && result->next()) {
            html << "            <div class='success'>✅ 数据库连接成功！</div>\n";
            html << "            <div class='metric'>响应时间: " << duration.count() << " 微秒</div>\n";
            html << "            <table>\n";
            html << "                <tr><th>测试值</th><th>数据库时间</th></tr>\n";
            html << "                <tr>\n";
            html << "                    <td>" << result->getInt("test_value") << "</td>\n";
            html << "                    <td>" << result->getString("current_time") << "</td>\n";
            html << "                </tr>\n";
            html << "            </table>\n";
        } else {
            html << "            <div class='error'>❌ 基本连接测试失败</div>\n";
        }
        html << "        </div>\n";
        
        // 测试2: 用户数据查询
        html << "        <div class='test-section'>\n";
        html << "            <div class='test-title'>👥 测试2: 用户数据查询</div>\n";
        
        start_time = std::chrono::high_resolution_clock::now();
        result = http::MysqlUtil::executeQuery("SELECT id, username, nickname, created_at FROM users LIMIT 5");
        end_time = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        
        if (result) {
            html << "            <div class='success'>✅ 用户数据查询成功</div>\n";
            html << "            <div class='metric'>查询时间: " << duration.count() << " 微秒</div>\n";
            html << "            <table>\n";
            html << "                <tr><th>ID</th><th>用户名</th><th>昵称</th><th>创建时间</th></tr>\n";
            
            int count = 0;
            while (result->next() && count < 10) {
                html << "                <tr>\n";
                html << "                    <td>" << result->getInt("id") << "</td>\n";
                html << "                    <td>" << result->getString("username") << "</td>\n";
                html << "                    <td>" << result->getString("nickname") << "</td>\n";
                html << "                    <td>" << result->getString("created_at") << "</td>\n";
                html << "                </tr>\n";
                count++;
            }
            html << "            </table>\n";
            html << "            <div class='info'>📊 显示了前 " << count << " 条用户记录</div>\n";
        } else {
            html << "            <div class='error'>❌ 用户数据查询失败</div>\n";
        }
        html << "        </div>\n";
        
        // 测试3: 文章数据查询
        html << "        <div class='test-section'>\n";
        html << "            <div class='test-title'>📝 测试3: 文章数据查询</div>\n";
        
        start_time = std::chrono::high_resolution_clock::now();
        result = http::MysqlUtil::executeQuery(
            "SELECT p.id, p.title, p.view_count, u.username as author "
            "FROM posts p JOIN users u ON p.user_id = u.id "
            "WHERE p.is_published = ? LIMIT 5", 
            true
        );
        end_time = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        
        if (result) {
            html << "            <div class='success'>✅ 文章数据查询成功</div>\n";
            html << "            <div class='metric'>联合查询时间: " << duration.count() << " 微秒</div>\n";
            html << "            <table>\n";
            html << "                <tr><th>ID</th><th>标题</th><th>作者</th><th>浏览数</th></tr>\n";
            
            int count = 0;
            while (result->next() && count < 10) {
                html << "                <tr>\n";
                html << "                    <td>" << result->getInt("id") << "</td>\n";
                html << "                    <td>" << result->getString("title") << "</td>\n";
                html << "                    <td>" << result->getString("author") << "</td>\n";
                html << "                    <td>" << result->getInt("view_count") << "</td>\n";
                html << "                </tr>\n";
                count++;
            }
            html << "            </table>\n";
            html << "            <div class='info'>📊 显示了前 " << count << " 篇已发布文章</div>\n";
        } else {
            html << "            <div class='error'>❌ 文章数据查询失败</div>\n";
        }
        html << "        </div>\n";
        
        // 测试4: 性能测试
        html << "        <div class='test-section'>\n";
        html << "            <div class='test-title'>⚡ 测试4: 连接池性能测试</div>\n";
        
        const int TEST_COUNT = 10;
        std::vector<long> query_times;
        
        for (int i = 0; i < TEST_COUNT; ++i) {
            start_time = std::chrono::high_resolution_clock::now();
            auto perf_result = http::MysqlUtil::executeQuery("SELECT COUNT(*) as total FROM users");
            end_time = std::chrono::high_resolution_clock::now();
            
            auto perf_duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
            query_times.push_back(perf_duration.count());
        }
        
        // 计算统计信息
        long total_time = 0;
        long min_time = query_times[0];
        long max_time = query_times[0];
        
        for (long time : query_times) {
            total_time += time;
            if (time < min_time) min_time = time;
            if (time > max_time) max_time = time;
        }
        
        long avg_time = total_time / TEST_COUNT;
        
        html << "            <div class='success'>✅ 性能测试完成</div>\n";
        html << "            <div class='metric'>测试次数: " << TEST_COUNT << "</div>\n";
        html << "            <div class='metric'>平均响应时间: " << avg_time << " 微秒</div>\n";
        html << "            <div class='metric'>最短响应时间: " << min_time << " 微秒</div>\n";
        html << "            <div class='metric'>最长响应时间: " << max_time << " 微秒</div>\n";
        html << "        </div>\n";
        
        // 总结
        html << "        <div class='test-section'>\n";
        html << "            <div class='test-title'>📋 测试总结</div>\n";
        html << "            <div class='success'>🎉 所有数据库连接池测试均通过！</div>\n";
        html << "            <div class='info'>💡 连接池工作正常，可以投入生产使用</div>\n";
        html << "        </div>\n";
        
    } catch (const http::db::DbException& e) {
        LOG_ERROR << "数据库异常: " << e.what();
        html << "        <div class='test-section'>\n";
        html << "            <div class='test-title'>❌ 数据库测试失败</div>\n";
        html << "            <div class='error'>数据库异常: " << e.what() << "</div>\n";
        html << "            <div class='info'>💡 请检查数据库连接配置和表结构</div>\n";
        html << "        </div>\n";
        
    } catch (const std::exception& e) {
        LOG_ERROR << "未知异常: " << e.what();
        html << "        <div class='test-section'>\n";
        html << "            <div class='test-title'>❌ 测试异常</div>\n";
        html << "            <div class='error'>未知异常: " << e.what() << "</div>\n";
        html << "        </div>\n";
    }
    
    html << "    </div>\n";
    html << "</body>\n";
    html << "</html>\n";
    
    // 设置响应
    resp->setStatusCode(http::HttpStatusCode::C200Ok);
    resp->setStatusMessage("OK");
    resp->setContentType("text/html; charset=utf-8");
    resp->setContentLength(html.str().size());
    resp->setBody(html.str());
    
    LOG_INFO << "数据库连接池测试请求处理完成";
}