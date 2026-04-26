-- 索引


主键索引，唯一索引，普通索引(只能加快速度)，复合索引(最左前缀)

-- 创表时创建

CREATE TABLE table_name (
    id INT PRIMARY KEY,
    user_email VARCHAR(100) NOT NULL,
    user_name VARCHAR(50),
    age INT,

    -- 创建唯一索引
    UNIQUE INDEX idx_unique_email (user_email),

    -- 创建普通索引
    INDEX idx_name (user_name),

    -- 创建复合索引，最左前缀
    INDEX idx_name_age (user_name, age)
);


-- 之后增加
CREATE INDEX 索引名 ON 表名 (字段名);

-- 查看索引
SHOW INDEX FROM students;

-- 删除索引
DROP INDEX idx_name ON students;


-- 使用 EXPLAIN 可以判断索引是否生效