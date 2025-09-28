package main

import "fmt"

type person struct {
	name string
	age  int32
	sex  string
}

func (p person) printInfo() {
	fmt.Printf("name: %s, age: %d, sex: %s\n", p.name, p.age, p.sex)
}

type unname_struct struct {
	string
	int64
}

type Address struct {
	Province string
}

type User struct {
	Name string
	Address
}

type Animal struct {
	name string
}

func (a *Animal) run() {
	fmt.Println("i can run")
}

type Dog struct {
	age int32
	Animal
}

func (d Dog) printInfo() {
	d.run()
	fmt.Println("-------")
}

type myInt int32

func (i myInt) sayHello() {
	println("Hello, i am a int")
}

func main() {
	type myInt1 = int32
	var a myInt1 = 10

	type myInt2 int32
	var b myInt2 = 12

	fmt.Printf("%T, %T\n", a, b)

	var person1 person
	person1.name = "chu_lan"
	person1.age = 20
	person1.sex = "man"
	fmt.Println(person1.name, person1.age, person1.sex)

	var person2 = new(person)
	person2.name = "chu_lan"
	person2.age = 20
	person2.sex = "man"
	fmt.Println(person2.name, person2.age, person2.sex)

	var person3 = person{
		name: "chu_lan",
		age:  20,
		sex:  "man",
	}
	fmt.Println(person3.name, person3.age, person3.sex)

	var person4 = person{"chu_lan", 20, "man"}
	fmt.Println(person4.name, person4.age, person4.sex)

	person1.printInfo()

	var c myInt = 10
	c.sayHello()

	var unname = unname_struct{
		"aaa", 12,
	}
	fmt.Printf("%s, %d\n", unname.string, unname.int64)

	var dog Dog
	dog.printInfo()
}
