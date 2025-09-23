#pragma once
#include "../../../../HttpServer/include/RouterHandler.hpp"
#include "../ChatServer.h"

class AIUploadHandler : public http::router::RouterHandler
{
public:
    explicit AIUploadHandler(ChatServer* server) : server_(server) {}

    void handle(const http::HttpRequest& req, http::HttpResponse* resp) override;

private:
    ChatServer* server_;
};