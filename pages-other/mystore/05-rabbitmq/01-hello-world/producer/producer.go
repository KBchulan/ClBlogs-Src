package main

import (
	"context"
	"log"
	"time"

	"github.com/rabbitmq/amqp091-go"
)

func main() {
	// 1. 连接到 RabbitMQ
	conn, err := amqp091.Dial("amqp://guest:guest@localhost:5672/")
	if err != nil {
		log.Fatalf("Cann't connect to RabbitMQ: %v", err)
	}
	defer conn.Close()

	// 2. 创建一个通道
	ch, err := conn.Channel()
	if err != nil {
		log.Fatalf("Cann't open a channel: %v", err)
	}
	defer ch.Close()

	// 3. 声明队列
	q, err := ch.QueueDeclare(
		"hello", // 队列名称
		false,   // durable: 是否持久化
		false,   // autoDelete: 是否自动删除
		false,   // exclusive: 是否独占
		false,   // noWait: 是否等待服务器确认
		nil,     // arguments: 额外参数
	)
	if err != nil {
		log.Fatalf("Cann't declare a queue: %v", err)
	}

	// 4. 准备要发布的消息
	body := "Hello RabbitMQ"
	ctx, cancel := context.WithTimeout(context.Background(), time.Second*5)
	defer cancel()

	// 5. 发布消息到队列
	err = ch.PublishWithContext(
		ctx,
		"",     // exchange: 交换机名称，如果为空字符串表示默认交换机
		q.Name, // routing key: 路由键，这里就是队列名
		false,  // mandatory: 如果无法路由是否返回
		false,  // immediate: 是否立即投递
		amqp091.Publishing{
			ContentType: "text/plain",
			Body:        []byte(body),
		},
	)
	if err != nil {
		log.Fatalf("Cann't publish a message: %v", err)
	}
	log.Printf(" [x] Sent %s", body)
}
