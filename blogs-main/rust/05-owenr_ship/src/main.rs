fn string_use() {
    let mut str = String::from("hello");
    str.push_str(", world!");
    println!("str is: {}", str);
}

fn bind_use_1() {
    let x: i8 = 10;
    let y: i8 = x;
    println!("x is: {}, y is: {}", x, y);
}

fn bind_use_2() {
    let x: String = String::from("hello");
    let y: String = x;
    println!("x is: {}", y);
}

fn bind_use_3() {
    let x: String = String::from("hello");
    let y: String = x.clone();
    println!("x is: {}, y is: {}", x, y);
}

fn borrow_use() {
    let x: i32 = 10;
    let y: &i32 = &x;
    assert_eq!(x, 10);
    assert_eq!(*y, 10);
}

fn main() {
    // string_use();
    // bind_use_1();
    // bind_use_2();
    // bind_use_3();
    borrow_use();
}
