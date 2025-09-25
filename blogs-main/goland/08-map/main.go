package main

import "fmt"

func main() {
	var testMap = map[string]bool{
		"a": true,
		"b": false,
	}
	fmt.Println(testMap)

	scoreMap := make(map[string]int32, 8)
	scoreMap["aaaa"] = 10
	scoreMap["bbbb"] = 10

	var value, ok = scoreMap["aaaa"]
	fmt.Println(value, ok)

	delete(scoreMap, "aaaa")
	fmt.Println(scoreMap)

	var mapSlice = make([]map[string]int32, 8)
	mapSlice[0] = make(map[string]int32, 2)
	mapSlice[0]["name"] = 1
	mapSlice[0]["age"] = 2
	for index, value := range mapSlice {
		fmt.Println(index, value)
	}

	for _, maps := range mapSlice {
		for key, val := range maps {
			println(key, val)
		}
	}

	var sliceMap = make(map[string][]int32, 2)
	sliceMap["hello"] = make([]int32, 10)
	sliceMap["hello"][0] = 11
	fmt.Println(sliceMap)
}
