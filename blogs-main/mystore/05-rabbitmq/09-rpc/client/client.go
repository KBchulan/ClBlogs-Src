package main

import (
	"context"
	"log"
	"os"
	"strconv"
	"time"

	"github.com/google/uuid"
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

	replyQueue, err := ch.QueueDeclare("", false, true, true, false, nil)
	if err != nil {
		log.Fatalf("Cann't declare a queue: %v", err)
	}

	msgs, err := ch.Consume(replyQueue.Name, "", true, false, false, false, nil)
	if err != nil {
		log.Fatalf("Cann't consume a queue: %v", err)
	}

	n := bodyFrom(os.Args)
	corrid := uuid.New().String()

	ctx, cancel := context.WithTimeout(context.Background(), time.Second*5)
	defer cancel()

	err = ch.PublishWithContext(ctx, "", "rpc_queue", false, false, amqp091.Publishing{
		ContentType:   "text/plain",
		CorrelationId: corrid,
		ReplyTo:       replyQueue.Name,
		Body:          []byte(strconv.Itoa(n)),
	})
	if err != nil {
		log.Fatalf("Cann't publish a message: %v", err)
	}

	log.Printf(" [x] 请求计算 fib(%d)", n)

	// ⭐⭐⭐ 8. 等待响应（匹配 CorrelationId）
	for d := range msgs {
		if d.CorrelationId == corrid {
			result, _ := strconv.Atoi(string(d.Body))
			log.Printf(" [.] 结果: %d", result)
			break // 收到响应，退出
		}
	}
}

func bodyFrom(args []string) int {
	var n int
	if len(args) < 2 {
		n = 30
	} else {
		n, _ = strconv.Atoi(args[1])
	}
	return n
}
