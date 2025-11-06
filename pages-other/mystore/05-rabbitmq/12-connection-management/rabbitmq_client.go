package connection_management

import (
	"log"
	"sync"
	"time"

	amqp "github.com/rabbitmq/amqp091-go"
)

// RabbitMQClient 是一个带自动重连的 RabbitMQ 客户端
type RabbitMQClient struct {
	url             string           // RabbitMQ 连接地址
	conn            *amqp.Connection // 连接对象
	channel         *amqp.Channel    // 通道对象
	isConnected     bool             // 连接状态
	mu              sync.RWMutex     // 读写锁，保护连接和通道
	notifyClose     chan *amqp.Error // 监听连接关闭事件
	notifyChanClose chan *amqp.Error // 监听通道关闭事件
	done            chan bool        // 关闭信号
}

// NewRabbitMQClient 创建一个新的 RabbitMQ 客户端
func NewRabbitMQClient(url string) *RabbitMQClient {
	client := &RabbitMQClient{
		url:  url,
		done: make(chan bool),
	}

	go client.handleReconnect()

	return client
}

// handleReconnect 处理连接和自动重连
func (c *RabbitMQClient) handleReconnect() {
	for {
		c.isConnected = false
		log.Println("Connecting to RabbitMQ")

		// 尝试建立连接
		if err := c.connect(); err != nil {
			log.Printf("Connect to RabbitMQ failed: %v, Wait 5 seconds to retry", err)

			select {
			case <-c.done:
				return
			case <-time.After(5 * time.Second):
			}
			continue
		}

		c.isConnected = true
		log.Println("RabbitMQ connecte successfully")

		// 等待连接或通道关闭事件
		select {
		case <-c.done:
			return
		case err := <-c.notifyClose:
			log.Printf("Connect to RabbitMQ closed: %v", err)
		case err := <-c.notifyChanClose:
			log.Printf("Channel to RabbitMQ closed: %v", err)
		}
	}
}

// connect 建立连接和通道
func (c *RabbitMQClient) connect() error {
	// 1. 建立连接
	conn, err := amqp.Dial(c.url)
	if err != nil {
		return err
	}

	// 2. 创建通道
	ch, err := conn.Channel()
	if err != nil {
		conn.Close()
		return err
	}

	// 3. 加锁更新连接和通道
	c.mu.Lock()
	c.conn = conn
	c.channel = ch
	c.mu.Unlock()

	// 4. 监听连接关闭事件
	c.notifyClose = make(chan *amqp.Error)
	c.conn.NotifyClose(c.notifyClose)

	// 5. 监听通道关闭事件
	c.notifyChanClose = make(chan *amqp.Error)
	c.channel.NotifyClose(c.notifyChanClose)

	return nil
}

// GetChannel 获取通道
func (c *RabbitMQClient) GetChannel() (*amqp.Channel, error) {
	c.mu.RLock()
	defer c.mu.RUnlock()

	if !c.isConnected {
		return nil, amqp.ErrClosed
	}

	return c.channel, nil
}

// NewChannel 创建一个新的独立通道
func (c *RabbitMQClient) NewChannel() (*amqp.Channel, error) {
	c.mu.RLock()
	defer c.mu.RUnlock()

	if !c.isConnected || c.conn == nil {
		return nil, amqp.ErrClosed
	}

	return c.conn.Channel()
}

// IsConnected 检查连接状态
func (c *RabbitMQClient) IsConnected() bool {
	c.mu.RLock()
	defer c.mu.RUnlock()
	return c.isConnected
}

// Close 关闭客户端
func (c *RabbitMQClient) Close() error {
	// 1. 发送关闭信号，停止重连循环
	close(c.done)

	// 2. 加锁关闭通道和连接
	c.mu.Lock()
	defer c.mu.Unlock()

	if c.channel != nil {
		c.channel.Close()
	}

	if c.conn != nil {
		return c.conn.Close()
	}

	c.isConnected = false
	log.Println("RabbitMQ has been closed")
	return nil
}
