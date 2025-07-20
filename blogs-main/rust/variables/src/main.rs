fn error_use_var() {
    let x = 5;
    println!("The value of x is: {}", x);
    x = 10;
    println!("The value of x is now: {}", x);
}

fn safe_use_var() {
    let mut x = 5;
    println!("The value of x is: {}", x);
    x = 10;
    println!("The value of x is now: {}", x);
}

fn unuse_var() {
    let _x = 5;
    let y = 10;
}

fn deconstruct() {
    let (a, mut b): (bool, bool) = (true, false);
    println!("a: {}, b: {}", a, b);

    b = true;
    assert_eq!(a, b);
}

struct MyStruct {
    e: i32
}

fn test() {
    let (a, b, c, e, d);
    (a, b) = (1, 2);
    [c, .., d, _] = [3, 4, 5, 6, 7];
    MyStruct {e, ..} = MyStruct {e: 30};
    println!("a: {}, b: {}, c: {}, d: {}, e: {}", a, b, c, d, e);
}

fn const_use() {
    const MY_CONST: i32 = 10;
    println!("The value of MY_CONST is: {}", MY_CONST);
}

#[allow(unused_variables)]
fn shallow_use() {
    let x = 5;
    let x = x + 1;

    {
        let x = x * 2;
        println!("the value of x in inner scope is: {}", x);
    }
    println!("the value of x is: {}", x);
}

fn main() {
    error_use_var();
    safe_use_var();
    unuse_var();
    deconstruct();
    test();
    const_use();
    shallow_use();
}
