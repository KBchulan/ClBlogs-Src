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

fn borrow_use_2() {
    let mut str = String::from("hello");
    change(&mut str);
    println!("str is: {}", str);
}

fn change(str: &mut String) {
    str.push_str(", world!");
}

fn borrow_use_3() {
    let mut str = String::from("hello");

    let s1 = &str;
    let s3: &mut String = &mut str;
    let s2 = &str;

    // 加上下面这一行会报错
    // println!("s1 is: {}, s2 is: {}, s3 is: {}", s1, s2, s3);
}

// fn dange() -> &String {
//     let str = String::from("hello");
//     &str
// }

fn main() {
    // string_use();
    // bind_use_1();
    // bind_use_2();
    // bind_use_3();
    borrow_use_2();
}
