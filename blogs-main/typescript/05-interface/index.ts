// interface 用于定义一个对象必须有什么属性和方法

// 1. 最基础的演示，interface类似于一个契约，不能多属性，也不能少属性
interface Person1 {
  name: string
  age: number
  live: boolean
  print: () => void
}

let person1: Person1 = {
  name: "chulan",
  age: 20,
  live: true,
  print: () => { }
}

// 2. 若是两个interface重名，则会进行一个重合，即对象应当具有所有同名接口定义的属性
interface Person2 {
  name: string
}

interface Person2 {
  age: number
}

let person2: Person2 = {
  name: 'chulan',
  age: 20
}

// 3. 调用接口后后台返回好多数据，但是我们只关注其中某几个，可以用索引签名的方式实现
interface Person3 {
  // 前面两个属性是强校验的
  name: string
  age: number
  [elseElement: string]: any  // 可以对应任意个key，但是don't care
}

let person3: Person3 = {
  name: 'chulan',
  age: 20,
  a: 1,
  b: 2
}

// 4. ？与readonly
interface Person4 {
  name: string,
  age?: number,                 // 表示可选，有没有都ok
  readonly cb: () => boolean    // 加上readonly，表示只读
}

let person4: Person4 = {
  name: 'chulan',
  cb: () => {
    return false
  }
}
// person4.cb = () => true // error

// 5. 接口的继承：extends
interface Person5 extends Person6, Person7 {
  name: string
}

interface Person6 {
  age: number
}

interface Person7 {
  love: string
}

let person5: Person5 = {
  name: 'chulan',
  age: 100,
  love: 'bbb'
}

// 6. 定义函数类型
interface Func {
  (name: string): number[]
}

const func: Func = () => {
  return [1, 2]
}
func('chulan')