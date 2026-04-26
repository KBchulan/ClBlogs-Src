-- 改

-- 增加字段
ALTER TABLE 表名 ADD COLUMN 字段名 数据类型 [约束] [FIRST | AFTER 另一个字段名];

FIRST: 将新字段作为表的第一个字段。
AFTER 另一个字段名: 将新字段添加到指定的字段后面。
如果都不指定，新字段默认添加到表的最后。

-- 删除字段
ALTER TABLE 表名 DROP COLUMN 字段名;

-- 修改字段
ALTER TABLE 表名 MODIFY COLUMN 字段名 新数据类型 [新约束];

-- 修改表名
ALTER TABLE 旧表名 RENAME TO 新表名;

-- 修改表选项
ALTER TABLE 表名 ENGINE=新存储引擎, CHARSET=新字符集;


-- 修改某些行
UPDATE 表名
SET 字段1 = 值1, 字段2 = 值2, ...
[WHERE 条件];
