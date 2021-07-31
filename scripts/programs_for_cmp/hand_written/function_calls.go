package main
func main() {

	print("call is: ",abs(1), " ", abs(0), " ", abs(-1), "\n")
	print("call should be: 1 0 1\n")
	a, b := swap(swap(5, 10))
	print("swap is: ", a, " ", b, "\n")
	print("swap should be: 5 10\n")
	print("max1: ", max(20, 30, 40), "\n")
	print("max1 should be 40\n")
	print("max2: ", max(max(20, 30, 40), max(split_to_3(max(split_to_3(903)))), max(5, 25, 15)), "\n")
	print("max2 should be: 101", "\n" )
	x := 1
	y:=0
	z:= 5
	x, y, z = split_to_3(999)
	print("split: ", x,y,z,"\n")
	print("split should be: 333333333\n")
}

func swap(x int, y int) (y_swaped int, x_swaped int) {
	x_swaped, y_swaped = x, y
	return
}

func abs(x int) int {
	if x > 0{
		return x
	} else {
		return -x
	}
}

func max (a int, b int, c int) int {
	if (a >= b && a >= c) {
		return a
	} else if b >= a && b >= c {
		return b
	} else if c>=a&&c>=b{
		return c
	}
	return -1
}

func split_to_3(x int) (int, int, int) {
	y := x/3
	return y, x-2* y, y
}

