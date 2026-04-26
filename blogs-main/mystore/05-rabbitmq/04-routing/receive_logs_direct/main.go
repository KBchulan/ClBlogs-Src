package main

import (
	"log"
	"os"

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

	// 3. 声明 exchange（与发布者保持一致）
	err = ch.ExchangeDeclare("logs_direct", "direct", false, true, false, false, nil)
	if err != nil {
		log.Fatalf("无法声明 exchange: %v", err)
	}

	// 4. 声明临时队列
	q, err := ch.QueueDeclare("", false, true, true, false, nil)
	if err != nil {
		log.Fatalf("无法声明队列: %v", err)
	}

	// 5. 从命令行获取要订阅的日志级别
	// 用法: go run main.go error warning
	//      go run main.go info
	if len(os.Args) < 2 {
		log.Printf("用法: %s [info] [warning] [error]", os.Args[0])
		os.Exit(0)
	}

	// 6. 为每个日志级别绑定队列
	for _, severity := range os.Args[1:] {
		log.Printf(" [*] 绑定队列到 routing key: %s", severity)
		err = ch.QueueBind(q.Name, severity, "logs_direct", false, nil)
		if err != nil {
			log.Fatalf("无法绑定队列: %v", err)
		}
	}

	// 7. 消费消息
	msgs, err := ch.Consume(q.Name, "", true, false, false, false, nil)
	if err != nil {
		log.Fatalf("无法注册消费者: %v", err)
	}

	log.Printf(" [*] 等待日志消息。按 CTRL+C 退出")

	// 8. 处理消息
	for d := range msgs {
		log.Printf(" [x] 收到 [%s] 日志: %s", d.RoutingKey, d.Body)
	}
}
