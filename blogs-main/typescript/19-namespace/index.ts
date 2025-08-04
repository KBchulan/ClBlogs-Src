// 基本使用
namespace Name1 {
  export let num: number = 15
  export const add = (a: number, b: number) => a + b
}

console.log(Name1.num)
console.log(Name1.add(2, 5))

// 嵌套
namespace Name2 {
  export namespace Son {
    export let num: number = 55
  }
}

console.log(Name2.Son.num)

// 合并
namespace Name1 {
  export let num2InOther: number = 65
}

console.log(Name1.num2InOther)

// 抽离
import { Test } from "./test"
console.log(Test.a)

// 实际案例
namespace ios {
  export const pushMessage = (msg: string, type: string) => {}
}

namespace android {
  export const pushMessage = (msg: string, type: string) => {}
}
