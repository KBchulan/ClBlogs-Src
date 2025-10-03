package main

import (
	"bufio"
	"fmt"
	"io"
	"os"
)

func func1() {
	file, err := os.Open("./test.json")
	if err != nil {
		fmt.Println("Open file error, error is:", err)
		return
	}

	defer file.Close()

	var content []byte
	var tmp = make([]byte, 128)
	for {
		n, err := file.Read(tmp)
		if err == io.EOF {
			fmt.Println("file end")
			break
		}
		if err != nil {
			fmt.Println("Read file error, error is:", err)
			return
		}
		content = append(content, tmp[:n]...)
	}
	fmt.Println(string(content))
}

func func2() {
	file, err := os.Open("./test.txt")
	if err != nil {
		fmt.Println("Open file error, error is:", err)
		return
	}
	defer file.Close()

	reader := bufio.NewReader(file)
	for {
		line, err := reader.ReadString('\n')
		if err == io.EOF {
			if len(line) != 0 {
				fmt.Println(line)
				break
			}
		}
		if err != nil {
			fmt.Println("Read file error, error is:", err)
			return
		}
		fmt.Print(line)
	}
}

func func3() {
	file, err := os.OpenFile("./writeTest.txt", os.O_CREATE|os.O_WRONLY, 0666)
	if err != nil {

	}
	defer file.Close()
	str := "hello, golang"
	file.Write([]byte(str))
	file.WriteString("iam")
}

func main() {
	func3()
}
