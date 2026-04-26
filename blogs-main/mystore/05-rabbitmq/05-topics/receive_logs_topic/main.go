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

	// 3. 声明 exchange
	err = ch.ExchangeDeclare("logs_topic", "topic", false, true, false, false, nil)
	if err != nil {
		log.Fatalf("无法声明 exchange: %v", err)
	}

	// 4. 声明临时队列
	q, err := ch.QueueDeclare("", false, true, true, false, nil)
	if err != nil {
		log.Fatalf("无法声明队列: %v", err)
	}

	// 5. 从命令行获取 binding key（支持通配符！）
	// 用法: go run main.go "*.error"      # 所有 error
	//      go run main.go "app.#"        # 所有 app 开头的
	//      go run main.go "#"            # 所有消息
	if len(os.Args) < 2 {
		log.Printf("用法: %s [binding_key]...", os.Args[0])
		log.Printf("示例:")
		log.Printf("  %s \"*.error\"         # 匹配所有 error",
			os.Args[0])
		log.Printf("  %s \"app.#\"           # 匹配所有 app 开头",
			os.Args[0])
		log.Printf("  %s \"#\"               # 匹配所有消息", os.Args[0])
		log.Printf("  %s \"kern.*\" \"*.critical\" # 多个模式",
			os.Args[0])
		os.Exit(0)
	}

	// 6. 为每个 binding key 绑定队列（支持通配符！）
	for _, bindingKey := range os.Args[1:] {
		log.Printf(" [*] 绑定队列到 pattern: %s", bindingKey)
		err = ch.QueueBind(
			q.Name,       // 队列名称
			bindingKey,   // ⭐ binding key: 支持通配符 * 和 #
			"logs_topic", // exchange 名称
			false,        // noWait
			nil,          // arguments
		)
		if err != nil {
			log.Fatalf("无法绑定队列: %v", err)
		}
	}

	// 7. 消费消息
	msgs, err := ch.Consume(
		q.Name, // queue
		"",     // consumer
		true,   // auto-ack
		false,  // exclusive
		false,  // no-local
		false,  // no-wait
		nil,    // args
	)
	if err != nil {
		log.Fatalf("无法注册消费者: %v", err)
	}

	log.Printf(" [*] 等待日志消息。按 CTRL+C 退出")

	// 8. 处理消息
	for d := range msgs {
		log.Printf(" [x] 收到 [%s]: %s", d.RoutingKey, d.Body)
	}
}
