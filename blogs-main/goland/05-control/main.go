package main

import "fmt"

func ifDemo() {
	score := 65
	if score > 90 {
		println(666)
	} else if score > 70 {
		println("just so so")
	} else {
		println("too low")
	}
}

func forDemo() {
	for i := 0; i < 10; i++ {
		print(i, " ")
	}
	println()

	i := 10
	for i < 20 {
		print(i, " ")
		i++
	}
	println()

	for {
		if i >= 30 {
			break
		}
		print(i, " ")
		i++
	}

	println()

	str := "hello, world"
	for index, val := range str {
		fmt.Printf("%d, %c\n", index, val)
	}

	println()
}

func switchDemo() {
	extern_name := ".a"
	switch extern_name {
	case ".html":
		println("html")
	case ".so":
		println("so")
	default:
		println("no")
	}

	switch n := 2; n {
	case 1, 3, 5, 7, 9:
		println("奇数")
	case 2, 4, 6, 8, 10:
		println("偶数")
	default:
		println(n)
	}

	switch age := 65; {
	case age < 25:
		fmt.Println("好好学习吧！")
	case age > 25 && age <= 60:
		fmt.Println("好好工作吧！")
	case age > 60:
		fmt.Println("好好享受吧！")
	default:
		fmt.Println("活着真好！")
	}

	str := "a"
	switch str {
	case "a":
		println("a")
		fallthrough
	case "b":
		println("b")
		fallthrough
	default:
		println("c")
	}
}

func breadDemo() {
label:
	for i := range 2 {
		for j := range 10 {
			if j == 2 {
				break label
			}
			println(i, j)
		}
		println("-----")
	}
}

func continueDemo() {
here:
	for i := range 2 {
		for j := range 10 {
			if j == 2 {
				continue here
			}
			println(i, j)
		}
	}
}

func main() {
	// ifDemo()
	// forDemo()
	// switchDemo()
	breadDemo()
	// continueDemo()
}
