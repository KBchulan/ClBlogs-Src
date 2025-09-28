package main

import (
	"fmt"
	"time"
)

func basic_time() {
	var now = time.Now()
	fmt.Printf("current time is: %v\n", now)

	year := now.Year()
	mouth := now.Month()
	day := now.Day()
	hour := now.Hour()
	minute := now.Minute()
	second := now.Second()
	fmt.Printf("%d-%02d-%02d %02d:%02d:%02d\n", year, mouth, day, hour, minute, second)
}

func format_use() {
	now := time.Now()

	// 24 小时制
	fmt.Println(now.Format("2006-01-02 15:04:05"))

	// 12 小时制
	fmt.Println(now.Format("2006-01-02 03:04:05"))
}

func time_stamp() {
	now := time.Now()
	timestamp1 := now.Unix()
	timestamp2 := now.UnixNano()
	fmt.Printf("timestamp1: %v\n", timestamp1)
	fmt.Printf("timestamp2: %v\n", timestamp2)
}

func unixToTime(timestamp int64) {
	timeObj := time.Unix(timestamp, 0)
	year := timeObj.Year()
	mouth := timeObj.Month()
	day := timeObj.Day()
	hour := timeObj.Hour()
	minute := timeObj.Minute()
	second := timeObj.Second()
	fmt.Printf("%d-%02d-%02d %02d:%02d:%02d\n", year, mouth, day, hour, minute, second)

	// 或者
	fmt.Println(timeObj.Format("2006-01-02 15:04:05"))
}

func timeStrToStamp(t1 string) {
	timeTemplate := "2006-01-02 15:04:05"
	stamp, err := time.ParseInLocation(timeTemplate, t1, time.Local)
	if err == nil {
		fmt.Println(stamp.Unix())
	}
}

func ticker() {
	ticker := time.NewTicker(time.Second)

	n := 0
	for v := range ticker.C {
		fmt.Println(v)
		n++
		if n > 5 {
			ticker.Stop()
			return
		}
	}
}

func main() {
	// basic_time()
	// format_use()
	// time_stamp()
	// unixToTime(1587880013)
	// timeStrToStamp("2019-01-08 13:50:30")
	ticker()
}
