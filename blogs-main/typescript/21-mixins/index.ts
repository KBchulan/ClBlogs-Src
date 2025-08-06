// 对象混入
interface A {
  name :string
}

interface B {
  age: number
}

let a: A = {
  name: "chulan"
}

let b: B = {
  age: 20
}

// 如何合并两个对象
// es6 扩展运算符 浅拷贝 新类型
let c = {...a, ...b}
console.log(c)

// es6 Object.assign 浅拷贝 交叉类型
let d = Object.assign({}, a, b)
console.log(d)

// structuredClone 深拷贝
let e = structuredClone(c)
console.log(e)

// 类的混入
class Logger {
  log(message: string) {
    console.log(`Log: ${message}`)
  }
}

class Html {
  render(content: string) {
    console.log(`Rendering HTML: ${content}`)
  }
}

class App {
  run() {
    console.log("App is running")
  }
}

type Custructor<T> = new (...args: any[]) => T
function pluginMixins<T extends Custructor<App>>(base: T) {
  return class extends base {
    private logger: Logger
    private html: Html

    constructor(...args: any[]) {
      super(...args)
      this.logger = new Logger()
      this.html = new Html()
    }

    run() {
      this.logger.log("App is starting")
      super.run()
      this.html.render("Hello, World!")
    }
  }
}

const MixedApp = pluginMixins(App)
const appInstance = new MixedApp()
appInstance.run()