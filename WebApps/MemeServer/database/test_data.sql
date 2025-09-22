-- 测试用户数据
-- 插入一些测试用户，模拟不同的分数水平

USE MemeTest;

-- 插入测试用户数据
INSERT INTO users (user_id, password, nickname, avatar, total_score, test_count) VALUES
('mememaster', '123456', '梗王大师', 'http://static.photos/people/200x200/1', 2500, 25),
('danklord', '123456', '黑暗领主', 'http://static.photos/people/200x200/2', 2200, 22),
('pepefrog', '123456', '蛙蛙酱', 'http://static.photos/people/200x200/3', 1950, 20),
('wojakwarrior', '123456', '战士沃jak', 'http://static.photos/people/200x200/4', 1800, 18),
('gigachad', '123456', '超级帅哥', 'http://static.photos/people/200x200/5', 1650, 16),
('baseddept', '123456', '靠谱部门', 'http://static.photos/people/200x200/6', 1500, 15),
('redpilled', '123456', '觉醒者', 'http://static.photos/people/200x200/7', 1350, 14),
('chadmaxxing', '123456', 'Chad修炼者', 'http://static.photos/people/200x200/8', 1200, 12),
('sigma', '123456', '西格玛男', 'http://static.photos/people/200x200/9', 1050, 11),
('mogger', '123456', '上进者', 'http://static.photos/people/200x200/10', 900, 9),
('looksmaxxer', '123456', '颜值修炼者', 'http://static.photos/people/200x200/11', 750, 8),
('gymcel', '123456', '健身侠', 'http://static.photos/people/200x200/12', 600, 6),
('neet', '123456', '家里蹲', 'http://static.photos/people/200x200/13', 450, 5),
('incel', '123456', '单身狗', 'http://static.photos/people/200x200/14', 300, 3),
('virgin', '123456', '纯洁小白', 'http://static.photos/people/200x200/15', 150, 2),
('newbie', '123456', '萌新报道', 'http://static.photos/people/200x200/16', 50, 1);

-- 插入一些测试记录，让排行榜更有数据
INSERT INTO test_records (user_id, score, total_questions, correct_answers, category, test_time) VALUES
(1, 95, 10, 9, '游戏', NOW() - INTERVAL 1 DAY),
(1, 88, 10, 8, '直播', NOW() - INTERVAL 2 DAY),
(2, 92, 10, 9, '游戏', NOW() - INTERVAL 1 DAY),
(2, 85, 10, 8, '抖音', NOW() - INTERVAL 3 DAY),
(3, 87, 10, 8, '影视', NOW() - INTERVAL 2 DAY),
(4, 83, 10, 8, '游戏', NOW() - INTERVAL 1 DAY),
(5, 89, 10, 8, '直播', NOW() - INTERVAL 4 DAY),
(6, 91, 10, 9, '抖音', NOW() - INTERVAL 2 DAY),
(7, 78, 10, 7, '影视', NOW() - INTERVAL 3 DAY),
(8, 82, 10, 8, '游戏', NOW() - INTERVAL 1 DAY);

-- 为前几个用户添加称号
INSERT INTO user_titles (user_id, title_id) VALUES
(1, 6), -- 梗王宗师
(2, 6), -- 梗王宗师
(3, 5), -- 梗王大师
(4, 5), -- 梗王大师
(5, 5), -- 梗王大师
(6, 5), -- 梗王大师
(7, 4), -- 梗王高手
(8, 4); -- 梗王高手