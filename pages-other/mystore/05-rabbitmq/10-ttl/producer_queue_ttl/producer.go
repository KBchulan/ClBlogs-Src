package main

import (
	"context"
	"log"
	"os"
	"strings"
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

	args := amqp091.Table{
		"x-expires":     int64(30000), // 30 seconds
		"x-message-ttl": int64(10000), // 10 seconds
	}

	q, err := ch.QueueDeclare("queue_ttl_demo", false, true, false, false, args)
	if err != nil {
		log.Fatalf("Cann't declare a queue: %v", err)
	}

	body := bodyFrom(os.Args)
	ctx, cancel := context.WithTimeout(context.Background(), time.Second*5)
	defer cancel()

	err = ch.PublishWithContext(ctx, "", q.Name, false, false, amqp091.Publishing{
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
	if len(args) < 2 || args[1] == "" {
		s = "Hello World!"
	} else {
		s = strings.Join(args[1:], " ")
	}
	return s
}
