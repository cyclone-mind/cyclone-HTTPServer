#include "../../include/AIUtil/ImageRecognizer.h"

ImageRecognizer::ImageRecognizer(const std::string& model_path,
    const std::string& label_path)
    : env(ORT_LOGGING_LEVEL_WARNING, "ImageRecognizer")
{
    Ort::SessionOptions session_options;
    session_options.SetIntraOpNumThreads(1);
    session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);

    session = std::make_unique<Ort::Session>(env, model_path.c_str(), session_options);
    allocator = std::make_unique<Ort::AllocatorWithDefaultOptions>();

    // 获取输入输出名称
    input_name = session->GetInputNameAllocated(0, *allocator).get();
    output_name = session->GetOutputNameAllocated(0, *allocator).get();

    // 获取输入形状 [1,3,H,W]
    input_shape = session->GetInputTypeInfo(0).GetTensorTypeAndShapeInfo().GetShape();
    input_height = static_cast<int>(input_shape[2]);
    input_width = static_cast<int>(input_shape[3]);

    // 加载标签文件
    LoadLabels(label_path);
}

void ImageRecognizer::LoadLabels(const std::string& label_path) {
    std::ifstream infile(label_path);
    if (!infile.is_open()) {
        throw std::runtime_error("Failed to open label file: " + label_path);
    }

    std::string line;
    while (std::getline(infile, line)) {
        if (!line.empty()) {
            labels.push_back(line);
        }
    }
    infile.close();

    if (labels.empty()) {
        throw std::runtime_error("No labels loaded from file: " + label_path);
    }
}

std::string ImageRecognizer::PredictFromFile(const std::string& image_path) {
    // 从文件路径加载图像
    cv::Mat img = cv::imread(image_path);
    if (img.empty()) {
        throw std::runtime_error("Failed to load image: " + image_path);
    }
    // 调用通用预测方法
    return PredictFromMat(img);
}

std::string ImageRecognizer::PredictFromBuffer(const std::vector<unsigned char>& image_data) {
    // 从内存缓冲区解码图像
    cv::Mat img = cv::imdecode(image_data, cv::IMREAD_COLOR);
    if (img.empty()) {
        throw std::runtime_error("Failed to decode image from buffer");
    }
    // 调用通用预测方法
    return PredictFromMat(img);
}

std::string ImageRecognizer::PredictFromMat(const cv::Mat& img_raw) {
    // 检查输入图像是否为空
    if (img_raw.empty()) {
        throw std::runtime_error("Input image is empty");
    }

    // 图像预处理：调整大小和归一化
    cv::Mat img;
    cv::resize(img_raw, img, cv::Size(input_width, input_height));  // 调整到模型输入尺寸
    img.convertTo(img, CV_32F, 1.0 / 255.0);                        // 归一化到[0,1]范围

    // 图像格式转换：NHWC -> NCHW
    cv::dnn::blobFromImage(img, img);

    // 准备输入张量维度
    std::vector<int64_t> dims = { 1, 3, input_height, input_width };
    size_t input_tensor_size = 1 * 3 * input_height * input_width;

    // 创建CPU内存信息
    Ort::MemoryInfo memory_info = Ort::MemoryInfo::CreateCpu(
        OrtAllocatorType::OrtArenaAllocator, OrtMemType::OrtMemTypeDefault);

    // 创建输入张量
    Ort::Value input_tensor = Ort::Value::CreateTensor<float>(
        memory_info, img.ptr<float>(), input_tensor_size, dims.data(), dims.size());

    // 设置输入输出名称
    const char* input_names[] = { input_name.c_str() };
    const char* output_names[] = { output_name.c_str() };

    // 执行模型推理
    auto output_tensors = session->Run(
        Ort::RunOptions{ nullptr },
        input_names, &input_tensor, 1,
        output_names, 1
    );

    // 获取输出数据
    float* output_data = output_tensors.front().GetTensorMutableData<float>();

    // 使用 argmax 找到预测类别
    int num_classes = labels.empty() ? 1000 : (int)labels.size();
    int pred_class = std::max_element(output_data, output_data + num_classes) - output_data;

    // 返回预测结果
    if (pred_class >= 0 && pred_class < (int)labels.size()) {
        return labels[pred_class];
    }
    else {
        return "Unknown";
    }
}
