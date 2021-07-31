package main
func main() {

	x := 1
	print("long equation result: ", x+(x+(x+(x+(x+(x+(x+(x+(x+(x+(x+(x+(x+(x+(x+(x+(x+x)*2)))))))))*2))))))*2, "\n")
	print("result should be 65\n")
	a := 3*identity(negation(20)) + 20
	print("result should be -40\n")
	print("result is: ", a, "\n")
	b := pow(3, 3)
	print("3^3 = ", b, " 2^10 = ", pow(2,  10), "\n")
	c := sqrt(int2float(16), 0.00001)
	print("sqrt(16) = (float)", c, " (int)", float2int(c), "\n")
	print("sqrt(85246.0) = ", float2int(sqrt(85246.0, 0.00000001)), "\n")
	d := sqrt(sqrt(81.0, 0.0001)*int2float(pow(9, 2))/9.0, 0.0001)
	print("result should be 9\n")
	print("result is: (float)", d, " (int)", float2int(d), "\n")
	x = abs(-b)*2+5-3*4+2*identity(7)*2*2/3-11+a
	print("result should be 14\n")
	print("result is: ", x, "\n")
	print("result is: ", 26+71/pow(4, 3) - negation(a) < float2int(sqrt(d*d*d, 0.00001)) +a, "\n")
	print("result should be false\n", "")

	if -(x*a) + x/a >= (x+a)/x + x+a {
		print("if\n")
	} else {
		print("else\n")
	}
}


func abs(x int) int {
	if x > 0{
		return x
	} else {
		return -x
	}
}

func negation(x int) (minus_x int) {
	minus_x = -x
	return
}

func identity(x int) (int) {
	return 2*x + 5 - 5 - x
}

func pow (base int, exponent int) int {
	res := base
	if (exponent == 0) {
		return 1
	}

	for i := exponent; i > 1; i -=1{
		res *= base
	}
	return res
}

func sqrt(num float64, eps float64) (float64) {
	num1 := num / (0.1*num)
	for ; num1*num1 + eps < num || num1*num1 - eps > num; {
		num1 = (num1 + num/num1)/2.0
	}
	return num1
}
