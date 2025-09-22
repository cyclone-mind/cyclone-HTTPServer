#include "../include/HttpRequest.hpp"
#include <algorithm> 
#include <cctype>

namespace http {

auto HttpRequest::parseRequestLine(const char* begin, const char* end) -> bool {
    // 解析请求行格式: METHOD PATH VERSION
    const char* start = begin;
    const char* space = std::find(start, end, ' ');
    
    if (space == end) {
        return false; // 找不到第一个空格
    }
    
    // 1. 解析 HTTP 方法
    if (!setMethod(start, space)) {
        return false;
    }
    
    // 2. 解析路径
    start = space + 1;
    space = std::find(start, end, ' ');
    if (space == end) {
        return false; // 找不到第二个空格
    }
    
    // 解析路径和查询参数
    const char* questionMark = std::find(start, space, '?');
    if (questionMark != space) {
        // 有查询参数
        setPath(std::string(start, questionMark));
        parseQueryParams(questionMark + 1, space);
    } else {
        // 没有查询参数
        setPath(std::string(start, space));
    }
    
    // 3. 解析 HTTP 版本
    start = space + 1;
    std::string versionStr(start, end);
    setVersion(stringToVersion(versionStr));
    
    return version() != HttpVersion::UNKNOWN;
}

auto HttpRequest::parseQueryParams(const char* begin, const char* end) -> void {
    const char* start = begin;

    while (start < end) {
        const char* ampersand = std::find(start, end, '&');
        if (ampersand == end) {
            // 最后一个参数，没有&
            const char* equal = std::find(start, end, '=');
            if (equal != end) {
                std::string key(start, equal);
                std::string value(equal + 1, end);
                setQueryParams(std::move(key), std::move(value));
            }
            break;
        } else {
            // 有&，解析当前参数
            const char* equal = std::find(start, ampersand, '=');
            if (equal != ampersand) {
                std::string key(start, equal);
                std::string value(equal + 1, ampersand);
                setQueryParams(std::move(key), std::move(value));
            }
            start = ampersand + 1;
        }
    }
}

auto HttpRequest::addHeader(const char* start, const char* colon, const char* end) -> void {
    std::string key(start, colon);
    ++colon;
    // 跳过空格
    while (colon < end && (isspace(*colon) != 0)) {
        ++colon;
    }
    std::string value(colon, end);
    while (!value.empty() && (isspace(value[value.size() - 1]) != 0))  // 消除尾部空格
    {
        value.resize(value.size() - 1);
    }
    headers_[key] = value;
}


}  // namespace http
