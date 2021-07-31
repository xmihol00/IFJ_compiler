package main

func main() () {
	
	print("Enter the number of rows: ")
	i, err1 := inputi()
	print("Enter the number of columns: ")
	j, err2 := inputi ()

	if err1 == 1 || err2 == 1 {
		print("Invalid input\n")

	}
    c1 := 0
    c2 := 2
    c3 := 0
	for ; i > 0; i -= 1 {
		for j:=j ; j>0; j -=1 {
            space := ""
            if (i >= j) {
                space = " "
            }
			print(i >= j, space, " ")
		}
        print("\n")
        for j:=j ; j>0; j -=1 {
            space := ""
            if (i < j) {
                space = " "
            }
			print(i < j, space+ " ")
            for i :=i; i>=0; i -= 1{
                if i > j {
                    c1 = c1 +1
                } else if j == i {
                    if c2 < c3 {
                        c2 *= c2
                    } else {
                        c2 += -c3
                    }
                } else {
                    c3 -= 2
                }
            }
		}
		print("\n")
	}

    print("counter1: ", c1, " counter2: ", c2, " counter3: ", c3, "\n")
}
