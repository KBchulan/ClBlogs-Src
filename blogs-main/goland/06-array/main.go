package main

import "fmt"

func main() {
	var testArray = [3]int32{1, 2, 9}
	fmt.Println(testArray)

	var numArray = [...]int32{1, 2, 3}
	fmt.Println(numArray)

	var stringArray = [...]string{1: "aaa", 5: "bbb"}
	fmt.Println(stringArray)

	for i := 0; i < len(numArray); i++ {
		print(numArray[i], " ")
	}
	println()
	for _, v := range numArray {
		print(v, " ")
	}
	println()

	var a = [...][2]string{
		{"北京", "上海"},
		{"广州", "深圳"},
		{"成都", "重庆"},
	}
	for _, v := range a {
		for _, city := range v {
			print(city, " ")
		}
		println()
	}
}
