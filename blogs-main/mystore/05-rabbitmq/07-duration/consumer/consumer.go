package main

import (
	"bytes"
	"log"
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

	// 3. 声明持久化队列（与生产者一致）⭐
	q, err := ch.QueueDeclare(
		"task_queue_durable", // 队列名
		true,                 // ⭐⭐⭐ durable: true
		false,                // autoDelete
		false,                // exclusive
		false,                // noWait
		nil,                  // arguments
	)
	if err != nil {
		log.Fatalf("无法声明队列: %v", err)
	}

	// 4. 消费消息
	msgs, err := ch.Consume(
		q.Name, // queue
		"",     // consumer
		false,  // auto-ack: false（配合手动 ACK）
		false,  // exclusive
		false,  // no-local
		false,  // no-wait
		nil,    // args
	)
	if err != nil {
		log.Fatalf("无法注册消费者: %v", err)
	}

	log.Printf(" [*] 等待持久化消息。按 CTRL+C 退出")

	// 5. 处理消息
	for d := range msgs {
		log.Printf(" [x] 收到: %s", d.Body)

		// 模拟处理
		dotCount := bytes.Count(d.Body, []byte("."))
		time.Sleep(time.Duration(dotCount) * time.Second)

		log.Printf(" [✓] 完成")
		d.Ack(false) // 手动确认
	}
}
