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

	// 3. 声明一个 topic 类型的 exchange
	err = ch.ExchangeDeclare("logs_topic", "topic", false, true, false, false, nil)
	if err != nil {
		log.Fatalf("无法声明 exchange: %v", err)
	}

	// 4. 从命令行获取 routing key 和消息
	// 用法: go run main.go "app.db.error" "数据库连接失败"
	//      go run main.go "kernel.critical" "内核崩溃"
	routingKey := routingKeyFrom(os.Args)
	body := bodyFrom(os.Args)

	ctx, cancel := context.WithTimeout(context.Background(), 5*time.Second)
	defer cancel()

	// 5. 发布消息
	err = ch.PublishWithContext(
		ctx,
		"logs_topic", // exchange
		routingKey,   // ⭐ routing key: 用点号分隔（如 "app.db.error"）
		false,        // mandatory
		false,        // immediate
		amqp.Publishing{
			ContentType: "text/plain",
			Body:        []byte(body),
		},
	)
	if err != nil {
		log.Fatalf("发布消息失败: %v", err)
	}

	log.Printf(" [x] 发送 [%s] 日志: %s", routingKey, body)
}

// 从命令行获取 routing key
func routingKeyFrom(args []string) string {
	var s string
	if len(args) < 2 || args[1] == "" {
		s = "anonymous.info"
	} else {
		s = args[1]
	}
	return s
}

// 从命令行获取消息内容
func bodyFrom(args []string) string {
	var s string
	if len(args) < 3 || args[2] == "" {
		s = "Hello World!"
	} else {
		s = strings.Join(args[2:], " ")
	}
	return s
}
