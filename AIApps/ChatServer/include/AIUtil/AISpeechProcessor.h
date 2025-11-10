#pragma once
#include <iostream>
#include <string>
#include <curl/curl.h>
#include <fstream>
#include <memory>
#include <sstream>
#include <thread>
#include <chrono>


#include "../../../../HttpServer/include/utils/JsonUtil.h"
#include "base64.h"

/**
 * AISpeechProcessor 类
 * AI 语音处理器
 *
 * 功能：
 * - 语音识别（Speech to Text, STT）：将语音转换为文字
 * - 语音合成（Text to Speech, TTS）：将文字转换为语音
 *
 * 服务提供商：百度语音API
 * - 使用百度语音开放平台的 API
 * - 需要提供 client_id 和 client_secret
 * - 支持多种音频格式和参数配置
 */
class AISpeechProcessor {
public:
    /**
     * 构造函数
     * @param clientId 百度应用的 API Key
     * @param clientSecret 百度应用的 Secret Key
     * @param cuid 用户唯一标识（默认值需要更改为实际应用的标识）
     */
    AISpeechProcessor(const std::string& clientId,
                      const std::string& clientSecret,
                      const std::string& cuid = "RZjSQGzNaA8EFWf6rvuHEKDh9i4XJIV9") //用户唯一标识，需要更改成自身标识
        : client_id_(clientId), client_secret_(clientSecret), cuid_(cuid)
    {
        token_ = getAccessToken();
    }

    /**
     * 语音识别（Speech to Text）
     * @param speechData 语音数据（Base64编码或原始音频数据）
     * @param format 音频格式（默认 pcm，支持 pcm/wav/amr/m4a）
     * @param rate 采样率（默认 16000，支持 8000/16000）
     * @param channel 声道数（默认 1）
     * @return 识别出的文字内容
     */
    std::string recognize(const std::string& speechData,const std::string& format = "pcm",int rate = 16000,int channel = 1);

    /**
     * 语音合成（Text to Speech）
     * @param text 要合成的文字内容
     * @param format 音频格式（默认 mp3-16k，支持 pcm-16k/wav/mp3-16k/mp3-48k）
     * @param lang 语言（默认 zh，支持 zh/en）
     * @param speed 语速（默认 5，范围 0-15）
     * @param pitch 音调（默认 5，范围 0-15）
     * @param volume 音量（默认 5，范围 0-15）
     * @return 合成的音频数据（Base64编码）
     */
    std::string synthesize(const std::string& text,const std::string& format = "mp3-16k",const std::string& lang = "zh",int speed = 5,int pitch = 5,int volume = 5);


private:
    std::string client_id_;      // 百度 API Key
    std::string client_secret_;  // 百度 Secret Key
    std::string cuid_;           // 用户唯一标识
    std::string token_;          // Access Token（通过 OAuth 获取）

    /**
     * 获取百度语音 API 的 Access Token
     * @return Access Token 字符串
     */
    std::string getAccessToken();

};
