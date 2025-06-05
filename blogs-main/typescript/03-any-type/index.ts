// 直接运行需要引入ts-node，我们先来安装一下
// npm install -g ts-node
// 此时我们就可以直接运行ts文件了，上一节也有说到这个

// 然后我们安装一下声明文件
// npm install @types/node -D


// 先来介绍一下类型层级
/*
TypeScript 类型层级：
1. any, unknown (顶级类型)
2. object, {}
3. Number, String, Boolean (包装对象类型)
4. number, string, boolean (原始类型)
5. 1, 'hello', true (字面量类型)
6. never (底层类型)
*/

// 其实高等级的类型可以覆盖低等级的类型，理解了这个本节就结束了
let a: any = 1  // 换成unknown也一样
a = 'hello'
a = 1
a = true
a = null
a = undefined
a = void 0
a = []
a = {}
a = () => { }
a = new Promise((resolve, reject) => { })
a = new Date()
a = new Error('error')
a = new Array()
a = new Object()
a = new Function()

// 两个顶级类型的区别
// 1.unknown只能赋值给unknown和any类型
let b: unknown = 1
let tmp: number = 2
// tmp = b  // error

// 2.unknown无法读任何属性，也无法调用任何方法
let c: unknown = {
  name: 'John',
  age: 20
}
// console.log(c.name); // error


// 以下列举了一些常见类型，可以先了解一下，后续会详细展开
// 原始类型：string, number, boolean, symbol, bigint
// 特殊类型：null, undefined, void, any, unknown, never
// 引用类型：Object, Array, Function, interface, class, tuple, enum, Map, Set, WeakMap, WeakSet, Promise