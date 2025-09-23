#pragma once
#include "../../../../HttpServer/include/RouterHandler.hpp"
#include "../../../../HttpServer/include/MysqlUtil.hpp"
#include "../ChatServer.h"

class ChatHandler : public http::router::RouterHandler
{
public:
    explicit ChatHandler(ChatServer* server) : server_(server) {}

    void handle(const http::HttpRequest& req, http::HttpResponse* resp) override;
private:

private:
    ChatServer* server_;
    http::MysqlUtil     mysqlUtil_;
};
