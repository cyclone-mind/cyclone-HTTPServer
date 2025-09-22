/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-20 21:31:00
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-20 21:31:29
 * @FilePath: /cyclone-HTTPServer/WebApps/MemeServer/backend/src/handlers/LeaderboardHandler.cpp
 * @Description: 排行榜处理器实现
 * Copyright (c) 2025 by cyclone, All Rights Reserved.
 */
#include "../../include/handlers/LeaderboardHandler.hpp"
#include <muduo/base/Logging.h>

namespace meme {

// 获取总排行榜
void LeaderboardHandler::handleGetTotalLeaderboard(const http::HttpRequest& req, http::HttpResponse* resp) {
    try {
        // 解析分页参数
        int page = 1;
        int limit = 20;

        std::string pageStr = req.getQueryParam("page");
        std::string limitStr = req.getQueryParam("limit");

        LOG_INFO << "Category leaderboard parsed query params - pageStr: '" << pageStr << "', limitStr: '" << limitStr << "'";

        if (!pageStr.empty()) {
            try {
                page = std::stoi(pageStr);
                if (page < 1) page = 1;
            } catch (const std::exception&) {
                page = 1;
            }
        }

        if (!limitStr.empty()) {
            try {
                limit = std::stoi(limitStr);
                if (limit < 1 || limit > 100) limit = 20;
            } catch (const std::exception&) {
                limit = 20;
            }
        }

        LOG_INFO << "Category leaderboard final params - page: " << page << ", limit: " << limit;

        nlohmann::json rankings = getLeaderboardData("", page, limit);
        int total = getLeaderboardTotal("");

        nlohmann::json data = {
            {"rankings", rankings},
            {"total", total},
            {"page", page},
            {"limit", limit}
        };

        JsonResponse::success(resp, data);

    } catch (const std::exception& e) {
        LOG_ERROR << "Get total leaderboard error: " << e.what();
        JsonResponse::serverError(resp);
    }
}

// 获取分类排行榜
void LeaderboardHandler::handleGetCategoryLeaderboard(const http::HttpRequest& req, http::HttpResponse* resp) {
    try {
        // 从路径参数获取分类
        std::string category = req.getPathParam("category");
        if (category.empty()) {
            JsonResponse::badRequest(resp, "分类参数不能为空");
            return;
        }

        // 解析分页参数
        int page = 1;
        int limit = 20;

        std::string pageStr = req.getQueryParam("page");
        std::string limitStr = req.getQueryParam("limit");

        LOG_INFO << "Parsed query params - pageStr: '" << pageStr << "', limitStr: '" << limitStr << "'";

        if (!pageStr.empty()) {
            try {
                page = std::stoi(pageStr);
                if (page < 1) page = 1;
            } catch (const std::exception&) {
                page = 1;
            }
        }

        if (!limitStr.empty()) {
            try {
                limit = std::stoi(limitStr);
                if (limit < 1 || limit > 100) limit = 20;
            } catch (const std::exception&) {
                limit = 20;
            }
        }

        LOG_INFO << "Final params - page: " << page << ", limit: " << limit;

        nlohmann::json rankings = getLeaderboardData(category, page, limit);
        int total = getLeaderboardTotal(category);

        nlohmann::json data = {
            {"rankings", rankings},
            {"total", total},
            {"page", page},
            {"limit", limit},
            {"category", category}
        };

        JsonResponse::success(resp, data);

    } catch (const std::exception& e) {
        LOG_ERROR << "Get category leaderboard error: " << e.what();
        JsonResponse::serverError(resp);
    }
}

// 获取排行榜数据
nlohmann::json LeaderboardHandler::getLeaderboardData(const std::string& category, int page, int limit) {
    try {
        int offset = (page - 1) * limit;
        std::string sql;
        nlohmann::json rankings = nlohmann::json::array();

        if (category.empty()) {
            // 总排行榜
            sql = "SELECT u.user_id, u.nickname, u.total_score, u.test_count "
                  "FROM users u "
                  "ORDER BY u.total_score DESC "
                  "LIMIT ? OFFSET ?";
        } else {
            // 分类排行榜 - 基于该分类的测试记录计算得分
            sql = "SELECT u.user_id, u.nickname, "
                  "COALESCE(SUM(tr.score), 0) as category_score, "
                  "COUNT(tr.id) as category_tests "
                  "FROM users u "
                  "LEFT JOIN test_records tr ON u.id = tr.user_id AND tr.category = ? "
                  "GROUP BY u.id, u.user_id, u.nickname "
                  "ORDER BY category_score DESC "
                  "LIMIT ? OFFSET ?";
        }

        auto result = category.empty() ?
            db_->executeQuery(sql, limit, offset) :
            db_->executeQuery(sql, category, limit, offset);

        if (result) {
            int currentRank = offset + 1;
            while (result->next()) {
                nlohmann::json ranking = {
                    {"rank", currentRank++},
                    {"user_id", result->getString("user_id")},
                    {"nickname", result->getString("nickname")}
                };

                if (category.empty()) {
                    ranking["total_score"] = result->getInt("total_score");
                    ranking["test_count"] = result->getInt("test_count");
                } else {
                    ranking["category_score"] = result->getInt("category_score");
                    ranking["category_tests"] = result->getInt("category_tests");
                }

                rankings.push_back(ranking);
            }
        }

        return rankings;

    } catch (const std::exception& e) {
        LOG_ERROR << "Get leaderboard data error: " << e.what();
        return nlohmann::json::array();
    }
}

// 获取排行榜总数
int LeaderboardHandler::getLeaderboardTotal(const std::string& category) {
    try {
        std::string sql;
        if (category.empty()) {
            sql = "SELECT COUNT(*) as total FROM users WHERE total_score > 0";
            auto result = db_->executeQuery(sql);
            if (result && result->next()) {
                return result->getInt("total");
            }
        } else {
            sql = "SELECT COUNT(DISTINCT u.id) as total "
                  "FROM users u "
                  "JOIN test_records tr ON u.id = tr.user_id AND tr.category = ?";
            auto result = db_->executeQuery(sql, category);
            if (result && result->next()) {
                return result->getInt("total");
            }
        }
        return 0;

    } catch (const std::exception& e) {
        LOG_ERROR << "Get leaderboard total error: " << e.what();
        return 0;
    }
}

} // namespace meme