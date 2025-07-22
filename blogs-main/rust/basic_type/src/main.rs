fn wrap_use() {
  let num: u8 = 255;
  let num2 = num.wrapping_add(10);
  println!("{}", num2);
}

fn main() {
  wrap_use();
}
