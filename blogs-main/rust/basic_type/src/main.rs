fn overflow_use() {
  let mut num: u8 = 255;
  num += 10;
  println!("overflow_use: {}", num);
}

fn wrap_use() {
  let num: u8 = 255;
  let num2 = num.wrapping_add(10);
  println!("wrap_use: {}", num2);
}

fn float_use() {
  let x = 1.0;
  let y: f32 = 2.0;
  let z = x + y as f64;
  println!("float_use: {}", z);
}

fn float_use_error() {
  let x = 0.1f64 + 0.2f64;
  println!("float_use_error: {}", x);
  assert_eq!(0.1 + 0.2, 0.3);
}

fn float_use_error_else() {
  let x: f32 = 0.1 + 0.2;
  let y: f32 = 0.3;
  assert_eq!(x, y)
}

fn nan_use() {
  let x = (-42.0f32).sqrt();
  println!("nan_use: {}", x);

  if x.is_nan() {
    println!("x is nan");
  }
}

#[allow(unused_variables)]
fn operate_use() {
  // 加法
  let sum = 5 + 10;

  // 减法
  let difference = 95.5 - 4.3;

  // 乘法
  let product = 4 * 30;

  // 除法
  let quotient = 56.7 / 32.2;

  // 求余
  let remainder = 43 % 5;
}

fn use_combale() {
  // 编译器会进行自动推导，给予twenty i32的类型
  let twenty = 20;
  // 类型标注
  let twenty_one: i32 = 21;
  // 通过类型后缀的方式进行类型标注：22是i32类型
  let twenty_two = 22i32;

  // 只有同样类型，才能运算
  let addition = twenty + twenty_one + twenty_two;
  println!("{} + {} + {} = {}", twenty, twenty_one, twenty_two, addition);

  // 对于较长的数字，可以用_进行分割，提升可读性
  let one_million: i64 = 1_000_000;
  println!("{}", one_million.pow(2));

  // 定义一个f32数组，其中42.0会自动被推导为f32类型
  let forty_twos = [
    42.0,
    42f32,
    42.0_f32,
  ];

  // 打印数组中第一个值，并控制小数位为2位
  println!("{:.2}", forty_twos[0]);
}

fn bit_operate_use() {
  let a: u8 = 2;
  let b: u8 = 3;

  println!("a & b = {}", a & b);   // 按位与
  println!("a | b = {}", a | b);   // 按位或
  println!("a ^ b = {}", a ^ b);   // 按位异或
  println!("!a = {}", !a);         // 按位非
  println!("a << 1 = {}", a << 1); // 左移
  println!("b >> 1 = {}", b >> 1); // 右移

  let mut a: u8 = a;
  a &= b;
  println!("a &= b -> a = {}", a);
}

fn ranges_use() {
  for i in 1..5 {
    println!("{}", i); // 1, 2, 3, 4
  }

  for i in 1..=5 {
    println!("{}", i); // 1, 2, 3, 4
  }

  for i in 'a'..='z' {
    println!("{}", i);
  }
}

fn num_use() {
  use num::complex::Complex;

  let a = Complex {re: 2.1, im: -1.2};
  let b = Complex::new(11.1, 22.2);
  let result = a + b;
  println!("num_use: {}", result);
}

fn express_use() {
  let y = {
    let x = 3;
    x + 1
  };
}

fn add(x: i32, y: i32) -> i32 {
  return x + y;
}

fn never_return() -> ! {
  while true {
    // 永远不会返回
    println!("This function never returns.");
  }
  panic!()
}

fn main() {
  // overflow_use();
  // wrap_use();
  // float_use();
  // float_use_error();
  // float_use_error_else();
  // nan_use();
  // bit_operate_use();
  // ranges_use();
  // num_use();
  println!("add(2, 3) = {}", add(2, 3));
}
