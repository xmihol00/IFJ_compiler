package main
func main() {
	a := 4
	b := 5
	c := 6
	for i := 0; i < a; i += 1 {
		for j := b+1; j >= i; j = j -1 {
			print("@", j, "\n")
			for k := 0; k < c-i+j; k = k+1 {
				print("$")
			}
			for k := c; k < i+j; k += 1 {
				print("*")
			}
			print("\n")
        }
        for j := a; j >= 0; j = j -2 {
			print("%", j, "\n")
			for k := 0; k < j; k = k+1 {
				print("&")
			}
			for k := 0; k < i+j; k += 1 {
				print("^")
			}
			print("\n")
		}
		print("----block----\n")
    }
    print(a, " ", b, " ", c, "\n")
}