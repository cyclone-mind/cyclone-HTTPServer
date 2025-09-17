-- 创建测试数据库连接池的示例表
-- 数据库名: Meme

-- 用户表
CREATE TABLE IF NOT EXISTS users (
    id INT PRIMARY KEY AUTO_INCREMENT,
    username VARCHAR(50) NOT NULL UNIQUE,
    email VARCHAR(100) NOT NULL UNIQUE,
    password_hash VARCHAR(255) NOT NULL,
    nickname VARCHAR(50),
    avatar_url VARCHAR(255),
    is_active BOOLEAN DEFAULT TRUE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
);

-- 文章表
CREATE TABLE IF NOT EXISTS posts (
    id INT PRIMARY KEY AUTO_INCREMENT,
    user_id INT NOT NULL,
    title VARCHAR(200) NOT NULL,
    content TEXT NOT NULL,
    category VARCHAR(50) DEFAULT 'general',
    view_count INT DEFAULT 0,
    like_count INT DEFAULT 0,
    is_published BOOLEAN DEFAULT FALSE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE
);

-- 评论表
CREATE TABLE IF NOT EXISTS comments (
    id INT PRIMARY KEY AUTO_INCREMENT,
    post_id INT NOT NULL,
    user_id INT NOT NULL,
    content TEXT NOT NULL,
    parent_id INT DEFAULT NULL,
    is_approved BOOLEAN DEFAULT TRUE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (post_id) REFERENCES posts(id) ON DELETE CASCADE,
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE,
    FOREIGN KEY (parent_id) REFERENCES comments(id) ON DELETE CASCADE
);

-- 插入测试数据
INSERT INTO users (username, email, password_hash, nickname) VALUES
('admin', 'admin@example.com', 'hashed_password_123', '管理员'),
('john_doe', 'john@example.com', 'hashed_password_456', 'John'),
('jane_smith', 'jane@example.com', 'hashed_password_789', 'Jane'),
('test_user', 'test@example.com', 'hashed_password_test', '测试用户');

INSERT INTO posts (user_id, title, content, category, is_published) VALUES
(1, '欢迎使用Cyclone HTTP Server', '这是一个基于muduo网络库构建的高性能HTTP服务器框架。', 'announcement', TRUE),
(2, 'C++现代化开发实践', '分享一些C++17的新特性和最佳实践...', 'tech', TRUE),
(3, '数据库连接池设计', '如何设计一个高效的数据库连接池...', 'tech', TRUE),
(4, '测试文章', '这是一篇测试文章，用于验证数据库功能。', 'test', FALSE);

INSERT INTO comments (post_id, user_id, content) VALUES
(1, 2, '很棒的框架，期待更多功能！'),
(1, 3, '性能看起来不错，有压测数据吗？'),
(2, 1, '感谢分享，很实用的内容。'),
(3, 2, '连接池的设计思路很清晰。');