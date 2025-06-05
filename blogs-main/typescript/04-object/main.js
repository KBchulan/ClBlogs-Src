// 加餐环节：上文多次提到了原型链，这里简单介绍一下
/**
 * Prototype 原型 | 原型对象
 *  1. Prototype是【函数】的一个属性
 *  2. Prototype是一个对象
 *  3. 当我们创建一个函数时，会默认加上Prototype这个属性
 *
 * __proto__ 隐式原型
 *  1. 【对象】的属性
 *  2. 指向构造函数的prototype
 *
 * 顶层：Object.prototype.__proto__ === null
 */

// 我们可以看到fn函数有一个prototype属性，这个属性是一个对象，这个对象就是fn函数的原型对象。
function fn() { }
console.dir(fn)

// 对象存在__proto__，指向构造函数的prototype，而这个原型对象也有__proto__，指向构造函数的原型对象，这样是不是就形成了链
const obj = new fn()
console.log(obj.__proto__ === fn.prototype) // true
console.log(obj.__proto__.__proto__ === Object.prototype) // true
console.log(obj.__proto__.__proto__.__proto__ === null) // true

// 查找时会顺着链条去找
obj.a = 5
fn.prototype.b = 10
console.log(obj.a) // 5
console.log(obj.b) // 10

/**
 * 差不多就是这意思
 * obj = {
 *  a: 5,
 *  __proto__: fn.prototype = {
 *    b: 10,
 *    __proto__: Object.prototype = {
 *      __proto__: null
 *    }
 *  }
 * }
 */