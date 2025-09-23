#pragma once
#include "../../../../HttpServer/include/RouterHandler.hpp"
#include "../ChatServer.h"

class ChatEntryHandler : public http::router::RouterHandler
{
public:
    explicit ChatEntryHandler(ChatServer* server) : server_(server) {}

    void handle(const http::HttpRequest& req, http::HttpResponse* resp) override;

private:
    ChatServer* server_;
    /*
        http::MysqlUtil mysqlUtil_;
        bool init=false;
    */

};