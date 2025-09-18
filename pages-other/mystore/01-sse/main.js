const express = require('express')
const cors = require('cors')

const app = express()
app.use(cors())

app.post('/sse', (req, res) => {
  // SSE 必须用这个响应头
  res.setHeader('Content-Type', 'text/event-stream')
  // 基于 HTTP 1.1 实现，必须设置为长连接
  res.setHeader('Connection', 'keep-alive')

  setInterval(() => {
    // 同时必须是 write 发送，send和json都不可以
    res.write('event: random\n') // event: 事件名称\n
    res.write(`id: ${Math.random()}\n`) // id: 事件 ID\n
    res.write(`data: ${Math.random()}\n\n`)  // data: 数据\n\n
  }, 1000)
})

app.listen(3000, () => {
  console.log('server is running on http://localhost:3000')
})