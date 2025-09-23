#pragma once

#include "../../../../HttpServer/include/RouterHandler.hpp"
#include "../../../../HttpServer/include/MysqlUtil.hpp"
#include"../ChatServer.h"
#include "../../../../HttpServer/include/JsonUtil.h"

class ChatLoginHandler : public http::router::RouterHandler
{
public:
    explicit ChatLoginHandler(ChatServer* server) : server_(server) {}

    void handle(const http::HttpRequest& req, http::HttpResponse* resp) override;

private:
    int queryUserId(const std::string& username, const std::string& password);

private:
    ChatServer* server_;
    http::MysqlUtil     mysqlUtil_;
};