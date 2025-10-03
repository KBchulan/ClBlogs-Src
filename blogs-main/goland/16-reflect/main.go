package main

import (
	"fmt"
	"reflect"
)

func reflectType(x any) {
	v := reflect.TypeOf(x)
	fmt.Printf("typeof: %v, name: %v, kind:%v\n", v, v.Name(), v.Kind())
}

type myInt int32
type Person struct {
	Name string
	Age  myInt
}

func reflectValue(x any) {
	value := reflect.ValueOf(x)
	fmt.Println(value.Int())
}

func reflectValue2(x any) {
	value := reflect.ValueOf(x)
	kind := value.Kind()
	switch kind {
	case reflect.Int:
		fmt.Println("int")
	case reflect.Bool:
		fmt.Println("bool")
	default:
		fmt.Println("unknown type")
	}
}

func reflectSetValueError(x any) {
	v := reflect.ValueOf(x)
	if v.Kind() == reflect.Int {
		v.SetInt(10)
	}
}

func reflectSetValue(x any) {
	v := reflect.ValueOf(x)
	if v.Elem().Kind() == reflect.Int {
		v.Elem().SetInt(v.Elem().Int() + 1)
	}
}

type Student struct {
	Name  string `json:"name"`
	Age   int    `json:"age"`
	Score int    `json:"score"`
}

func (s Student) GetInfo() (result string) {
	result = fmt.Sprintf("姓名:%s, 年龄:%d, 成绩:%d", s.Name, s.Age, s.Score)
	return
}

func (s *Student) SetInfo(name string, age, score int) {
	s.Name = name
	s.Age = age
	s.Score = score
}

func PrintStructFieldByReflect(s any) {
	t := reflect.TypeOf(s)
	kind := t.Kind()

	if kind != reflect.Struct && t.Elem().Kind() != reflect.Struct {
		fmt.Println("this is no struct")
		return
	}

	field0 := t.Field(0)
	fmt.Println(field0.Name)
	fmt.Println(field0.Type)
	fmt.Println(field0.Tag.Get("json"))

	field1, _ := t.FieldByName("Age")
	fmt.Println(field1.Name)
	fmt.Println(field1.Type)
	fmt.Println(field1.Tag.Get("json"))

	num := t.NumField()
	fmt.Printf("has %d field\n", num)
}

func CallStructFuncByReflect(x any) {
	t := reflect.TypeOf(x)
	v := reflect.ValueOf(x)

	if t.Kind() != reflect.Struct && t.Elem().Kind() != reflect.Struct {
		fmt.Println("this is no struct")
		return
	}

	// 获取方法类型
	var tMethod = t.Method(0)
	fmt.Println(tMethod.Name)
	fmt.Println(tMethod.Type)

	// 获取方法数量
	fmt.Println(t.NumMethod())

	// 调用方法
	str := v.MethodByName("GetInfo").Call(nil)
	fmt.Println(str)

	// 调用有参方法
	params := make([]reflect.Value, 0, 3)
	params = append(params, reflect.ValueOf("bbb"))
	params = append(params, reflect.ValueOf(15))
	params = append(params, reflect.ValueOf(200))
	v.MethodByName("SetInfo").Call(params)

	str2 := v.MethodByName("GetInfo").Call(nil)
	fmt.Println(str2)
}

func reflectChangStruct(x any) {
	v := reflect.ValueOf(x)

	v.Elem().FieldByName("Name").SetString("bbb")
	v.Elem().FieldByName("Age").SetInt(50)
}

func main() {
	reflectType(1.2)
}
