package main

func foo (x int, y float64) int {
	return x + float2int(y)
}

func main() {
	a := 1+2*6/(5+2-1)*2
	b := 3*a-5
	c, _ := inputf()
	d := int2float(a+b)

	if a > b {
		b = a
	} else if c >= d {
		print(foo(a*b, c+d), " ", c, "\n")
	}

	b = 5+4*a*b/2/a+a 
	print(a, " ", b, "\n")
}