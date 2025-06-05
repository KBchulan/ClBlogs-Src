// 这一节介绍一下函数

// 指定参数和返回值类型
function add(...args: number[]): number {
  return args.reduce((sum, current) => sum + current, 0)
}

const add2 = (...args: number[]): number => args.reduce((sum, current) => sum + current, 0)

// 默认参数和可选参数
function add3(a: number = 3, b: number = 5): number {
  return a + b
}

function add3else(a: number = 3, b?: number): number {
  return a + (b ?? 0)     // 空值合并运算符，如果b为null或undefined返回0,其他返回具体的值
}

// 参数为对象时
interface Person {
  name: string
  age: number
}

function add4(person1: Person = { name: 'a', age: 5 },
  person2: Person = { name: 'b', age: 6 }): number {
  return person1.age + person2.age
}

// 函数重载
let user: number[] = [1, 2, 3]

function findNum(): number[];                   // 查询所有的
function findNum(id: number): number[];         // 查询自己
function findNum(nums: number[]): number[];     // 增加一个数组

function findNum(ids?: number | number[]): number[] {
  if (typeof ids == 'number') {
    return user.filter(v => v == ids)
  }
  else if (Array.isArray(ids)) {
    user.push(...ids)
    return user
  }
  else {
    return user
  }
}

// 使用类型别名定义函数类型
type MathOperation = (a: number, b: number) => number

const multiply: MathOperation = (a, b) => a * b
const divide: MathOperation = (a, b) => a / b

// 使用接口定义函数类型
interface Calculator {
  (operation: string, a: number, b: number): number
}

const calculator: Calculator = (operation, a, b) => {
  switch (operation) {
    case 'add': return a + b
    case 'subtract': return a - b
    default: return 0
  }
}

// 接受函数作为参数
function processArray<T>(arr: T[], processor: (item: T) => T): T[] {
  return arr.map(processor)
}

const numbers = [1, 2, 3, 4]
const doubled = processArray(numbers, x => x * 2)
console.log(doubled) // [2, 4, 6, 8]

// 返回函数
function createMultiplier(factor: number): (num: number) => number {
  return (num: number) => num * factor
}

const double = createMultiplier(2)
const triple = createMultiplier(3)
console.log(double(5))  // 10
console.log(triple(5))  // 15