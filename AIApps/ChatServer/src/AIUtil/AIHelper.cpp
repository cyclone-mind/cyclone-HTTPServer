#include"../../include/AIUtil/AIHelper.h"
#include"../../include/AIUtil/MQManager.h"
#include <stdexcept>
#include <chrono>

// 构造函数
AIHelper::AIHelper(const std::string& apiKey)
    : apiKey_(apiKey) {
}

// 设置默认模型
void AIHelper::setModel(const std::string& modelName) {
    model_ = modelName;
}

// 添加一条用户消息
void AIHelper::addMessage(int userId,const std::string& userName, bool is_user,const std::string& userInput ) {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    messages.push_back({ userInput,ms });
    //消息队列异步入库
    pushMessageToMysql(userId, userName, is_user, userInput,ms);
}

void AIHelper::restoreMessage(const std::string& userInput,long long ms) {
    messages.push_back({ userInput,ms });
}


// 发送聊天消息
std::string AIHelper::chat(int userId,std::string userName) {
    // 构造 payload
    json payload;
    payload["model"] = model_;
    json msgArray = json::array();

    for (size_t i = 0; i < messages.size(); ++i) {
        json msg;
        if (i % 2 == 0) { // 偶数下标：用户
            msg["role"] = "user";
            msg["content"] = messages[i].first;
        }
        else { // 奇数下标：AI
            msg["role"] = "assistant";
            msg["content"] = messages[i].first;
        }
        msgArray.push_back(msg);
    }

    payload["messages"] = msgArray;

    // 打印 payload（缩进 4 个空格）
    std::cout << "[DEBUG] payload = " << payload.dump(4) << std::endl;

    // 执行请求
    json response = executeCurl(payload);

    if (response.contains("choices") && !response["choices"].empty()) {
        std::string answer = response["choices"][0]["message"]["content"];
        // 保存 AI 回复
        addMessage(userId,userName, false,answer);
        return answer;
    }

    return "[Error] 无法解析响应";
}

// 发送自定义请求体
json AIHelper::request(const json& payload) {
    return executeCurl(payload);
}

std::vector<std::pair<std::string, long long>> AIHelper::GetMessages() {
    return this->messages;
}


// 内部方法：执行 curl 请求
json AIHelper::executeCurl(const json& payload) {
    // 1. 初始化 CURL 句柄
    CURL* curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("Failed to initialize curl");
    }

    // 2. 准备响应数据缓冲区
    std::string readBuffer;
    // 3. 设置 HTTP 请求头
    struct curl_slist* headers = nullptr;
    std::string authHeader = "Authorization: Bearer " + apiKey_;

    // 4. 添加认证头和内容类型头
    headers = curl_slist_append(headers, authHeader.c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");

    // 5. 序列化 JSON 负载为字符串
    std::string payloadStr = payload.dump();
    // 6. 调试输出：打印序列化后的 JSON 字符串
    std::cout << "[DEBUG] JSON payload: " << payloadStr << std::endl;
    
    // 7. 配置 CURL 选项
    curl_easy_setopt(curl, CURLOPT_URL, apiUrl_.c_str());           // 设置请求 URL
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);           // 设置 HTTP 头
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payloadStr.c_str()); // 设置 POST 数据
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);   // 设置响应数据回调函数
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);         // 设置回调数据指针
    
    // 8. 添加额外的 CURL 选项以提高性能和稳定性
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);                   // 设置超时时间为 30 秒
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);             // 允许重定向
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "AIHelper/1.0");      // 设置 User-Agent

    // 9. 执行 HTTP 请求
    CURLcode res = curl_easy_perform(curl);
    
    // 10. 清理资源：释放 HTTP 头和 CURL 句柄
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    // 11. 检查 CURL 执行结果
    if (res != CURLE_OK) {
        throw std::runtime_error("curl_easy_perform() failed: " + std::string(curl_easy_strerror(res)));
    }

    // 12. 解析 JSON 响应
    try {
        return json::parse(readBuffer);
    }
    catch (const json::parse_error& e) {
        // 13. 捕获具体的 JSON 解析异常
        throw std::runtime_error("Failed to parse JSON response: " + std::string(e.what()) + "\nResponse: " + readBuffer);
    }
    catch (...) {
        // 14. 捕获其他未知异常
        throw std::runtime_error("Failed to parse JSON response: " + readBuffer);
    }
}

// curl 回调函数，把返回的数据写到 string buffer
size_t AIHelper::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    // 1. 计算接收到的数据总大小（字节数）
    size_t totalSize = size * nmemb;
    // 2. 将用户数据指针转换为字符串指针
    std::string* buffer = static_cast<std::string*>(userp);
    // 3. 将接收到的数据追加到字符串缓冲区
    buffer->append(static_cast<char*>(contents), totalSize);
    // 4. 返回实际处理的数据大小（必须等于 totalSize，否则 CURL 会认为出错）
    return totalSize;
}

// 字符串转义函数，用于防止 SQL 注入攻击
std::string AIHelper::escapeString(const std::string& input) {
    std::string output;
    // 1. 预分配足够的内存空间（最坏情况下每个字符都需要转义）
    output.reserve(input.size() * 2);
    
    // 2. 遍历输入字符串的每个字符
    for (char c : input) {
        // 3. 根据字符类型进行转义处理
        switch (c) {
            case '\\': output += "\\\\"; break;  // 反斜杠转义为两个反斜杠
            case '\'': output += "\\\'"; break;  // 单引号转义
            case '\"': output += "\\\""; break;  // 双引号转义
            case '\n': output += "\\n"; break;   // 换行符转义
            case '\r': output += "\\r"; break;   // 回车符转义
            case '\t': output += "\\t"; break;   // 制表符转义
            default:   output += c; break;       // 普通字符直接添加
        }
    }
    // 4. 返回转义后的安全字符串
    return output;
}


void AIHelper::pushMessageToMysql(int userId, const std::string& userName, bool is_user, const std::string& userInput,long long ms) {
    // std::string sql = "INSERT INTO chat_message (id, username, is_user, content, ts) VALUES ("
    //     + std::to_string(userId) + ", "  // 这里用 userId 作为 id，或者自己生成
    //     + "'" + userName + "', "
    //     + std::to_string(is_user ? 1 : 0) + ", "
    //     + "'" + userInput + "', "
    //     + std::to_string(ms) + ")";
    std::string safeUserName = escapeString(userName);
    std::string safeUserInput = escapeString(userInput);

    std::string sql = "INSERT INTO chat_message (id, username, is_user, content, ts) VALUES ("
        + std::to_string(userId) + ", "
        + "'" + safeUserName + "', "
        + std::to_string(is_user ? 1 : 0) + ", "
        + "'" + safeUserInput + "', "
        + std::to_string(ms) + ")";
    //改成消息队列异步执行mysql操作，用于流量削峰与解耦逻辑
    //mysqlUtil_.executeUpdate(sql);

    MQManager::instance().publish("sql_queue", sql);
}
