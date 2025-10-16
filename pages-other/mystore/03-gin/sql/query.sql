-- 查

-- 简单查询
SELECT [字段列表] -- 可以指定，也可以为 *，可以设置 AS 别名
FROM [表名]
WHERE [筛选条件]  -- 运算符, 范围(BETWEEN .. AND..)，IN，LIKE(%多个字符, _一个字符), IS [NOT] NULL, AND OR NOT
GROUP BY [分组字段]  -- 按照指定字段进行分组，并对每个组进行聚合计算 COUNT, SUM, AVG, MAX, MIN
HAVING [分组后筛选条件] -- 对分组后的结果进行筛选，WHERE 是对分组前的进行筛选
ORDER BY [排序字段]  -- ASC 升序，DESC 降序，可以多个字段排序，优先前面的
LIMIT [结果数量限制]; -- 限制数量，用于实现分页，如 LIMIT 2,3 跳过2个数据，查3个，也可以用 OFFSET 实现

-- 连接查询

SELECT [字段]
FROM 表1
INNER JOIN 表2 ON 表1.关联字段 = 表2.关联字段;

SELECT [字段]
FROM 表1
LEFT JOIN 表2 ON 表1.关联字段 = 表2.关联字段;

SELECT [字段]
FROM 表1
RIGHT JOIN 表2 ON 表1.关联字段 = 表2.关联字段;

SELECT [字段]
FROM 表1
FULL JOIN 表2 ON 表1.关联字段 = 表2.关联字段;


-- 子查询与 CTE

-- 子查询放在 where(与一个值比较) 和 select(查询成某个字段) 时只能为一列，
-- 但是放在 from(当做临时表) 时可以有很多列

SELECT [字段]
FROM (
    SELECT [字段]
    FROM [表名]
    WHERE [筛选条件]
) AS 子查询;

-- WITH 就像临时创建了一张表，只能在当前查询使用，可以让子查询更容易看

WITH 临时结果 AS (
    SELECT [字段]
    FROM [表名]
    WHERE [筛选条件]
)
SELECT [字段]
FROM 临时结果;

-- 窗口函数，可以把一个操作传播到相关行
-- 如 sum(sales) OVER () as sample，就会出来一列 sample，值都为sales的求和
-- 默认窗口大小为所有的，但是也可以分组，如使用 PARTITION BY 进行分组
-- 如果加上 ORDER BY 那么就会对每一组按照排序的方式执行前面的函数
-- 排序如果相等的两行值会一样，忽略这个限制的方法就是:
-- ROWS BETWEEN UNBOUNDED PRECEDING AND CURRENT ROW : 从组开头到当前行。
-- ROWS BETWEEN 1 PRECEDING AND 1 FOLLOWING ：前一行到后一行，做滑动窗口。


SELECT [字段],
       ROW_NUMBER() OVER (PARTITION BY [分组字段] ORDER BY [排序字段]) AS 行号,
       SUM([数值字段]) OVER (PARTITION BY [分组字段] ORDER BY [排序字段]
                             ROWS BETWEEN UNBOUNDED PRECEDING AND CURRENT ROW) AS 累计值
FROM [表名];

-- 结果集合并

-- 直接拼起来，不去重
SELECT [字段] FROM 表1
UNION ALL -- 不加 ALL 可以去重
SELECT [字段] FROM 表2;

-- 只保留两边都出现的行
SELECT [字段] FROM 表1
INTERSECT
SELECT [字段] FROM 表2;

-- 保留第一个结果里有但是第二个里面没有的
SELECT [字段] FROM 表1
EXCEPT
SELECT [字段] FROM 表2;

-- 条件表达式，可以增加一列成新的结果

SELECT CASE
           WHEN [条件1] THEN [结果1]
           WHEN [条件2] THEN [结果2]
           ELSE [默认结果]
       END AS 分类结果
FROM [表名];

-- 实例
SELECT order_id,
       amount,
       CASE
           WHEN amount >= 200 THEN '高额订单'
           WHEN amount >= 100 THEN '普通订单'
           ELSE '小额订单'
       END AS level
FROM orders;

-- 调优提示

EXPLAIN SELECT [字段]
FROM [表名]
WHERE [筛选条件]; -- 查看执行计划，关注索引使用情况
