-- 删
-- 删除库

DROP DATABASE [IF EXISTS] 数据库名;

-- 删除表

DROP TABLE [IF EXISTS] 表名1, 表名2, ...;

-- 删除某几行

DELETE FROM 表名 [WHERE 条件]; -- 如果不写 WHRER 会删除所有数据，一行一行删除
TRUNCATE TABLE 表名;  -- 直接删除表并重建
