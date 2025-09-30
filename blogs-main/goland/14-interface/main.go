package main

import "fmt"

type Usb_er interface {
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

func interfaceUse() {
	var p Usb_er = Phone{
		Name: "aaaa",
	}
	p.Start()
	p.Stop("hai-hai-hai")
}

func main() {
	var x any
	s := "你好 golang"
	x = s
	fmt.Printf("type:%T value:%v\n", x, x)
}
