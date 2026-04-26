package main

import (
	"log"
	"strconv"

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

	q, err := ch.QueueDeclare("rpc_queue", false, true, false, false, nil)
	if err != nil {
		log.Fatalf("Cann't declare a queue: %v", err)
	}

	// Qos
	err = ch.Qos(
		1, 0, false,
	)
	if err != nil {
		log.Fatalf("Cann't set Qos: %v", err)
	}

	msgs, err := ch.Consume(q.Name, "", false, false, false, false, nil)
	if err != nil {
		log.Fatalf("Cann't register a consumer: %v", err)
	}

	log.Printf(" [*] Waiting for messages. To exit press CTRL+C")

	for d := range msgs {
		n, err := strconv.Atoi(string(d.Body))
		if err != nil {
			log.Printf("Cann't convert body to int: %v", err)
		}

		log.Printf(" [.] 计算 fib(%d)", n)

		response := fib(n)

		err = ch.Publish(
			"",
			d.ReplyTo,
			false,
			false,
			amqp091.Publishing{
				ContentType:   "text/plain",
				CorrelationId: d.CorrelationId,
				Body:          []byte(strconv.Itoa(response)),
			},
		)
		if err != nil {
			log.Printf("Cann't publish a message: %v", err)
		}

		d.Ack(false)
	}
}

func fib(n int) int {
	if n <= 2 {
		return 1
	}

	return fib(n-1) + fib(n-2)
}
