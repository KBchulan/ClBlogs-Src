// 本节我们介绍一下生成器和迭代器

// 1.生成器
function* gen() {
  yield 'chulan'                  // 同步
  yield Promise.resolve('chulan') //异步
}

const xi = gen()
console.log(xi.next())  // { value: 'chulan', done: false }
console.log(xi.next())  // { value: Promise { 'chulan' }, done: false }
// 若是后面的done为true，表示没有东西可供迭代了

// 类型：yieldType, returnTtpe, nextType
function* dialogue(): Generator<string, void, string> {
  const response1 = yield '你好吗?'
  console.log(response1)
}
const dia = dialogue()
console.log(dia.next().value)

// 2.迭代器

// 先创建set和map，后续使用
// set 是非重复集合(c++会自动升序)
let set: Set<number> = new Set([1, 2, 6, 2, 1])

// map的key比对象更为宽泛，它可以使用引用类型作为参数
let map: Map<number, string> = new Map()
map.set(1, 'aaa')
let iterator = map[Symbol.iterator]()

// 此外还有许多如arguments,querySelectorAll等伪数组，那有没有一种统一的格式支持所有的遍历

// 有的兄弟，有的，就是我们的iterator
const each = (value: any) => {
  let it: any = value[Symbol.iterator]()
  let next: any = { done: false }
  while (!next.done) {
    next = it.next()
    if (!next.done) {
      console.log(next.value)
    }
  }
}

// 迭代器的语法糖: for of(补充：如果是异步的还可以写成 for await (...of ))
// 但是for of不能使用对象的遍历
for (let value of map) {
  console.log(value)
}

// 解构的底层也是iterator
let [a, b, c] = [1, 2, 3]

let a2 = [2, 3, 6]
let a3 = [...a2]

// 我们来手搓一下，让这个对象也可以被for of 或者被解构
let obj = {
  max: 5,
  current: 0,
  [Symbol.iterator]() {
    return {
      max: this.max,
      current: this.current,
      next() {
        if (this.current === this.max) {
          return {
            value: undefined,
            done: true
          }
        }
        else {
          return {
            value: this.current++,
            done: false
          }
        }
      }
    }
  }
}

for (let value of obj) {
  console.log(value)
}

// 后话：
// interface Generator<T = unknown, TReturn = any, TNext = unknown> extends Iterator<T, TReturn, TNext> {
//     // NOTE: 'next' is defined using a tuple to ensure we report the correct assignability errors in all places.
//     next(...args: [] | [TNext]): IteratorResult<T, TReturn>;
//     return(value: TReturn): IteratorResult<T, TReturn>;
//     throw(e: any): IteratorResult<T, TReturn>;
//     [Symbol.iterator](): Generator<T, TReturn, TNext>;
// }
// 其实生成器就是迭代器的一个子实现