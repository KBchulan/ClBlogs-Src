package main

import (
	"fmt"
	c "modLearn/calc"

	"github.com/shopspring/decimal"
)

func init() {
	fmt.Println(c.Sub(1, 2))
}

func init() {
	fmt.Println("second-------")
}

func main() {
	fmt.Println(c.Add(c.B, 10))
	a := decimal.NewFromFloat(0.1)
	fmt.Println(a.String())
}
