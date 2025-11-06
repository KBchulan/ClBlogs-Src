package main

import (
	"context"
	"log"
	"os"
	"strings"
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

	// 准备消息内容，此处从命令行参数中获取
	body := bodyFrom(os.Args)
	ctx, cancel := context.WithTimeout(context.Background(), time.Second*5)
	defer cancel()

	err = ch.PublishWithContext(ctx, "", q.Name, false, false, amqp.Publishing{
		ContentType: "text/plain",
		Body:        []byte(body),
	})
	if err != nil {
		log.Fatalf("Cann't publish a message: %v", err)
	}
	log.Printf(" [x] Sent %s", body)
}

func bodyFrom(args []string) string {
	var s string
	if (len(args) < 2) || os.Args[1] == "" {
		s = "hello"
	} else {
		s = strings.Join(args[1:], " ")
	}
	return s
}
