/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-20 21:31:00
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-20 21:32:06
 * @FilePath: /cyclone-HTTPServer/WebApps/MemeServer/backend/src/handlers/UserHandler.cpp
 * @Description: 用户处理器实现
 * Copyright (c) 2025 by cyclone, All Rights Reserved.
 */
#include "../../include/handlers/UserHandler.hpp"
#include <muduo/base/Logging.h>

namespace meme {

// 获取用户资料
void UserHandler::handleGetProfile(const http::HttpRequest& req, http::HttpResponse* resp) {
    try {
        // 检查用户是否登录
        std::string userId = getCurrentUserId(req);
        if (userId.empty()) {
            JsonResponse::unauthorized(resp);
            return;
        }

        nlohmann::json profile = getUserProfile(userId);
        if (profile.is_null()) {
            JsonResponse::error(resp, 404, "用户不存在");
            return;
        }

        nlohmann::json titles = getUserTitles(userId);
        profile["titles"] = titles;

        JsonResponse::success(resp, profile);

    } catch (const std::exception& e) {
        LOG_ERROR << "Get profile error: " << e.what();
        JsonResponse::serverError(resp);
    }
}

// 获取测试历史
void UserHandler::handleGetHistory(const http::HttpRequest& req, http::HttpResponse* resp) {
    try {
        // 检查用户是否登录
        std::string userId = getCurrentUserId(req);
        if (userId.empty()) {
            JsonResponse::unauthorized(resp);
            return;
        }

        // 解析分页参数
        int page = 1;
        int limit = 10;

        std::string pageStr = req.getQueryParam("page");
        if (!pageStr.empty()) {
            try {
                page = std::stoi(pageStr);
                if (page < 1) page = 1;
            } catch (const std::exception&) {
                page = 1;
            }
        }

        std::string limitStr = req.getQueryParam("limit");
        if (!limitStr.empty()) {
            try {
                limit = std::stoi(limitStr);
                if (limit < 1 || limit > 50) limit = 10;
            } catch (const std::exception&) {
                limit = 10;
            }
        }

        nlohmann::json history = getUserHistory(userId, page, limit);

        // 获取总数
        auto totalResult = db_->executeQuery(
            "SELECT COUNT(*) as total FROM test_records tr JOIN users u ON tr.user_id = u.id WHERE u.user_id = ?",
            userId);
        int total = 0;
        if (totalResult && totalResult->next()) {
            total = totalResult->getInt("total");
        }

        nlohmann::json data = {
            {"history", history},
            {"total", total},
            {"page", page},
            {"limit", limit}
        };

        JsonResponse::success(resp, data);

    } catch (const std::exception& e) {
        LOG_ERROR << "Get history error: " << e.what();
        JsonResponse::serverError(resp);
    }
}

// 获取当前登录用户ID
std::string UserHandler::getCurrentUserId(const http::HttpRequest& req) {
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

// 获取用户基本信息
nlohmann::json UserHandler::getUserProfile(const std::string& userId) {
    try {
        auto result = db_->executeQuery(
            "SELECT user_id, nickname, avatar, total_score, test_count FROM users WHERE user_id = ?",
            userId);

        if (result && result->next()) {
            // 获取当前称号
            auto titleResult = db_->executeQuery(
                "SELECT t.title_name FROM titles t "
                "JOIN user_titles ut ON t.id = ut.title_id "
                "JOIN users u ON ut.user_id = u.id "
                "WHERE u.user_id = ? "
                "ORDER BY t.min_score DESC LIMIT 1",
                userId);

            std::string currentTitle = "梗王新手";
            if (titleResult && titleResult->next()) {
                currentTitle = titleResult->getString("title_name");
            }

            return {
                {"user_id", result->getString("user_id")},
                {"nickname", result->getString("nickname")},
                {"avatar", result->getString("avatar")},
                {"total_score", result->getInt("total_score")},
                {"test_count", result->getInt("test_count")},
                {"current_title", currentTitle}
            };
        }

    } catch (const std::exception& e) {
        LOG_ERROR << "Get user profile error: " << e.what();
    }

    return nullptr;
}

// 获取用户称号列表
nlohmann::json UserHandler::getUserTitles(const std::string& userId) {
    try {
        auto result = db_->executeQuery(
            "SELECT t.title_name FROM titles t "
            "JOIN user_titles ut ON t.id = ut.title_id "
            "JOIN users u ON ut.user_id = u.id "
            "WHERE u.user_id = ? "
            "ORDER BY ut.achieved_at DESC",
            userId);

        nlohmann::json titles = nlohmann::json::array();
        if (result) {
            while (result->next()) {
                titles.push_back(result->getString("title_name"));
            }
        }

        return titles;

    } catch (const std::exception& e) {
        LOG_ERROR << "Get user titles error: " << e.what();
        return nlohmann::json::array();
    }
}

// 获取用户测试历史
nlohmann::json UserHandler::getUserHistory(const std::string& userId, int page, int limit) {
    try {
        int offset = (page - 1) * limit;

        auto result = db_->executeQuery(
            "SELECT tr.score, tr.total_questions, tr.correct_answers, tr.category, "
            "tr.test_time, t.title_name as title_achieved "
            "FROM test_records tr "
            "JOIN users u ON tr.user_id = u.id "
            "LEFT JOIN titles t ON tr.score >= t.min_score AND (tr.score <= t.max_score OR t.max_score IS NULL) "
            "WHERE u.user_id = ? "
            "ORDER BY tr.test_time DESC "
            "LIMIT ? OFFSET ?",
            userId, limit, offset);

        nlohmann::json history = nlohmann::json::array();
        if (result) {
            while (result->next()) {
                nlohmann::json record = {
                    {"score", result->getInt("score")},
                    {"total_questions", result->getInt("total_questions")},
                    {"correct_answers", result->getInt("correct_answers")},
                    {"category", result->getString("category")},
                    {"test_time", result->getString("test_time")},
                    {"title_achieved", result->getString("title_achieved")}
                };
                history.push_back(record);
            }
        }

        return history;

    } catch (const std::exception& e) {
        LOG_ERROR << "Get user history error: " << e.what();
        return nlohmann::json::array();
    }
}

} // namespace meme