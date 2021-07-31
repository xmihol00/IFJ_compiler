
package main
func main() {
	
	for i :=0 ; i< 5; i += 1 {
		a, err := inputi()
		if (err == 0) {
			if a > 0 {
				print("a is positive\n")
			} else if a == 0 {
				print("a is zero\n")
			} else {
				print("a is negative\n")
			}
		} else {
			print("wrong input\n")
		}

		b, err := inputi()
		if (err == 1){
			print("wrong input\n")
		} else {
			if b > 0 {
				print("b is positive\n")
			} else if b < 0 {
				print("b is negative\n")
			} else {
				print("b is zero\n")
			}
		}

		c, err := inputb()
		if (err == 0){
			if c {
				print("c true\n")
			} else if !c {
				print("c false\n")
			} else {
			}
		} else {
			print("wrong input\n")
		}

		d, err := inputs()
		if (err == 1) {
			print("wrong input\n")
		} else {
			print("input: ", d, "\n")
		}
	}
}