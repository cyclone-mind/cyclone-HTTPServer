
#pragma once

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
namespace http::router {
class RouterHandler {
public:
    virtual ~RouterHandler() = default;
    virtual void handle(const HttpRequest& request, HttpResponse* response) = 0;
};
}  // namespace http::router