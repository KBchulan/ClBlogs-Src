// var, const, let 都可以用于定义变量
var a: number = 1
const b: number = 2
let c: number = 3

// var 会提升变量的定义到作用域的顶部
function printA() {
  var value1: number = 1
  if (true) {
    var value1: number = 2
    console.log(value1)  // 2
  }
  console.log(value1)    // 2
}

// let 只影响当前作用域的变量
function printC() {
  let value2: number = 1
  if (true) {
    let value2: number = 2
    console.log(value2)  // 2
  }
  console.log(value2)    // 1
}

printA()
printC()

// 如何运行呢
// 我们可以使用tsc -w进行监听，会生成我们的ts对应的js，然后我们可以使用node运行
/*
tsc -w
node index.js
*/

// 数字类型
let num0: number = 111       // 普通数字
let num1: number = NaN       // not a number
let num2: number = Infinity  // 无穷大
let num3: number = 0b111     // 二进制
let num4: number = 0o77      // 八进制
let num5: number = 0xA56D    // 十六进制

// 字符串
let str0: string = 'chulan'   // 普通字符串
let str1: string = `${num0}`  // 模板字符串

// 布尔值
let bool0: boolean = true
let bool1: boolean = false

console.log(num0, num1, num2, num3, num4, num5, str0, str1, bool0, bool1)
// 打印结果：111 NaN Infinity 7 63 42349 chulan 111 true false

// 空值和未定义
let null0: null = null
let undefined0: undefined = undefined

console.log(null0, undefined0)
// 打印结果：null undefined

// void，基本是用在函数没有返回值的时候
let v1: void = undefined
// let v2: void = null

// v2 会报错，因为严格模式下，void 不能赋值为 null
// 但是我们可以使用tsc --init，生成tsconfig.json，然后修改strict为false，就可以赋值为null
// 同样严格模式下null和undefined不能彼此赋值
