package main

import "fmt"

func Max[T int32 | float64](a, b T) (result T) {
	if a > b {
		return a
	}
	return b
}

type Stack[T any] struct {
	items []T
}

func (s *Stack[T]) Push(item T) {
	s.items = append(s.items, item)
}

func (s *Stack[T]) Pop() (T, bool) {
	if len(s.items) == 0 {
		var zero T
		return zero, false
	}
	item := s.items[len(s.items)-1]
	s.items = s.items[:len(s.items)-1]
	return item, true
}

type Pair[K comparable, V any] struct {
	Key   K
	Value V
}

func NewPair[K comparable, V any](key K, value V) Pair[K, V] {
	return Pair[K, V]{Key: key, Value: value}
}

func main() {
	p1 := NewPair("name", "张三")
	fmt.Println(p1)
}
