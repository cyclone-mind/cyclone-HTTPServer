#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <onnxruntime_cxx_api.h>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <iostream>

class ImageRecognizer {
public:
    // 构造函数，传入模型路径和标签路径
    explicit ImageRecognizer(const std::string& model_path,
        const std::string& label_path = "/root/imagenet_classes.txt");

    // 从文件预测
    std::string PredictFromFile(const std::string& image_path);

    // 从内存缓冲区预测（前端上传图片数据）
    std::string PredictFromBuffer(const std::vector<unsigned char>& image_data);

    // 从 OpenCV Mat 预测
    std::string PredictFromMat(const cv::Mat& img);

private:
    Ort::Env env;                                           // ONNX Runtime 环境对象
    std::unique_ptr<Ort::Session> session;                  // ONNX 模型会话
    std::unique_ptr<Ort::AllocatorWithDefaultOptions> allocator;  // 内存分配器

    std::string input_name;                                 // 模型输入节点名称
    std::string output_name;                                // 模型输出节点名称
    std::vector<int64_t> input_shape;                       // 模型输入形状 [batch, channels, height, width]
    int input_height{}, input_width{};                      // 模型输入图像的高度和宽度

    std::vector<std::string> labels;                        // 存储分类标签

    void LoadLabels(const std::string& label_path);         // 加载标签文件
};