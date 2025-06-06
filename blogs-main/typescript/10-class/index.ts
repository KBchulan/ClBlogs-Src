// 本节介绍一下类和抽象类

// 类：
// 1.基本用法，继承和类型约束(implements用于约束,extends用于继承)
// 2.修饰符号readonly, private, public, protected(后面这三个和c++的基本一致,默认级别不一样罢了)
// 3.super是指向父类的函数使用，原理：父类的prototype.constructor.call
// 4.静态(与c++也是一样的)
// 5.get，set


// 基础部分不介绍了，给出一个例子如下：
interface Options {
  el: string | HTMLElement
}

interface VueClass {
  options: Options
  init(): void
}

interface VNode {
  tag: string
  text?: string
  children?: VNode[]
}

class Dom {
  createElement(el: string) {
    return document.createElement(el)
  }

  setText(el: HTMLElement, text: string) {
    el.textContent = text
  }

  render(data: VNode) {
    let root: HTMLElement = this.createElement(data.tag)
    if (data.children && Array.isArray(data.children)) {
      data.children.forEach(item => {
        root.appendChild(this.render(item))
      })
    }
    else {
      this.setText(root, data.text!)
    }
    return root
  }
}

class Vue extends Dom implements VueClass {
  options: Options

  constructor(options: Options) {
    super()
    this.options = options
    this.init()
  }

  init(): void {
    let data: VNode = {
      tag: 'div',
      children: [
        {
          tag: 'h1',
          text: 'Hello, World!'
        },
        {
          tag: 'p',
          text: 'This is a paragraph.'
        }
      ]
    }

    let app: HTMLElement = typeof this.options.el === 'string' ? document.querySelector(this.options.el) as HTMLElement : this.options.el
    app.appendChild(this.render(data))
  }
}


new Vue({
  el: '#app'
})


// 抽象类：
// 与c++中带有纯虚函数的类一个道理，不过ts有显式的语法
// abstruct 修饰类则为抽象类，无法实例化
// abstruct 修饰方法则是抽象方法，不能实现，且子类必须实现

abstract class Vuee {
  name: string

  constructor(name?: string) {
    this.name = name!
  }

  abstract abFunc(): void // 该方法不能实现
}

// let vue = new Vue() // 错误，抽象类不能实例化

// 派生类必须实现抽象方法，和纯虚函数一个道理
class React extends Vuee {
  constructor(name: string) {
    super(name)
  }

  abFunc(): void { }
}

let react: React = new React('chulan')
console.log(react.name)