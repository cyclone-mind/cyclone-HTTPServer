-- 热梗测试应用数据库结构
-- 数据库名: MemeTest

-- 创建数据库
CREATE DATABASE IF NOT EXISTS MemeTest CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
USE MemeTest;

-- 用户表
CREATE TABLE users (
    id INT PRIMARY KEY AUTO_INCREMENT,
    user_id VARCHAR(50) UNIQUE NOT NULL COMMENT '用户ID（唯一标识）',
    password VARCHAR(255) NOT NULL COMMENT '密码（明文存储）',
    nickname VARCHAR(100) DEFAULT '' COMMENT '昵称',
    avatar VARCHAR(255) DEFAULT '' COMMENT '头像URL',
    total_score INT DEFAULT 0 COMMENT '总得分',
    test_count INT DEFAULT 0 COMMENT '测试次数',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    INDEX idx_user_id (user_id),
    INDEX idx_total_score (total_score DESC)
);

-- 梗表
CREATE TABLE memes (
    id INT PRIMARY KEY AUTO_INCREMENT,
    title VARCHAR(200) NOT NULL COMMENT '梗标题',
    description TEXT COMMENT '梗描述',
    category ENUM('游戏', '直播', '抖音', '生活', '影视', '其他') NOT NULL COMMENT '分类',
    source VARCHAR(500) COMMENT '来源',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_category (category)
);

-- 题目表
CREATE TABLE questions (
    id INT PRIMARY KEY AUTO_INCREMENT,
    meme_id INT NOT NULL COMMENT '关联的梗ID',
    question_text TEXT NOT NULL COMMENT '题目内容',
    option_a VARCHAR(500) NOT NULL COMMENT '选项A',
    option_b VARCHAR(500) NOT NULL COMMENT '选项B',
    option_c VARCHAR(500) NOT NULL COMMENT '选项C',
    option_d VARCHAR(500) NOT NULL COMMENT '选项D',
    correct_answer CHAR(1) NOT NULL COMMENT '正确答案(A/B/C/D)',
    explanation TEXT COMMENT '答案解释',
    score INT DEFAULT 10 COMMENT '题目分数',
    difficulty ENUM('简单', '中等', '困难') DEFAULT '中等',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (meme_id) REFERENCES memes(id) ON DELETE CASCADE,
    INDEX idx_meme_id (meme_id),
    INDEX idx_difficulty (difficulty)
);

-- 测试记录表
CREATE TABLE test_records (
    id INT PRIMARY KEY AUTO_INCREMENT,
    user_id INT NOT NULL COMMENT '用户ID',
    score INT NOT NULL COMMENT '本次得分',
    total_questions INT NOT NULL COMMENT '总题目数',
    correct_answers INT NOT NULL COMMENT '正确答案数',
    category VARCHAR(50) COMMENT '测试分类（可选）',
    test_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE,
    INDEX idx_user_id (user_id),
    INDEX idx_test_time (test_time DESC),
    INDEX idx_score (score DESC)
);

-- 称号表
CREATE TABLE titles (
    id INT PRIMARY KEY AUTO_INCREMENT,
    title_name VARCHAR(100) NOT NULL COMMENT '称号名称',
    min_score INT NOT NULL COMMENT '最低分数',
    max_score INT DEFAULT NULL COMMENT '最高分数(NULL表示无上限)',
    description VARCHAR(500) COMMENT '称号描述',
    icon VARCHAR(255) COMMENT '称号图标URL',
    UNIQUE KEY unique_title_range (min_score, max_score),
    INDEX idx_min_score (min_score)
);

-- 用户称号表
CREATE TABLE user_titles (
    id INT PRIMARY KEY AUTO_INCREMENT,
    user_id INT NOT NULL,
    title_id INT NOT NULL,
    achieved_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE,
    FOREIGN KEY (title_id) REFERENCES titles(id) ON DELETE CASCADE,
    UNIQUE KEY unique_user_title (user_id, title_id),
    INDEX idx_user_id (user_id)
);

-- 插入一些示例数据
-- 称号数据
INSERT INTO titles (title_name, min_score, max_score, description) VALUES
('梗王新手', 0, 99, '刚刚入门热梗世界'),
('梗王学徒', 100, 299, '对热梗有了一定了解'),
('梗王达人', 300, 599, '热梗知识丰富'),
('梗王高手', 600, 999, '热梗运用自如'),
('梗王大师', 1000, 1499, '热梗领域的专家'),
('梗王宗师', 1500, NULL, '热梗界的传奇人物');

-- 示例梗数据
INSERT INTO memes (title, description, category, source) VALUES
('打野', '在游戏中担任打野位置的玩家', '游戏', '英雄联盟'),
('上头', '形容某事物让人着迷或兴奋', '生活', '网络流行语'),
('emo', 'emotional的缩写，表示情绪化', '生活', '网络用语'),
('内卷', '内部竞争激烈', '生活', '社会现象'),
('整活', '搞事情、制造热闹', '直播', '直播文化');

-- 示例题目数据
INSERT INTO questions (meme_id, question_text, option_a, option_b, option_c, option_d, correct_answer, explanation, score, difficulty) VALUES
(1, '在英雄联盟中，"打野"通常指什么位置？', '上单', '中单', '打野', 'ADC', 'C', '打野位主要负责清理野怪、抓人、控制视野', 10, '简单'),
(2, '"上头"这个词通常形容什么感觉？', '头痛', '兴奋', '困倦', '生气', 'B', '"上头"形容某事物让人着迷或兴奋的状态', 10, '简单'),
(3, '"emo"通常指什么？', '电子音乐', '情绪化', '电子竞技', '表情包', 'B', 'emo是emotional的缩写，表示情绪化', 10, '简单'),
(4, '"内卷"现象通常出现在哪里？', '体育比赛', '职场竞争', '游戏竞技', '学习考试', 'B', '内卷指内部竞争过于激烈，常见于职场和教育', 15, '中等'),
(5, '"整活"通常指什么行为？', '整理东西', '搞事情', '生活习惯', '工作安排', 'B', '"整活"指制造热闹、搞事情的行为', 10, '简单');