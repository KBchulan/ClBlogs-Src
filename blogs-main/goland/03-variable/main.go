package main

import (
	"fmt"
	"math"
)

func var_use() {
	var name string = "Golang"
	var age int = 20
	var isOk bool = true
	fmt.Printf("name: %s, age: %d, isOk: %t\n", name, age, isOk)
}

func more_var() {
	var username, sex string
	username = "chu_lan"
	sex = "male"
	fmt.Printf("username: %s, sex: %s\n", username, sex)

	var a, b, c, d int = 1, 2, 3, 4
	fmt.Printf("a: %d, b: %d, c: %d, d: %d\n", a, b, c, d)
}

func all_var() {
	var (
		a string = "Golang"
		b int    = 20
		c bool   = false
	)
	fmt.Printf("a: %s, b: %d, c: %t\n", a, b, c)
}

func init_use() {
	var a int
	var b int = 10
	fmt.Printf("a: %d, b: %d\n", a, b)
}

func type_derivation() {
	var name = ""
	fmt.Printf("The type of name: %T\n", name)
}

func short_var() {
	// 全局变量会被局部变量覆盖
	a, b := 1, 2
	fmt.Printf("a: %d, b: %d\n", a, b)
}

func unname_var() (int, string) {
	return 20, "Golang"
}

func const_var() {
	const pi float64 = math.Pi
	fmt.Printf("pi: %f\n", pi)

	// const 声明多个常量时，如果未指定类型，会自动按照上一行的匹配
	const (
		n1 int = 100
		n2
		n3
	)
	fmt.Printf("n1: %d, n2: %d, n3: %d\n", n1, n2, n3)
}

func iota_use() {
	const a int = iota
	fmt.Println("a:", a)

	const (
		b int = iota
		c
		_
		d
	)
	fmt.Printf("b: %d, c: %d, d: %d\n", b, c, d)

	const (
		n1 int = iota
		n2 int = 100
		n3 int = iota
		n4
	)
	fmt.Printf("n1: %d, n2: %d, n3: %d, n4: %d\n", n1, n2, n3, n4)

	const (
		a1, b1 = iota + 1, iota + 2
		c1, d1
		e1, f1
	)
	fmt.Printf("a1: %d, b1: %d, c1: %d, d1: %d, e1: %d, f1: %d\n", a1, b1, c1, d1, e1, f1)
}

func main() {
	// var 声明
	var_use()

	// 多个变量
	more_var()

	// 批量声明
	all_var()

	// 变量初始化
	init_use()

	// 类型推导
	type_derivation()

	// 短变量声明
	short_var()

	// 匿名变量
	_, uname := unname_var()
	fmt.Println("Unname var:", uname)

	// 常量使用
	const_var()

	// iota 使用
	iota_use()
}
