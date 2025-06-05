// 本文档介绍Object, object, {}的区别

// 1. Object：所有类型的顶级类型，在原型链是顶级的

let a1: Object = 123        // 基本类型 ok
let a2: Object = '123'      // 字符串 ok
let a3: Object = true       // 布尔值 ok
let a4: Object = Symbol()   // 符号 ok
let a5: Object = 100n       // BigInt ok
let a6: Object = {}         // 对象 ok
let a7: Object = []         // 数组 ok
let a8: Object = () => 213  // 函数 ok

// 但是不能访问具体方法
let aaa: Object = {
  name: '张三',
  age: 20
}
// console.log(aaa.name) // error

// 2. object：所有非原始类型，具体可以看一看上一节最后一块内容
// let b1: object = 123;     // 错误：基本类型不能赋值给 object
// let b2: object = '123';   // 错误：字符串不能赋值给 object
// let b3: object = true;    // 错误：布尔值不能赋值给 object

let b4: object = {}         // ok
let b5: object = []         // ok
let b6: object = () => 213  // ok

// 引用类型都可以
let set: object = new Set([1, 2, 3])
let map: object = new Map([["name", true]])
let date: object = new Date()

// 3. {}：对象字面量类型，和 Object 类似，但有更多限制，它不可以访问原型链上的任何东西