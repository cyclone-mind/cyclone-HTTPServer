/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-20 21:29:00
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-20 21:29:00
 * @FilePath: /cyclone-HTTPServer/WebApps/MemeServer/backend/src/handlers/TestHandler.cpp
 * @Description: 测试处理器实现
 * Copyright (c) 2025 by cyclone, All Rights Reserved.
 */
#include "../../include/handlers/TestHandler.hpp"
#include <muduo/base/Logging.h>
#include <algorithm>
#include <random>
#include <ctime>

namespace meme {

// 获取测试题目
void TestHandler::handleGetQuestions(const http::HttpRequest& req, http::HttpResponse* resp) {
    try {
        // 检查用户是否登录
        std::string userId = getCurrentUserId(req);
        if (userId.empty()) {
            JsonResponse::unauthorized(resp);
            return;
        }

        // 解析查询参数
        int count = 10; // 默认10道题
        std::string category = "";

        std::string countStr = const_cast<http::HttpRequest&>(req).getQueryParam("count");
        if (!countStr.empty()) {
            try {
                count = std::stoi(countStr);
            } catch (const std::exception&) {
                count = 10;
            }
        }

        category = const_cast<http::HttpRequest&>(req).getQueryParam("category");

        // 限制题目数量
        if (count <= 0 || count > 50) {
            count = 10;
        }

        nlohmann::json questions = getRandomQuestions(count, category);
        nlohmann::json data = {
            {"questions", questions}
        };

        JsonResponse::success(resp, data);

    } catch (const std::exception& e) {
        LOG_ERROR << "Get questions error: " << e.what();
        JsonResponse::serverError(resp);
    }
}

// 提交测试答案
void TestHandler::handleSubmitTest(const http::HttpRequest& req, http::HttpResponse* resp) {
    try {
        // 检查用户是否登录
        std::string userId = getCurrentUserId(req);
        if (userId.empty()) {
            JsonResponse::unauthorized(resp);
            return;
        }

        // 解析请求体
        nlohmann::json requestBody;
        if (!JsonResponse::parseJsonBody(req.getBody(), requestBody)) {
            JsonResponse::badRequest(resp, "无效的JSON格式");
            return;
        }

        auto answers = requestBody.value("answers", nlohmann::json::array());
        if (answers.empty()) {
            JsonResponse::badRequest(resp, "答案不能为空");
            return;
        }

        // 计算测试结果
        nlohmann::json result = calculateTestResult(answers);
        int score = result["score"];
        int totalQuestions = result["total_questions"];
        int correctAnswers = result["correct_answers"];

        // 保存测试记录
        if (!saveTestRecord(userId, score, totalQuestions, correctAnswers)) {
            LOG_ERROR << "Failed to save test record for user: " << userId;
        }

        // 更新用户总分
        if (!updateUserScore(userId, score)) {
            LOG_ERROR << "Failed to update user score for user: " << userId;
        }

        // 获取称号
        std::string title = getUserTitle(score);

        nlohmann::json data = {
            {"score", score},
            {"total_questions", totalQuestions},
            {"correct_answers", correctAnswers},
            {"title", title},
            {"results", result["results"]}
        };

        JsonResponse::success(resp, data, "测试完成");

    } catch (const std::exception& e) {
        LOG_ERROR << "Submit test error: " << e.what();
        JsonResponse::serverError(resp);
    }
}

// 重新开始测试
void TestHandler::handleRestartTest(const http::HttpRequest& req, http::HttpResponse* resp) {
    try {
        // 检查用户是否登录
        std::string userId = getCurrentUserId(req);
        if (userId.empty()) {
            JsonResponse::unauthorized(resp);
            return;
        }

        // 这里可以添加一些清理逻辑，比如清除之前的测试状态
        // 目前简单返回成功
        JsonResponse::success(resp, nullptr, "测试已重置");

    } catch (const std::exception& e) {
        LOG_ERROR << "Restart test error: " << e.what();
        JsonResponse::serverError(resp);
    }
}

// 获取当前登录用户ID
std::string TestHandler::getCurrentUserId(const http::HttpRequest& req) {
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

// 获取随机题目
nlohmann::json TestHandler::getRandomQuestions(int count, const std::string& category) {
    try {
        std::string sql;
        if (category.empty()) {
            sql = "SELECT q.id, q.question_text, q.option_a, q.option_b, q.option_c, q.option_d, "
                  "m.title as meme_title, m.category "
                  "FROM questions q "
                  "JOIN memes m ON q.meme_id = m.id "
                  "ORDER BY RAND() LIMIT ?";
        } else {
            sql = "SELECT q.id, q.question_text, q.option_a, q.option_b, q.option_c, q.option_d, "
                  "m.title as meme_title, m.category "
                  "FROM questions q "
                  "JOIN memes m ON q.meme_id = m.id "
                  "WHERE m.category = ? "
                  "ORDER BY RAND() LIMIT ?";
        }

        auto result = category.empty() ?
            db_->executeQuery(sql, count) :
            db_->executeQuery(sql, category, count);

        nlohmann::json questions = nlohmann::json::array();

        if (result) {
            while (result->next()) {
                nlohmann::json question = {
                    {"id", result->getInt("id")},
                    {"question_text", result->getString("question_text")},
                    {"options", {
                        {"A", result->getString("option_a")},
                        {"B", result->getString("option_b")},
                        {"C", result->getString("option_c")},
                        {"D", result->getString("option_d")}
                    }},
                    {"meme", {
                        {"title", result->getString("meme_title")},
                        {"category", result->getString("category")}
                    }}
                };
                questions.push_back(question);
            }
        }

        return questions;

    } catch (const std::exception& e) {
        LOG_ERROR << "Get random questions error: " << e.what();
        return nlohmann::json::array();
    }
}

// 计算测试结果
nlohmann::json TestHandler::calculateTestResult(const nlohmann::json& answers) {
    int totalScore = 0;
    int correctCount = 0;
    nlohmann::json results = nlohmann::json::array();

    for (const auto& answer : answers) {
        int questionId = answer.value("question_id", 0);
        std::string userAnswer = answer.value("answer", "");

        if (questionId == 0 || userAnswer.empty()) {
            continue;
        }

        std::string explanation;
        bool isCorrect = checkAnswer(questionId, userAnswer, explanation);

        if (isCorrect) {
            correctCount++;
            totalScore += 10; // 每题10分
        }

        nlohmann::json result = {
            {"question_id", questionId},
            {"correct", isCorrect},
            {"explanation", explanation}
        };
        results.push_back(result);
    }

    return {
        {"score", totalScore},
        {"total_questions", static_cast<int>(answers.size())},
        {"correct_answers", correctCount},
        {"results", results}
    };
}

// 保存测试记录
bool TestHandler::saveTestRecord(const std::string& userId, int score, int totalQuestions,
                               int correctAnswers, const std::string& category) {
    try {
        // 获取用户ID
        auto userResult = db_->executeQuery("SELECT id FROM users WHERE user_id = ?", userId);
        if (!userResult || !userResult->next()) {
            return false;
        }
        int dbUserId = userResult->getInt("id");

        std::string sql = "INSERT INTO test_records (user_id, score, total_questions, correct_answers, category) "
                         "VALUES (?, ?, ?, ?, ?)";
        int result = db_->executeUpdate(sql, dbUserId, score, totalQuestions, correctAnswers, category);
        return result > 0;

    } catch (const std::exception& e) {
        LOG_ERROR << "Save test record error: " << e.what();
        return false;
    }
}

// 更新用户总分
bool TestHandler::updateUserScore(const std::string& userId, int newScore) {
    try {
        std::string sql = "UPDATE users SET total_score = total_score + ?, test_count = test_count + 1 "
                         "WHERE user_id = ?";
        int result = db_->executeUpdate(sql, newScore, userId);
        return result > 0;

    } catch (const std::exception& e) {
        LOG_ERROR << "Update user score error: " << e.what();
        return false;
    }
}

// 获取用户称号
std::string TestHandler::getUserTitle(int score) {
    try {
        auto result = db_->executeQuery(
            "SELECT title_name FROM titles WHERE min_score <= ? AND (max_score IS NULL OR max_score >= ?) "
            "ORDER BY min_score DESC LIMIT 1",
            score, score);

        if (result && result->next()) {
            return result->getString("title_name");
        }

    } catch (const std::exception& e) {
        LOG_ERROR << "Get user title error: " << e.what();
    }

    return "梗王新手"; // 默认称号
}

// 检查答案是否正确
bool TestHandler::checkAnswer(int questionId, const std::string& userAnswer, std::string& explanation) {
    try {
        auto result = db_->executeQuery(
            "SELECT correct_answer, explanation FROM questions WHERE id = ?", questionId);

        if (result && result->next()) {
            std::string correctAnswer = result->getString("correct_answer");
            explanation = result->getString("explanation");
            return userAnswer == correctAnswer;
        }

    } catch (const std::exception& e) {
        LOG_ERROR << "Check answer error: " << e.what();
    }

    return false;
}

} // namespace meme