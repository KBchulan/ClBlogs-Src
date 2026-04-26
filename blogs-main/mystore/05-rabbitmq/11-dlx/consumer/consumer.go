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

	// 3. 消费死信队列
	msgs, err := ch.Consume("dead_letter_queue", "", true, false, false, false, nil)
	if err != nil {
		log.Fatalf("无法注册消费者: %v", err)
	}

	log.Printf(" [*] 死信消费者启动，等待死信消息...")

	// 4. 处理死信消息
	for d := range msgs {
		log.Printf("死信消息: %s", d.Body)
		log.Printf("原因: %s", d.Headers["x-death"])
	}
}
