#include "../../include/handlers/AIUploadSendHandler.h"


void AIUploadSendHandler::handle(const http::HttpRequest& req, http::HttpResponse* resp)
{
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

            server_->packageResp(req.version(), http::HttpResponse::k401Unauthorized,
                "Unauthorized", true, "application/json; charset=utf-8", errorBody.size(),
                errorBody, resp);
            return;
        }
        //处理响应开始start
        // 1. 解析 JSON 参数
        int userId = std::stoi(session->getValue("userId"));
        std::shared_ptr<ImageRecognizer> ImageRecognizerPtr;
        {
            std::lock_guard<std::mutex> lock(server_->mutexForImageRecognizerMap);
            if (server_->ImageRecognizerMap.find(userId) == server_->ImageRecognizerMap.end()) {
                // 创建一个新的 ImageRecognizer
                server_->ImageRecognizerMap.emplace(
                    userId,
                    std::make_shared<ImageRecognizer>("/root/models/mobilenetv2/mobilenetv2-7.onnx")  //todo:需要将/path/to/model.onnx替换为实际路径
                );
            }
            ImageRecognizerPtr = server_->ImageRecognizerMap[userId];
        }

        auto body = req.getBody();
        std::string filename;
        std::string imageBase64;
        if (!body.empty()) {
            auto j = json::parse(body);
            if (j.contains("filename")) filename = j["filename"];
            if (j.contains("image")) imageBase64 = j["image"];
        }
        if (imageBase64.empty())
        {
            throw std::runtime_error("No image data provided");
        }

        std::string decodedData = base64_decode(imageBase64);
        std::vector<uchar> imgData(decodedData.begin(), decodedData.end());
        //开始识别
        std::string className = ImageRecognizerPtr->PredictFromBuffer(imgData);

        // 4. 构造响应
        json successResp;
        successResp["success"] = "ok";
        successResp["filename"] = filename;
        successResp["class_name"] = className;
        //模型的置信度
        successResp["confidence"] = 0.95; // todo:这里写死了，需要从模型返回获取实际值

        //end
        std::string successBody = successResp.dump(4);

        resp->setStatusLine(req.version(), http::HttpResponse::k200Ok, "OK");
        resp->setCloseConnection(false);
        resp->setContentType("application/json; charset=utf-8");
        resp->setContentLength(successBody.size());
        resp->setBody(successBody);
        return;

    }
    catch (const std::exception& e)
    {
        // 处理异常，返回错误信息
        json failureResp;
        failureResp["status"] = "error";
        failureResp["message"] = e.what();
        std::string failureBody = failureResp.dump(4);
        resp->setStatusLine(req.version(), http::HttpResponse::k400BadRequest, "Bad Request");
        resp->setCloseConnection(true);
        resp->setContentType("application/json; charset=utf-8");
        resp->setContentLength(failureBody.size());
        resp->setBody(failureBody);
    }
}



