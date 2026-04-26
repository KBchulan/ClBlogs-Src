package main

import (
	"context"
	"log"
	"os"
	"time"

	amqp "github.com/rabbitmq/amqp091-go"
)

func main() {
	// 1. 连接
	conn, err := amqp.Dial("amqp://guest:guest@localhost:5672/")
	if err != nil {
		log.Fatalf("无法连接: %v", err)
	}
	defer conn.Close()

	// 2. 创建通道
	ch, err := conn.Channel()
	if err != nil {
		log.Fatalf("无法创建通道: %v", err)
	}
	defer ch.Close()

	// 3. 声明普通队列
	q, err := ch.QueueDeclare("message_ttl_demo", false, true, true, false, nil)
	if err != nil {
		log.Fatalf("无法声明队列: %v", err)
	}

	// 4. 获取 TTL 时间（从命令行参数）
	// 用法: go run producer.go 5000  # 5秒后过期
	ttl := "10000"
	if len(os.Args) >= 2 {
		ttl = os.Args[1]
	}

	ctx, cancel := context.WithTimeout(context.Background(), 5*time.Second)
	defer cancel()

	// 5. 发布消息（设置消息级别 TTL）
	err = ch.PublishWithContext(ctx, "", q.Name, false, false, amqp.Publishing{
		Expiration:  ttl, // 消息过期时间（字符串）
		ContentType: "text/plain",
		Body:        []byte("Message with TTL " + ttl + "ms"),
	})
	if err != nil {
		log.Fatalf("发布失败: %v", err)
	}

	log.Printf(" [x] 发送消息(TTL=%sms)", ttl)
}
