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
	// 1. 连接到 RabbitMQ
	conn, err := amqp.Dial("amqp://guest:guest@localhost:5672/")
	if err != nil {
		log.Fatalf("无法连接到 RabbitMQ: %v", err)
	}
	defer conn.Close()

	// 2. 创建通道
	ch, err := conn.Channel()
	if err != nil {
		log.Fatalf("无法创建通道: %v", err)
	}
	defer ch.Close()

	// 3. 声明一个 fanout 类型的 exchange
	err = ch.ExchangeDeclare(
		"logs",   // exchange 名称
		"fanout", // exchange 类型：fanout = 广播
		false,    // durable: 是否持久化
		true,     // autoDelete: 是否自动删除
		false,    // internal: 是否为内部使用
		false,    // noWait: 是否等待服务器确认
		nil,      // arguments: 额外参数
	)
	if err != nil {
		log.Fatalf("无法声明 exchange: %v", err)
	}

	// 4. 准备消息
	body := bodyFrom(os.Args)
	ctx, cancel := context.WithTimeout(context.Background(), 5*time.Second)
	defer cancel()

	// 5. 发布消息到 exchange
	err = ch.PublishWithContext(ctx, "logs", "", false, false, amqp.Publishing{
		ContentType: "text/plain",
		Body:        []byte(body),
	})
	if err != nil {
		log.Fatalf("发布消息失败: %v", err)
	}

	log.Printf(" [x] 发送日志: %s", body)
}

func bodyFrom(args []string) string {
	var s string
	if len(args) < 2 || args[1] == "" {
		s = "info: Hello World!"
	} else {
		s = strings.Join(args[1:], " ")
	}
	return s
}
