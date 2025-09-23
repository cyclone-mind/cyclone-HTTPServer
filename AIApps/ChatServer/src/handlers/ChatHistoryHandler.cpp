#include "../../include/handlers/ChatHistoryHandler.h"

void ChatHistoryHandler::handle(const http::HttpRequest& req, http::HttpResponse* resp)
{
    try
    {
        // 判断用户是否登录
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

        // 获取用户信息以及获取用户消息记录
        int userId = std::stoi(session->getValue("userId"));
        std::string username = session->getValue("username");
        std::vector<std::pair<std::string, long long>> messages;
        {
            std::shared_ptr<AIHelper> AIHelperPtr;
            std::lock_guard<std::mutex> lock(server_->mutexForChatInformation);
            if (server_->chatInformation.find(userId) == server_->chatInformation.end()) {
                //在linux环境中通过环境变量获取相应的api-key初始化一个AIHelper
                const char* apiKey = std::getenv("DASHSCOPE_API_KEY");
                if (!apiKey) {
                    std::cerr << "Error: DASHSCOPE_API_KEY not found in environment!" << std::endl;
                    return;
                }
                // 新建一个AIHelper
                server_->chatInformation.emplace(
                    userId,
                    std::make_shared<AIHelper>(apiKey)
                );
            }
            AIHelperPtr = server_->chatInformation[userId];
            messages= AIHelperPtr->GetMessages();
        }
        //start
        json successResp;
        successResp["success"] = true;
        successResp["history"] = json::array();

        for (size_t i = 0; i < messages.size(); ++i) {
            json msgJson;
            msgJson["is_user"] = (i % 2 == 0);
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









