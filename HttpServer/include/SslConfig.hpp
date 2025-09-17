#pragma once
#include "SslTypes.hpp"
#include <string>
#include <vector>

namespace ssl 
{

class SslConfig 
{
public:
    SslConfig();
    ~SslConfig() = default;

    // 证书配置
    void setCertificateFile(const std::string& certFile) { certFile_ = certFile; }
    void setPrivateKeyFile(const std::string& keyFile) { keyFile_ = keyFile; }
    void setCertificateChainFile(const std::string& chainFile) { chainFile_ = chainFile; }
    
    // 协议版本和加密套件配置
    void setProtocolVersion(SSLVersion version) { version_ = version; }
    void setCipherList(const std::string& cipherList) { cipherList_ = cipherList; }
    
    // 客户端验证配置
    void setVerifyClient(bool verify) { verifyClient_ = verify; }
    void setVerifyDepth(int depth) { verifyDepth_ = depth; }
    
    // 会话配置
    void setSessionTimeout(int seconds) { sessionTimeout_ = seconds; }
    void setSessionCacheSize(long size) { sessionCacheSize_ = size; }

    // Getters
    [[nodiscard]] auto getCertificateFile() const -> const std::string& { return certFile_; }
    [[nodiscard]] auto getPrivateKeyFile() const -> const std::string& { return keyFile_; }
    [[nodiscard]] auto getCertificateChainFile() const -> const std::string& { return chainFile_; }
    [[nodiscard]] auto getProtocolVersion() const -> SSLVersion { return version_; }
    [[nodiscard]] auto getCipherList() const -> const std::string& { return cipherList_; }
    [[nodiscard]] auto getVerifyClient() const -> bool { return verifyClient_; }
    [[nodiscard]] auto getVerifyDepth() const -> int { return verifyDepth_; }
    [[nodiscard]] auto getSessionTimeout() const -> int { return sessionTimeout_; }
    [[nodiscard]] auto getSessionCacheSize() const -> long { return sessionCacheSize_; }

private:
    std::string certFile_; // 证书文件
    std::string keyFile_; // 私钥文件
    std::string chainFile_; // 证书链文件
    SSLVersion  version_; // 协议版本
    std::string cipherList_; // 加密套件
    bool        verifyClient_; // 是否验证客户端
    int         verifyDepth_; // 验证深度
    int         sessionTimeout_; // 会话超时时间
    long        sessionCacheSize_; // 会话缓存大小
};

} // namespace ssl