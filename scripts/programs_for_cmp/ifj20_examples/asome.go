
package main

func main() {
	print("Hello world\n")
	print(f1(3), "\n")
	print(f1(4), "\n")
	print(f1(5), "\n")
	print(f1(257), "\n")
	print(f1(60), "\n")
	print(f1(25), "\n")
}

func f1 (a int) int {
	return f2(a+2)
}

func f2 (b int) int {
	if b > 5 {
		return f3(2*b)
	} else {
		return f4(2*b)
	}
}

func f3(c int) (res int ) {
	res = c -2
	return
}

func f4 (d int) (
	int) {
	if d < 256 {
		return f1(d)
	} else {
		return d - 256
	}
}
