#include "../include/HttpResponse.hpp"

#include <muduo/base/Logging.h>
#include <sys/stat.h>

#include <fstream>
#include <vector>

namespace http {
auto HttpResponse::appendToBuffer(muduo::net::Buffer* outputBuf) const -> void {
    std::array<char, STATUS_LINE_BUFFER_SIZE> buf;
    int len = snprintf(buf.data(), buf.size(), "%s %d ", versionToString(httpVersion_).c_str(),
             static_cast<int>(statusCode_));
    outputBuf->append(buf.data(), len);
    outputBuf->append(statusMessage_);
    outputBuf->append("\r\n");

    if (closeConnection_) {
        outputBuf->append("Connection: close\r\n");
    } else {
        outputBuf->append("Connection: Keep-Alive\r\n");
    }

    for (const auto& header : headers_) {  // 为什么这里不用格式化字符串？因为key和value的长度不定
        outputBuf->append(header.first);
        outputBuf->append(": ");
        outputBuf->append(header.second);
        outputBuf->append("\r\n");
    }
    outputBuf->append("\r\n");
    outputBuf->append(body_);
}

}  // namespace http