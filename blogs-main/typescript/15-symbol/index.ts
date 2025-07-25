// 本节我们介绍一下symbol这种类型

// symbol是一种唯一表示符号类型，每一个symbol都是独一无二的
let a1: symbol = Symbol(1)
let a2: symbol = Symbol(1)

console.log(a1 === a2)  // false

// for会找全局有没有注册过某个key值，普通Symbol则只会直接注册
console.log(Symbol.for('huaixi') === Symbol.for('huaixi'))  // true

// 实际场景：
let obj = {
  [a1]: 111,
  [a2]: 222,   // 此处[]表示计算属性名
  example: 'test'
}
console.log(obj)
console.log(obj[a1])  // []来访问

// for in读不到symbol
for (let key in obj) {
  console.log(key)
}

// 这些都是读不到这个symbol属性的
console.log(Object.keys(obj))                   // 非symbol
console.log(Object.getOwnPropertyNames(obj))    // 非symbol
// 所以我们比较常用作类的私有成员

console.log(Object.getOwnPropertySymbols(obj))
console.log(Reflect.ownKeys(obj))