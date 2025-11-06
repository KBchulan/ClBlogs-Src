package main

import (
	"bytes"
	"log"
	"time"

	amqp "github.com/rabbitmq/amqp091-go"
)

func main() {
	conn, err := amqp.Dial("amqp://guest:guest@localhost:5672/")
	if err != nil {
		log.Fatalf("Cann't connect to RabbitMQ: %v", err)
	}
	defer conn.Close()

	ch, err := conn.Channel()
	if err != nil {
		log.Fatalf("Cann't open a channel: %v", err)
	}
	defer ch.Close()

	q, err := ch.QueueDeclare("task_queue", false, true, false, false, nil)
	if err != nil {
		log.Fatalf("Cann't declare a queue: %v", err)
	}

	msgs, err := ch.Consume(q.Name, "", true, false, false, false, nil)
	if err != nil {
		log.Fatalf("Cann't register a consumer: %v", err)
	}

	log.Printf(" [*] 等待任务中。按 CTRL+C 退出")

	for d := range msgs {
		log.Printf(" [x] 收到 %s", d.Body)

		dotCount := bytes.Count(d.Body, []byte("."))
		t := time.Duration(dotCount)
		time.Sleep(t * time.Second)

		log.Printf("Task complete")
	}
}
