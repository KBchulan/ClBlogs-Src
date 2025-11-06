package main

import (
	"log"

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

	// 3. 声明 exchange，与生产者保持一致
	err = ch.ExchangeDeclare("logs", "fanout", false, true, false, false, nil)
	if err != nil {
		log.Fatalf("无法声明 exchange: %v", err)
	}

	// 4. 声明一个临时队列，需要设置为独占队列
	q, err := ch.QueueDeclare("", false, true, true, false, nil)
	if err != nil {
		log.Fatalf("无法声明队列: %v", err)
	}

	// 5. 将队列绑定到 exchange
	err = ch.QueueBind(
		q.Name, // 队列名称
		"",     //
		"logs", // exchange 名称
		false,  // noWait
		nil,    // arguments
	)
	if err != nil {
		log.Fatalf("无法绑定队列: %v", err)
	}

	// 6. 消费消息
	msgs, err := ch.Consume(q.Name, "", true, false, false, false, nil)
	if err != nil {
		log.Fatalf("无法注册消费者: %v", err)
	}

	log.Printf(" [*] 等待日志消息。按 CTRL+C 退出")
	log.Printf(" [*] 队列名称: %s", q.Name)

	// 7. 处理消息
	for d := range msgs {
		log.Printf(" [x] 收到日志: %s", d.Body)
	}
}
