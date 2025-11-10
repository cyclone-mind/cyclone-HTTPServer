#include "../../include/handlers/ChatHistoryHandler.h"

/**
 * ChatHistoryHandler::handle
 * 处理获取聊天历史记录的请求
 *
 * 功能：
 * 1. 验证用户登录状态
 * 2. 解析会话ID
 * 3. 查找或创建对应的 AIHelper 实例
 * 4. 获取该会话的聊天历史
 * 5. 返回历史记录（JSON数组格式）
 */
void ChatHistoryHandler::handle(const http::HttpRequest& req, http::HttpResponse* resp)
{
    try
    {
        // === 步骤1：判断用户是否登录 ===
        auto session = server_->getSessionManager()->getSession(req, resp);
        LOG_INFO << "session->getValue(\"isLoggedIn\") = " << session->getValue("isLoggedIn");
        if (session->getValue("isLoggedIn") != "true")
        {
            // 用户未登录，无法获取历史记录
            json errorResp;
            errorResp["status"] = "error";
            errorResp["message"] = "Unauthorized";
            std::string errorBody = errorResp.dump(4);

            server_->packageResp(req.version(), http::HttpStatusCode::C401Unauthorized,
                "Unauthorized", true, "application/json", errorBody.size(),
                errorBody, resp);
            return;
        }

        // === 步骤2：获取用户信息 ===
        int userId = std::stoi(session->getValue("userId"));
        std::string username = session->getValue("username");

        // === 步骤3：解析请求体，获取会话ID ===
        std::string sessionId;
        auto body = req.getBody();
        if (!body.empty()) {
        auto j = json::parse(body);
        if (j.contains("sessionId")) sessionId = j["sessionId"];
        }

        // === 步骤4：获取该会话的消息历史（线程安全）===
        std::vector<std::pair<std::string, long long>> messages;

        {
            std::shared_ptr<AIHelper> AIHelperPtr;
            // 加锁保护并发访问
            std::lock_guard<std::mutex> lock(server_->mutexForChatInformation);

            auto& userSessions = server_->chatInformation[userId];

            // 如果该会话不存在，创建新的 AIHelper
            if (userSessions.find(sessionId) == userSessions.end()) {

                userSessions.emplace(
                    sessionId,
                    std::make_shared<AIHelper>()
                );
            }

            AIHelperPtr = server_->chatInformation[userId];
            messages= AIHelperPtr->GetMessages();
        }

        // === 步骤5：构造历史消息数组并返回 ===
        json successResp;
        successResp["success"] = true;
        successResp["history"] = json::array();

        // 遍历消息历史
        // 偶数下标：用户消息，奇数下标：AI回复
        for (size_t i = 0; i < messages.size(); ++i) {
            json msgJson;
            msgJson["is_user"] = (i % 2 == 0);  // 偶数索引是用户消息
            msgJson["content"] = messages[i].first;
            successResp["history"].push_back(msgJson);
        }

        std::string successBody = successResp.dump(4);

        resp->setStatusLine(req.version(), http::HttpStatusCode::C200Ok, "OK");
        resp->setCloseConnection(false);
        resp->setContentType("application/json");
        resp->setContentLength(successBody.size());
        resp->setBody(successBody);
        return;
    }
    catch (const std::exception& e)
    {
        // 处理异常并返回错误信息
        json failureResp;
        failureResp["status"] = "error";
        failureResp["message"] = e.what();
        std::string failureBody = failureResp.dump(4);
        resp->setStatusLine(req.version(), http::HttpStatusCode::C400BadRequest, "Bad Request");
        resp->setCloseConnection(true);
        resp->setContentType("application/json");
        resp->setContentLength(failureBody.size());
        resp->setBody(failureBody);
    }
}









