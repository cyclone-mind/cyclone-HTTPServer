#pragma once
#include "../../../../HttpServer/include/RouterHandler.hpp"
#include "../ChatServer.h"


class AIMenuHandler : public http::router::RouterHandler
{
public:
    explicit AIMenuHandler(ChatServer* server) : server_(server) {}

    void handle(const http::HttpRequest& req, http::HttpResponse* resp) override;
private:
    ChatServer* server_;
};