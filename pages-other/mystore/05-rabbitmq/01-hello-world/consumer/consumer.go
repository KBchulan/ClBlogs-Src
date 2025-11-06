package main

import (
	"log"

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

	// 3. 声明一个队列，要和生产者的队列名称一致
	q, err := ch.QueueDeclare(
		"hello", // name
		false,   // durable
		false,   // delete when unused
		false,   // exclusive
		false,   // no-wait
		nil,     // arguments
	)
	if err != nil {
		log.Fatalf("Cann't declare a queue: %v", err)
	}

	// 4. 接收消息
	msgs, err := ch.Consume(
		q.Name,
		"",    // consumer: 消费者标签
		true,  // autoAck: 自动确认
		false, // exclusive: 是否独占
		false, // noLocal: 是否接收同一连接中发布的消息
		false, // noWait: 是否等待服务器确认
		nil,   // args: 额外参数
	)
	if err != nil {
		log.Fatalf("Cann't register a consumer: %v", err)
	}

	forever := make(chan struct{})

	go func() {
		for d := range msgs {
			log.Printf("[x] Received a message: %s", d.Body)
		}
	}()

	log.Printf(" [*] Waiting for messages. To exit press CTRL+C")
	<-forever
}
