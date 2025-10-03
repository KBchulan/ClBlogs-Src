package main

import (
	"fmt"
	"runtime"
	"strconv"
	"sync"
	"time"
)

var wg sync.WaitGroup

func func1() {
	test := func() {
		defer wg.Done()
		for i := 0; i <= 10; i++ {
			fmt.Println("test: hello, world", strconv.FormatInt(int64(i), 10))
			time.Sleep(time.Second)
		}
	}

	wg.Add(1)
	go test()
	for i := 0; i <= 10; i++ {
		fmt.Println("main: hello, world", strconv.FormatInt(int64(i), 10))
		time.Sleep(time.Second)
	}

	wg.Wait()
}

func func2() {
	hello := func(i int32) {
		defer wg.Done()
		fmt.Println("Hello Goroutine", i)
	}

	for i := range 10 {
		wg.Add(1)
		go hello(int32(i))
	}

	wg.Wait()
}

func count1() {
	start := time.Now().UnixMilli()
	for num := range 120000 {
		flag := true
		for i := 2; i < num; i++ {
			if num%i == 0 {
				flag = false
				break
			}
		}
		if flag {
			// fmt.Print(num, " ")
		}
	}
	end := time.Now().UnixMilli()
	fmt.Println(end - start)
}

func count2() {
	cpuNum := runtime.NumCPU()
	countsEach := 120000 / cpuNum

	fn := func(i int32) {
		defer wg.Done()
		for num := (int(i) - 1) * countsEach; num < int(i)*countsEach; num++ {
			flag := true
			for j := 2; j < int(num); j++ {
				if num%j == 0 {
					flag = false
					break
				}
			}
			if flag {
				// fmt.Print(num, " ")
			}
		}
	}

	start := time.Now().UnixMilli()
	for i := 1; i <= cpuNum; i++ {
		wg.Add(1)
		go fn(int32(i))
	}
	wg.Wait()
	end := time.Now().UnixMilli()
	fmt.Println(end - start)
}

func func3() {
	ch := make(chan int, 10)
	ch <- 11
	x := <-ch
	close(ch)
	println(x)
}

func func4() {
	var ch1 chan int = make(chan int, 5)
	for i := range 5 {
		ch1 <- i + 1
	}

	close(ch1)
	for val := range ch1 {
		println(val)
	}
}

func func5() {
	inChan := make(chan int, 100)
	wGroup := sync.WaitGroup{}

	input := func() {
		defer wGroup.Done()

		count := 0
		for count < 100 {
			inChan <- count
			count++
		}
		close(inChan)
	}

	output := func() {
		defer wGroup.Done()

		count := 0
		for count < 100 {
			val := <-inChan
			println(val)
			count++
		}
	}

	wGroup.Add(2)
	go input()
	go output()
	fmt.Println("Begin-------")

	wGroup.Wait()
}

func func6() {
	resultChan := make(chan int, 1000)
	consumeWg := sync.WaitGroup{}
	processWg := sync.WaitGroup{}
	eachCount := 120000 / runtime.NumCPU()

	isPrime := func(num int) bool {
		if num <= 1 {
			return false
		}
		if num <= 3 {
			return true
		}
		for i := 2; i < num; i++ {
			if num%i == 0 {
				return false
			}
		}
		return true
	}

	in := func(n int) {
		defer processWg.Done()
		for i := (n - 1) * eachCount; i < n*eachCount; i++ {
			if isPrime(i) {
				resultChan <- i
			}
		}
	}

	printResult := func() {
		defer consumeWg.Done()
		for v := range resultChan {
			print(v, " ")
		}
	}

	for i := range runtime.NumCPU() {
		processWg.Add(1)
		go in(i + 1)
	}

	consumeWg.Add(1)
	go printResult()

	processWg.Wait()
	close(resultChan)
	consumeWg.Wait()
}

func func7() {
	// 双向管道
	var chan1 chan int = make(chan int, 5)

	// 只写
	var chan2 chan<- int = chan1
	chan2 <- 10

	// 只读
	var chan3 <-chan int = chan1
	println(<-chan3)
}

func func8() {
	var intChan = make(chan int, 10)
	for v := range 10 {
		intChan <- v
	}

	var stringChan = make(chan string, 5)
	for i := range 5 {
		stringChan <- strconv.Itoa(i) + "hhh"
	}

	for {
		select {
		case v := <-intChan:
			fmt.Printf("from int chan: %d\n", v)
		case v := <-stringChan:
			fmt.Printf("from int chan: %s\n", v)
		default:
			fmt.Println("no data")
			time.Sleep(time.Microsecond * 100)
			return
		}
	}
}

func main() {
	func8()
}
