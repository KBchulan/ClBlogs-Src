-- 增
-- 新增数据库

CREATE DATABASE [IF NOT EXISTS] 数据库名
[CHARACTER SET 字符集名] -- 建议 utf8mb4
[COLLATE 校对规则名];    -- 建议 utf8mb4_unicode_ci

-- 实例
CREATE DATABASE IF NOT EXISTS learn CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

-- 新增表

CREATE TABLE [IF NOT EXISTS] 表名 (
    字段名1  数据类型  [约束条件],
    ...
) [表选项];

其中数据类型有:

整数型: TINYINT, INT, BIGINT
浮点型: FLOAT, DOUBLE, DECIMAL(M,D)
字符型: CHAR, VARCHAR
备注型: TINYTEXT, TEXT, LONGTEXT
日期型: DATETIME, DATA

约束条件有:

空值: NOT NULL
默认值: DEFAULT
键: PRIMARY KEY, FOREIGN KEY, UNIQUE
额外: AUTO_INCREMENT, COMMENT

表选项:

ENGINE: InnoDB(默认且推荐), MyISAM
CHARSET: 为表单独指定字符集
COLLATE: 设置规则

实例:

CREATE TABLE IF NOT EXISTS students (
    id INT PRIMARY KEY AUTO_INCREMENT,
    student_no VARCHAR(20) NOT NULL UNIQUE COMMENT '学号',
    name VARCHAR(50) NOT NULL COMMENT '学生姓名',
    gender TINYINT DEFAULT 0 COMMENT '性别, 0-未知, 1-男, 2-女',
    email VARCHAR(100) UNIQUE COMMENT '电子邮箱',
    enrollment_date DATE COMMENT '入学日期',
    create_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '记录创建时间'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 新增数据项

-- 单行
INSERT INTO 表名 (字段1, 字段2, ...) VALUES (值1, 值2, ...); -- 指定字段
INSERT INTO 表名 VALUES (值1, 值2, ...); -- 按顺序的字段

-- 多行
INSERT INTO 表名 (字段1, 字段2, ...) VALUES
(值1a, 值2a, ...),
(值1b, 值2b, ...),
(值1c, 值2c, ...);