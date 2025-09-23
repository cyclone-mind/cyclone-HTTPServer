#pragma once
#include "../../../../HttpServer/include/RouterHandler.hpp"
#include "../ChatServer.h"


class ChatHistoryHandler : public http::router::RouterHandler
{
public:
    explicit ChatHistoryHandler(ChatServer* server) : server_(server) {}

    void handle(const http::HttpRequest& req, http::HttpResponse* resp) override;
private:

private:
    ChatServer* server_;
};