package calc

import "fmt"

var a int32 = 100
var B int32 = 200

func init() {
	fmt.Println(a)
}

func Add(x, y int32) (result int32) {
	result = x + y
	return
}
