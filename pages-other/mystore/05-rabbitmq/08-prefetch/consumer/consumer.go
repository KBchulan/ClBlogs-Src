package main

import (
	"bytes"
	"log"
	"time"

	"github.com/rabbitmq/amqp091-go"
)

func main() {
	conn, err := amqp091.Dial("amqp://guest:guest@localhost:5672/")
	if err != nil {
		log.Fatalf("Cann't connect to RabbitMQ: %v", err)
	}
	defer conn.Close()

	ch, err := conn.Channel()
	if err != nil {
		log.Fatalf("Cann't open a channel: %v", err)
	}
	defer ch.Close()

	q, err := ch.QueueDeclare("task_queue_fair", false, true, false, false, nil)
	if err != nil {
		log.Fatalf("Cann't declare a queue: %v", err)
	}

	// 设置 Qos
	err = ch.Qos(
		1,     // prefetchCount: 每次只预取1条消息
		0,     // prefetchSize: 0 表示不限制大小
		false, // global: false 表示只应用于当前 channel
	)
	if err != nil {
		log.Fatalf("Cann't set Qos: %v", err)
	}

	msgs, err := ch.Consume(q.Name, "", false, false, false, false, nil)
	if err != nil {
		log.Fatalf("Cann't register a consumer: %v", err)
	}

	log.Printf(" [*] 等待任务(Qos=1, 公平分发)。按 CTRL+C 退出")

	// 6. 处理消息
	for d := range msgs {
		log.Printf(" [x] 收到: %s", d.Body)

		// 模拟耗时任务
		dotCount := bytes.Count(d.Body, []byte("."))
		time.Sleep(time.Duration(dotCount) * time.Second)

		log.Printf(" [✓] 完成")
		d.Ack(false)
	}
}
