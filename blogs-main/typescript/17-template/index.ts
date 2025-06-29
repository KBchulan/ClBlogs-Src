// 本节我们介绍一下泛型

// 此处只是介绍一下ts里的泛型怎么写
const huaixi = <Type>(a: Type, b: Type): Array<Type> => {
  return [a, b]
}

// 这里也可以看出，ts是只存在值传递的，没有引用传递的概念
function swap<Type>(a: Type, b: Type): void {
  [a, b] = [b, a]
}

// 除此之外，type，interface都可以使用泛型
type A<Type> = string | number | Type
let a: A<boolean> = true

// interface
interface Person<Type, Upe> {
  msg: Type
  data: Upe
}

let person: Person<string, number> = {
  msg: 'aaa',
  data: 123
}

// 常见的多参数泛型和默认泛型都是老生常谈了
function add<T = number, U = number>(a: T, b: U) {
  return [a, b]
}

// 接下来写一些例子
const axios = {
  get<T>(url: string): Promise<T> {
    return new Promise((resolve, reject) => {
      let xhr: XMLHttpRequest = new XMLHttpRequest()
      xhr.open('GET', url)
      xhr.onreadystatechange = () => {
        if (xhr.readyState == 4 && xhr.status == 200) {
          resolve(JSON.parse(xhr.responseText))
        }
      }
      xhr.send(null)
    })
  }
}

interface Data {
  msg: string
  code: number
}

// 当我们指定泛型时，axios.get<Data>会将Data作为T的类型，这样就会有代码提示了
axios.get<Data>('').then(res => {
  console.log(res)
})