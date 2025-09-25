package main

import (
	"fmt"
	"math"
	"strconv"
	"strings"
	"unsafe"
)

func basic_use() {
	// 基本用法
	var a int64 = 222_333
	fmt.Printf("value is %d, type is %T, size is %d\n", a, a, unsafe.Sizeof(a))

	// 类型转换
	b := int32(a)
	fmt.Println(b)

	// 普通浮点数
	fmt.Printf("%f\n", math.Pi)

	// 精度丢失
	c := 1129.6
	fmt.Println((c * 100))

	// 科学计数法
	num1 := 5.1234e2
	num2 := 5.1234e-2
	fmt.Printf("%f, %f\n", num1, num2)

	// 字符串
	s11 := "Hello"
	s12 := "World"
	fmt.Println(s11)

	// 长度
	fmt.Printf("len is %d\n", len(s11))

	// 拼接
	fmt.Println(s11 + s12)

	// 分割
	var str string = "123-456-789"
	fmt.Println(strings.Split(str, "-"))

	s2 := `sss
aaa`

	fmt.Println(s2)
	fmt.Println("str := \"c:\\Code\\demo\\go.exe\"")

	s3 := "Hello, 张三"
	for i := 0; i < len(s3); i++ {
		fmt.Printf("%v(%c)", s3[i], s3[i])
	}
	fmt.Println()

	for _, r := range s3 {
		fmt.Printf("%v(%c)", r, r)
	}
	fmt.Println()

	s4 := "big"
	byteS1 := []byte(s4)
	byteS1[0] = 'a'
	fmt.Println(string(byteS1))
}

func type_tran() {
	var a int8 = 10
	var b int16 = 20
	var c int16 = int16(a) + b
	fmt.Printf("%d, %T\n", c, c)

	s1 := strconv.Itoa(20)
	fmt.Printf("%s, %T\n", s1, s1)

	var d float64 = 20.113123
	s2 := strconv.FormatFloat(d, 'e', 2, 64)
	fmt.Printf("%s, %T\n", s2, s2)

	s3 := strconv.FormatBool(true)
	fmt.Printf("%s, %T\n", s3, s3)

	var e int64 = 10
	s4 := strconv.FormatInt(e, 10)
	fmt.Printf("%s, %T\n", s4, s4)

	var ss1 string = "1234"
	i64, _ := strconv.ParseInt(ss1, 10, 64)
	fmt.Println(i64)

	for _, v := range ss1 {
		fmt.Printf("%c ", v)
	}
	fmt.Println()
}

func main() {
	var i64 int64 = 2
	fmt.Println(i64 << 1)
}
