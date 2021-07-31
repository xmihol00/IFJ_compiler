package main

func main() {

	print(int2float(len("ahoj")), "\n")
	print(float2int(int2float(len("zdar"))), "\n")
	print(int2float(len(str())), "\n")
	print(float2int(int2float(len(str()))), "\n")	
	x := float2int(8.35)
	y := int2float(x)
	z := x + float2int(y)
	print (x, " ", y, " ", z, "\n")
	a := float2int(flt(5+x*98))
	print(a, "\n")
	a = float2int(2.2*flt(5+3*x*z+a-15)+15.3*y)
	b := float2int(int2float(a*float2int(flt(x*a+6))))
	print(b, "\n")
	print(len(str()), "\n")
	c := len(str())
	print(c, "\n")
}

func str() string {
	return "test string"
}

func flt(i int) float64 {
	return int2float(i+78-2)
}
