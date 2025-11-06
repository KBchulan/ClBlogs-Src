package main

import (
	"context"
	"log"
	"os"
	"strings"
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

	// 3. 声明死信交换机
	err = ch.ExchangeDeclare("dlx_exchange", "direct", false, true, false, false, nil)
	if err != nil {
		log.Fatalf("无法声明死信交换机: %v", err)
	}

	// 4. 声明死信队列
	dlq, err := ch.QueueDeclare("dead_letter_queue", false, true, false, false, nil)
	if err != nil {
		log.Fatalf("无法声明死信队列: %v", err)
	}

	// 5. 绑定死信队列到死信交换机
	err = ch.QueueBind(dlq.Name, "dead", "dlx_exchange", false, nil)
	if err != nil {
		log.Fatalf("无法绑定死信队列: %v", err)
	}

	// 6. 声明业务队列（配置 DLX）
	args := amqp.Table{
		"x-message-ttl":             int32(10000),   // 10秒过期
		"x-dead-letter-exchange":    "dlx_exchange", // 死信交换机
		"x-dead-letter-routing-key": "dead",         // 死信路由键
	}

	q, err := ch.QueueDeclare("business_queue", false, true, false, false, args)
	if err != nil {
		log.Fatalf("无法声明业务队列: %v", err)
	}

	// 7. 获取消息内容
	body := bodyFrom(os.Args)

	ctx, cancel := context.WithTimeout(context.Background(), 5*time.Second)
	defer cancel()

	// 8. 发布消息到业务队列
	err = ch.PublishWithContext(ctx, "", q.Name, false, false, amqp.Publishing{
		ContentType: "text/plain",
		Body:        []byte(body),
	})
	if err != nil {
		log.Fatalf("发布失败: %v", err)
	}

	log.Printf(" [x] 发送消息(10秒后过期 → 死信队列): %s", body)
}

func bodyFrom(args []string) string {
	var s string
	if len(args) < 2 || args[1] == "" {
		s = "Hello World!"
	} else {
		s = strings.Join(args[1:], " ")
	}
	return s
}
