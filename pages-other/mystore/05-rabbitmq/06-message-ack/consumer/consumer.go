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

	q, err := ch.QueueDeclare("task_queue_ack", false, true, false, false, nil)
	if err != nil {
		log.Fatalf("Cann't declare a queue: %v", err)
	}

	// autoAck 设置为 false
	msgs, err := ch.Consume(q.Name, "", false, false, false, false, nil)
	if err != nil {
		log.Fatalf("Cann't register a consumer: %v", err)
	}

	for d := range msgs {
		log.Printf("Received a message: %s", d.Body)

		dotCount := bytes.Count(d.Body, []byte("."))
		t := time.Duration(dotCount)
		time.Sleep(t * time.Second)

		log.Printf(" [✓] 任务完成")

		// 手动确认消息
		// false 表示只确认当前消息
		// true 表示确认当前及之前所有未确认的消息
		d.Ack(false)
	}
}

/*
// Nack: 可以批量拒绝
  d.Nack(
      false, // multiple: false=只拒绝当前，true=拒绝当前及之前所有
      true,  // requeue: true=重新入队，false=丢弃
  )

  // Reject: 只能拒绝单条
  d.Reject(true) // requeue: true=重新入队, false=丢弃
*/
