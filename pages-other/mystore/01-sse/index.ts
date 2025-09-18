// const sse: EventSource = new EventSource("http://localhost:3000/sse")
const content: HTMLDivElement = document.getElementById("content") as HTMLDivElement

// const arr: string[] = []
// sse.addEventListener("random", (event) => {
//   console.log("Received message:", event.data)
//   arr.push(...(event.data as string).split(''))
//   content.innerHTML += arr.shift()
// })

async function startSSE() {
  const response = await fetch('http://localhost:3000/sse', {
    method: "POST",
    headers: {
      'Content-Type': 'application/json'
    },
    body: JSON.stringify({
      message: "Hello, server!"
    })
  })

  // 我们的数据都会在 response.body的reader 中
  const reader = response.body?.getReader()

  const decoder = new TextDecoder("utf-8")
  while (true) {
    const { done, value } = await reader!.read()

    if (done) {
      break
    }

    const data = decoder.decode(value, { stream: true })
    const arr = data.split('\n')
    arr.pop()
    arr.pop()
    for (let value of arr) {
      if (value.startsWith('data: ')) {
        content.innerHTML += value.split(': ')[1]
      }
    }
  }
}

startSSE()