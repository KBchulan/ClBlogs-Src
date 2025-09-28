package main

import "fmt"

func sayHello() {
	fmt.Println("Hello")
}

func Sum(x int32, y int32) int32 {
	return x + y
}

func Sum2(x int32, y ...int32) int32 {
	var sum int32
	for _, v := range y {
		sum = sum + v
	}
	return sum + x
}

func calc(x int32, y int32) (sum int32, sub int32) {
	return x + y, x - y
}

type calculation func(int32, int32) int32

func SumHighLevel(x int32, y int32, op calculation) int32 {
	return op(x, y)
}

func do(s string) calculation {
	switch s {
	case "+":
		return func(i1, i2 int32) int32 { return i1 + i2 }
	case "-":
		return func(i1, i2 int32) int32 { return i1 - i2 }
	default:
		return nil
	}
}

func closure() func(int32) int32 {
	x := 10
	return func(i2 int32) int32 { return int32(x) + i2 }
}

func makeSuffixFunc(suffix string) func(string) string {
	return func(s string) string {
		return s + suffix
	}
}

func calc2(base int) (func(int) int, func(int) int) {
	add := func(i int) int {
		base += i
		return base
	}
	sub := func(i int) int {
		base -= i
		return base
	}
	return add, sub
}

func deferUse() {
	fmt.Println("start")
	defer fmt.Println(1)
	defer fmt.Println(12)
	defer fmt.Println(13)
	fmt.Println("end")
}

func f1() int {
	x := 5
	defer func() {
		x++
	}()
	return x
}

func f2() (x int) {
	defer func() {
		x++
	}()
	return 5
}

func f3() (y int) {
	x := 5
	defer func() {
		x++
	}()
	return x
}

func f4() (x int) {
	defer func(x int) {
		x++
	}(x)
	return 5
}

func calc3(index string, a, b int) int {
	ret := a + b
	fmt.Println(index, a, b, ret)
	return ret
}

func errorUse() {
	defer func() {
		err := recover()
		if err != nil {
			fmt.Println("Recover")
		}
	}()
	panic("Panic")
}

func main() {
	fmt.Println("Begin")
	errorUse()
	fmt.Println("End")
}
