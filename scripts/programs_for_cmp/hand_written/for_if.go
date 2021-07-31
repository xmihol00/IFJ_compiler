package main
func main() {
	a := 8
	b := 5
	c := 6
	for i := 0; i < a; i += 1 {
		if i > a/2 {
			for i := a; i >= 0; i -= 1 {
				if i < b {
					print(i, " ")
				}
			}
			print("\n")
		} else if a == c {
			a = 5
		} else if a == b {
			a = c
		} else {

		}

		
		for i := 0; i < a; i = i +1{
			print("@")
		}

		if ( a > 0 ) {
			print("\n")
		}
    }
    print(a, " ", b, " ", c, "\n")
}