package main

import (
	"fmt"//TODO remove after testing
	"math"
	"time"
)

type inter interface {
	Function(int) int
}

type myStruct struct {
	i int
	f float64
	neighbors map[string]*uint
	comID uint
}

func co() {
	fmt.Println("Go")
}

func co2(c chan float64) {
	c <- 13.37
	close(c)
}

func main() {
	defer fmt.Println("Hello KDE!")
	value := math.Log2(256)
	fmt.Printf("%.1f\n", value)
	var mybool bool
	mybool = true
	if mybool {
		mybool = false
	} else {
		panic("No!")//NOTICE
	}
	go co()
	m:= make(map[string]string)
	m["1"] = "2"/*Multi
	liner*/
	for k, v := range m {
		m[v] = k
	}
	switch m["2"] {
		case "1":
			mybool = true
		default:
			return
	}
	ch := make(chan float64)
	go co2(ch)
	time.Sleep(100*time.Millisecond)
	select {
		case <-ch:
			fmt.Println("DONE")
		default:
			return
	}
}

var s string = "A string with \x60 a horse \140 \u2014 \U0001F40E"
var s string = "A string with \x6 a horse \400 \u201 \U0001F40"
var s string = "A string with \x6x a horse \400x \u201x \U0001F40x"
var c float32 = imag(987i) + 0o_7_7_7 + 0x1.fp-2 + 0b101010 01 0o21 0x1
var c float32 = imag(0x_987i) + 0o_7_7_7i + 0x1.fp-2i 0b101010 023__
type R[T any, C comparable] struct { a T; b C }
