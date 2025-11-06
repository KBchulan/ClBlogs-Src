package main

import (
	"log"

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

	// 3. 声明队列（与生产者保持一致）
	q, err := ch.QueueDeclare(
		"message_ttl_demo", // 队列名（改成对应的队列）
		false,
		false,
		false,
		false,
		nil, // 消费者声明时不需要再设置 TTL
	)
	if err != nil {
		log.Fatalf("无法声明队列: %v", err)
	}

	// 4. 消费消息
	msgs, err := ch.Consume(
		q.Name,
		"",
		true, // auto-ack
		false,
		false,
		false,
		nil,
	)
	if err != nil {
		log.Fatalf("无法注册消费者: %v", err)
	}

	log.Printf(" [*] 等待消息...")

	// 5. 处理消息
	for d := range msgs {
		log.Printf(" [x] 收到: %s", d.Body)
	}
}
