/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-20 21:29:00
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-20 21:29:53
 * @FilePath: /cyclone-HTTPServer/WebApps/MemeServer/backend/include/handlers/TestHandler.hpp
 * @Description: 测试处理器
 * Copyright (c) 2025 by cyclone, All Rights Reserved.
 */
#pragma once

#include <memory>
#include <nlohmann/json.hpp>
#include "../../../../../HttpServer/include/HttpRequest.hpp"
#include "../../../../../HttpServer/include/HttpResponse.hpp"
#include "../../../../../HttpServer/include/DbConnection.hpp"
#include "../../../../../HttpServer/include/SessionManager.hpp"
#include "../JsonResponse.hpp"

namespace meme {

class TestHandler {
private:
    std::shared_ptr<http::db::DbConnection> db_;
    http::session::SessionManager* sessionManager_;

public:
    TestHandler(std::shared_ptr<http::db::DbConnection> db,
                http::session::SessionManager* sessionManager)
        : db_(db), sessionManager_(sessionManager) {}

    // 获取测试题目
    void handleGetQuestions(const http::HttpRequest& req, http::HttpResponse* resp);

    // 提交测试答案
    void handleSubmitTest(const http::HttpRequest& req, http::HttpResponse* resp);

    // 重新开始测试
    void handleRestartTest(const http::HttpRequest& req, http::HttpResponse* resp);

private:
    // 获取当前登录用户ID
    std::string getCurrentUserId(const http::HttpRequest& req);

    // 获取随机题目
    nlohmann::json getRandomQuestions(int count, const std::string& category = "");

    // 计算测试结果
    nlohmann::json calculateTestResult(const nlohmann::json& answers);

    // 保存测试记录
    bool saveTestRecord(const std::string& userId, int score, int totalQuestions,
                       int correctAnswers, const std::string& category = "");

    // 更新用户总分
    bool updateUserScore(const std::string& userId, int newScore);

    // 获取用户称号
    std::string getUserTitle(int score);

    // 检查答案是否正确
    bool checkAnswer(int questionId, const std::string& userAnswer, std::string& explanation);
};

} // namespace meme