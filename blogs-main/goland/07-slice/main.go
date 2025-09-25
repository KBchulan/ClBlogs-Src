package main

import (
	"fmt"
)

func main() {
	var string_slice []string
	println(string_slice == nil)

	var a = []int32{1, 2, 3, 4}
	for i := 0; i < len(a); i++ {
		print(a[i], " ")
	}

	println()
	for _, v := range a {
		print(v, " ")
	}
	println()

	b := a[:2]
	fmt.Println(b)

	c := b[1:4]
	fmt.Println(c)

	d := make([]int32, 2, 10)
	d = append(d, 1)
	println(len(d))
	println(cap(d))

	s1 := make([]int32, 10)
	s2 := s1
	s2[0] = 10
	println(s1[0])

	s2Copy := make([]int32, len(s2))
	copy(s2Copy, s2)
	s2Copy[0] = 100
	println(s2Copy[0])

	aa := []int32{1, 2, 4, 56}
	aa = append(aa[:2], aa[3:]...)
}
