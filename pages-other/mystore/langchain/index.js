import { ChatOpenAI } from '@langchain/openai'
import { env } from './env.js'
import express from 'express'
import cors from 'cors'

const app = express()
app.use(express.json())
app.use(cors())

const model = new ChatOpenAI({
  openAIApiKey: env.OPENAI_API_KEY,   // API Key
  temperature: 1.3,                   // 通用模型
  modelName: 'deepseek-chat',         // 模型名称

  // 配置选项
  configuration: {
    baseURL: 'https://api.deepseek.com',
  }
})

// 获取消息
const result = await model.invoke('你好')
console.log(result.content)

// 流式获取消息
const result2 = await model.stream('你好')

