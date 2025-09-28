package main

import "fmt"

type Animal struct {
	name string
}

func (a Animal) run() {
	fmt.Println("i can run")
}

type Dog struct {
	age int32
	Animal
}

func (d Dog) printInfo() {
	fmt.Println(d.name, d.age)
	d.run()
}

func main() {
	dog := Dog{
		age: 20,
		Animal: Animal{
			name: "dog",
		},
	}
	dog.printInfo()
}
