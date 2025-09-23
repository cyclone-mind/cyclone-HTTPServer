#include "../../include/handlers/AIMenuHandler.h"

void AIMenuHandler::handle(const http::HttpRequest& req, http::HttpResponse* resp)
{
    // JSON 处理使用 try catch 处理异常
    try
    {
        // 检查用户是否已登录
        auto session = server_->getSessionManager()->getSession(req, resp);
        LOG_INFO << "session->getValue(\"isLoggedIn\") = " << session->getValue("isLoggedIn");
        if (session->getValue("isLoggedIn") != "true")
        {
            // 用户未登录，返回未授权错误
            json errorResp;
            errorResp["status"] = "error";
            errorResp["message"] = "Unauthorized";
            std::string errorBody = errorResp.dump(4);

            server_->packageResp(req.version(), http::HttpStatusCode::C401Unauthorized,
                "Unauthorized", true, "application/json; charset=utf-8", errorBody.size(),
                errorBody, resp);
            return;
        }

        // 获取用户信息
        int userId = std::stoi(session->getValue("userId"));
        std::string username = session->getValue("username");

        std::string reqFile("../AIApps/ChatServer/resource/menu.html");
        FileUtil fileOperater(reqFile);
        if (!fileOperater.isValid())
        {
            LOG_WARN << reqFile << "not exist.";
            fileOperater.resetDefaultFile();
        }

        std::vector<char> buffer(fileOperater.size());
        fileOperater.readFile(buffer); // 读取文件内容
        std::string htmlContent(buffer.data(), buffer.size());

        // 在HTML头部插入userId
        size_t headEnd = htmlContent.find("</head>");
        if (headEnd != std::string::npos)
        {
            std::string script = "<script>const userId = '" + std::to_string(userId) + "';</script>";
            htmlContent.insert(headEnd, script);
        }

        // server_->packageResp(req.version(), HttpResponse::C200Ok, "OK"
        //             , false, "text/html", htmlContent.size(), htmlContent, resp);
        resp->setStatusLine(req.version(), http::HttpStatusCode::C200Ok, "OK");
        resp->setCloseConnection(false);
        resp->setContentType("text/html; charset=utf-8");
        resp->setContentLength(htmlContent.size());
        resp->setBody(htmlContent);
    }
    catch (const std::exception& e)
    {
        // 处理异常，返回错误信息
        json failureResp;
        failureResp["status"] = "error";
        failureResp["message"] = e.what();
        std::string failureBody = failureResp.dump(4);
        resp->setStatusLine(req.version(), http::HttpStatusCode::C400BadRequest, "Bad Request");
        resp->setCloseConnection(true);
        resp->setContentType("application/json; charset=utf-8");
        resp->setContentLength(failureBody.size());
        resp->setBody(failureBody);
    }
}