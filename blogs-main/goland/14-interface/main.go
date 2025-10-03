package main

import (
	"fmt"
)

type Usb interface {
	Start()
	Stop(str string)
}

type Phone struct {
	Name string
}

func (p Phone) Start() {
	fmt.Printf("i am %s, start\n", p.Name)
}
func (p Phone) Stop(str string) {
	fmt.Printf("i am %s, stop, message is %s\n", p.Name, str)
}

type Computer struct {
}

func (c Computer) printInfo(u Usb) {
	u.Start()
	u.Stop("ssss")
}

func interfaceUse() {
	var p Usb = Phone{
		Name: "aaaa",
	}
	p.Start()
	p.Stop("hai-hai-hai")
}

func methodInterface() {
	c := Computer{}
	var p Usb = Phone{
		Name: "aaaa",
	}
	c.printInfo(p)
}

func emptyInterface() {
	var x interface{}
	s := "Hello, World"
	x = s
	fmt.Printf("%T, %v\n", x, x)
}

func assert() {
	var x any = 12
	v, ok := x.(int)
	println(v, ok)
}

func justifyType(x any) {
	switch x.(type) {
	case string:
		println("string")
	default:
		println("non")
	}
}

func main() {
	justifyType("s")
}
