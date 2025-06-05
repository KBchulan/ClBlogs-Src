// 本节介绍一下数组相关的内容

// 通过基本数据类型来定义，下面为原始数据类型
let arr2: string[] = ['a', 'b', 'c']
let arr1: number[] = [1, 2, 3]
let arr3: boolean[] = [true, false]
let arr4: symbol[] = [Symbol(), Symbol()]
let arr5: bigint[] = [2n ** 1024n, 900n ** 1024n]

// 泛型的方式
let a1: Array<number> = [1, 2, 3]

// 如果需要多种类型
let arr6: (number | string)[] = [1, 'a']
let arr7: Array<number | string> = [1, 'a']

// 对象数组
interface IUser {
  name: string
  age: number
}

let arr8: IUser[] = [{ name: '张三', age: 18 }, { name: '李四', age: 20 }]

// 多维数组
let arr9: number[][] = [[1, 2, 3], [4, 5, 6]]
let arr10: Array<Array<number>> = [[1, 2, 3], [4, 5, 6]]

// 函数
let a5 = (...args: number[]) => { console.log(args) }
a5(1, 2, 3)

// 补充三个知识点(非ts独有): ...  arguments  遍历

// ...为展开/收集运算符，args则是所有传入参数的一个数组，因此args方法都是数组方法
// 做参数时可以表示收集，但是若是调用加上...则表示展开
let tmp1: number[] = [1, 2, 3]
let tmp2: number[] = [...tmp1, 4, 5]
console.log(tmp2)

// arguments其实是一个包含所有函数参数的伪数组
function a6(...args: any[]) {
  console.log(arguments)         // [Arguments] { '0': 1, '1': true }
  // let a: any[] = arguments    // 这是错误的，因为arguments不是一个数组

  let a: IArguments = arguments  // 我们可以使用IArguments内置对象来接这个
}

// 这里说一下基本的遍历写法
const arr11: number[] = [1, 2, 3]

// 传统
for (let i = 0; i < arr11.length; i++) {
  console.log(arr11[i])
}

// for ... in
for (const key in arr11) {
  console.log(arr11[key])
}

// for ... of
for (const item of arr11) {
  console.log(item)
}

// 内置方法
arr11.forEach((value, index, arr) => {
  console.log(value, index, arr)
})

// entries：迭代器的方案，对应还有keys和values
for (const [index, value] of arr11.entries()) {
  console.log(`index: ${index}, value: ${value}`)
}