/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-15 13:56:34
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-16 10:47:26
 * @FilePath: /cyclone-HTTPServer/HttpServer/include/HttpTypes.hpp
 * @Description: 
 * Copyright (c) 2025 by ${git_name} email: ${git_email}, All Rights Reserved.
 */
#pragma once
#include <string>

namespace http {

enum class HttpVersion { UNKNOWN = 0, HTTP_1_0 = 10, HTTP_1_1 = 11, HTTP_2_0 = 20, HTTP_3_0 = 30 };

enum class HttpMethod { CInvalid, CGet, CPost, CPut, CDelete, COptions };

enum class HttpRequestParseState {
    CExpectRequestLine,  // 解析请求行
    CExpectHeaders,      // 解析请求头
    CExpectBody,         // 解析请求体
    CGotAll,             // 解析完成
};

enum class HttpStatusCode {
    CUnknown,
    C200Ok = 200,
    C204NoContent = 204,
    C301MovedPermanently = 301,
    C400BadRequest = 400,
    C401Unauthorized = 401,
    C403Forbidden = 403,
    C404NotFound = 404,
    C409Conflict = 409,
    C500InternalServerError = 500,
};

// 版本转换工具函数
inline auto versionToString(HttpVersion version) -> std::string {
    switch (version) {
        case HttpVersion::HTTP_1_0:
            return "HTTP/1.0";
        case HttpVersion::HTTP_1_1:
            return "HTTP/1.1";
        case HttpVersion::HTTP_2_0:
            return "HTTP/2.0";
        case HttpVersion::HTTP_3_0:
            return "HTTP/3.0";
        default:
            return "Unknown";
    }
}

inline auto methodToString(HttpMethod method)-> std::string { 
    switch (method) {
        case HttpMethod::CDelete:
            return "DELETE";
        case HttpMethod::CGet:
            return "GET";
        case HttpMethod::CInvalid:
            return "INVALID";
        case HttpMethod::COptions:
            return "OPTIONS";
        case HttpMethod::CPost:
            return "POST";
        case HttpMethod::CPut:
            return "PUT";
        default:
            return "Unknown";
    }
}
inline auto stringToVersion(const std::string& versionStr) -> HttpVersion {
    if (versionStr == "HTTP/1.0") {
        return HttpVersion::HTTP_1_0;
    }
    if (versionStr == "HTTP/1.1") {
        return HttpVersion::HTTP_1_1;
    }
    if (versionStr == "HTTP/2.0") {
        return HttpVersion::HTTP_2_0;
    }
    if (versionStr == "HTTP/3.0") {
        return HttpVersion::HTTP_3_0;
    }
    return HttpVersion::UNKNOWN;
}
}  // namespace http