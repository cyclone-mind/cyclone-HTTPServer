#include "../../include/handlers/ChatEntryHandler.h"



void ChatEntryHandler::handle(const http::HttpRequest& req, http::HttpResponse* resp)
{
    // 因为是get请求，参数在url中也行，但我们就直接返回响应

    //首先通过字符串拼接一个路径
    //然后将响应路径下的文件通过FileUtil打开，
    //读取外部文件内容赋值给string buffer，然后构造响应
    std::string reqFile;
    reqFile.append("../AIApps/ChatServer/resource/entry.html");
    FileUtil fileOperater(reqFile);
    if (!fileOperater.isValid())
    {
        LOG_WARN << reqFile << " not exist";
        fileOperater.resetDefaultFile(); // 404 NOT FOUND
    }

    std::vector<char> buffer(fileOperater.size());
    fileOperater.readFile(buffer); // 读取文件内容
    std::string bufStr = std::string(buffer.data(), buffer.size());

    resp->setStatusLine(req.version(), http::HttpStatusCode::C200Ok, "OK");
    resp->setCloseConnection(false);
    resp->setContentType("text/html");
    resp->setContentLength(bufStr.size());
    resp->setBody(bufStr);
}
