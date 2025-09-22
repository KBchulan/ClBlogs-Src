package main

import "fmt"

func main() {
	fmt.Println("----------------")

	var name string = "Golang"
	age := 20

	fmt.Println(name, ":", age)

	fmt.Println("----------------")

	fmt.Print("Hello")
	fmt.Print("World\n")

	fmt.Println("----------------")

	fmt.Println("Hello")
	fmt.Println("World")
	fmt.Println("姓名:", name, "年龄:", age)

	fmt.Println("----------------")

	score := 99.2
	fmt.Printf("name: %s, age: %d, score: %.2f\n", name, age, score)
	fmt.Printf("%#v\n", name)
	fmt.Printf("%T\n", name)

	fmt.Println("----------------")

	message := fmt.Sprintf("sprintf: name: %s, age: %d", name, age)
	fmt.Println(message)

	fmt.Println("----------------")

	var name2 string
	var age2 int32
	fmt.Print("Please input name: ")
	fmt.Scan(&name2)
	fmt.Print("Please input age: ")
	fmt.Scan(&age2)
	fmt.Printf("name2: %s, age2: %d\n", name2, age2)
}
