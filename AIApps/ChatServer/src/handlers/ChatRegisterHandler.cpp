#include "../../include/handlers/ChatRegisterHandler.h"


/**
 * ChatRegisterHandler::handle
 * 处理用户注册请求
 *
 * 功能：
 * 1. 解析注册信息（用户名、密码）
 * 2. 检查用户名是否已存在
 * 3. 插入新用户到数据库
 * 4. 返回注册结果
 */
void ChatRegisterHandler::handle(const http::HttpRequest& req, http::HttpResponse* resp)
{
    // 解析 JSON 格式的请求体
    json parsed = json::parse(req.getBody());
    std::string username = parsed["username"];
    std::string password = parsed["password"];

    // 尝试插入用户，返回用户ID（-1表示失败）
    int userId = insertUser(username, password);
    if (userId != -1)
    {
        // 注册成功，返回成功响应
        json successResp;
        successResp["status"] = "success";
        successResp["message"] = "Register successful";
        successResp["userId"] = userId;
        std::string successBody = successResp.dump(4);

        resp->setStatusLine(req.version(), http::HttpStatusCode::C200Ok, "OK");
        resp->setCloseConnection(false);
        resp->setContentType("application/json");
        resp->setContentLength(successBody.size());
        resp->setBody(successBody);
    }
    else
    {
        // 注册失败（用户名已存在），返回409 Conflict
        json failureResp;
        failureResp["status"] = "error";
        failureResp["message"] = "username already exists";
        std::string failureBody = failureResp.dump(4);

        resp->setStatusLine(req.version(), http::HttpStatusCode::C409Conflict, "Conflict");
        resp->setCloseConnection(false);
        resp->setContentType("application/json");
        resp->setContentLength(failureBody.size());
        resp->setBody(failureBody);
    }
}

/**
 * insertUser
 * 插入新用户到数据库
 *
 * @param username 用户名
 * @param password 密码
 * @return 成功返回用户ID，失败返回-1
 *
 * ⚠️ 安全警告：此函数使用字符串拼接构造SQL语句，存在SQL注入风险！
 * 建议改用预编译语句（Prepared Statements）来防止SQL注入攻击
 */
int ChatRegisterHandler::insertUser(const std::string& username, const std::string& password)
{

    if (!isUserExist(username))
    {
        // ⚠️ SQL注入风险：直接拼接用户输入到SQL语句
        // 应该使用参数化查询或预编译语句
        std::string sql = "INSERT INTO users (username, password) VALUES ('" + username + "', '" + password + "')";
        mysqlUtil_.executeUpdate(sql);
        std::string sql2 = "SELECT id FROM users WHERE username = '" + username + "'";
        auto res = mysqlUtil_.executeQuery(sql2);
        if (res->next())
        {
            return res->getInt("id");
        }
    }
    return -1;
}

/**
 * isUserExist
 * 检查用户名是否已经存在
 *
 * @param username 要检查的用户名
 * @return 存在返回true，不存在返回false
 *
 * ⚠️ 安全警告：此函数也存在SQL注入风险
 */
bool ChatRegisterHandler::isUserExist(const std::string& username)
{
    // ⚠️ SQL注入风险：直接拼接用户输入到SQL语句
    std::string sql = "SELECT id FROM users WHERE username = '" + username + "'";
    auto res = mysqlUtil_.executeQuery(sql);
    if (res->next())
    {
        return true;
    }
    return false;
}