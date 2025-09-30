package main

import (
	"encoding/json"
	"fmt"
	"time"
)

type Student struct {
	ID     int32
	Gender string
	name   string // 你会发现私有属性没有被序列化
	Sno    string
}

type Student2 struct {
	ID     int32
	Gender string
	Name   string
	Sno    string
}

type Student3 struct {
	ID     int32  `json:"id"`
	Gender string `json:"gender"`
	Name   string
	Sno    string
}

type Student4 struct {
	ID     int32  `json:"id"`
	Name   string `json:"name"`
	Gender string
	Sno    string
}

type Classes struct {
	ClassID  int32      `json:"classID"`
	Students []Student2 `json:"students"`
}

func structToJson() {
	var s1 = Student{1, "man", "aaa", "s1001"}
	fmt.Printf("%#v\n", s1)
	var str, err = json.Marshal(s1)
	if err != nil {
		panic("error in marshal")
	}
	fmt.Printf("%s\n", string(str))
}

func jsonToStruct() {
	var jsonStr string = `{"ID":1,"Gender":"男","Name":"李四","Sno":"s0001"}`
	var student Student2
	err := json.Unmarshal([]byte(jsonStr), &student)
	if err != nil {
		panic("error in unmarshal")
	}
	fmt.Println(student.ID, student.Gender, student.Name, student.Sno)
}

func reflectBase() {
	// 有标签的被序列化为我们指定的标签字段，可用于前后端对接等操作
	var s1 = Student3{1, "man", "bbb", "s00001"}
	fmt.Printf("%#v\n", s1)

	jsonStr, err := json.Marshal(s1)
	if err == nil {
		fmt.Printf("%s\n", string(jsonStr))
	}
}

func reflectBase2() {
	var jsonStr string = `{"id":1,"Gender":"男","name":"李四","Sno":"s0001"}`
	var student Student4
	err := json.Unmarshal([]byte(jsonStr), &student)
	if err == nil {
		fmt.Println(student.ID, student.Gender, student.Name, student.Sno)
	}
}

func deepStruct() {
	var class = Classes{
		ClassID:  1,
		Students: make([]Student2, 0, 200),
	}
	for i := 0; i < 20; i++ {
		stu := Student2{
			ID:     int32(i),
			Gender: "man",
			Name:   fmt.Sprintf("iam%d", i),
			Sno:    fmt.Sprintf("sno%d", i),
		}
		class.Students = append(class.Students, stu)
	}

	var data, err = json.Marshal(class)
	if err == nil {
		fmt.Printf("%s\n", string(data))
	}
}

type Time time.Time

func (t Time) MarshalJSON() ([]byte, error) {
	stamp := fmt.Sprintf(`"%s"`, time.Time(t).Format("2006-01-02"))
	return []byte(stamp), nil
}

func (t *Time) UnmarshalJSON(data []byte) error {
	str := string(data[1 : len(data)-1]) // 去掉引号
	parsed, err := time.Parse("2006-01-02", str)
	if err != nil {
		return err
	}
	*t = Time(parsed)
	return nil
}

type Person struct {
	Name     string `json:"name"`
	Birthday Time   `json:"birthday"`
}

func customTime() {
	now := time.Now()
	person := Person{
		Name:     "sss",
		Birthday: Time(now),
	}
	data, err := json.Marshal(person)
	if err == nil {
		fmt.Println(string(data))
	}
}

func main() {
	customTime()
}
