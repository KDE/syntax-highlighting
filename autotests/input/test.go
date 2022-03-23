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
