package main

func main () {
    if 25>2 || 55<21 {
		print("first if\n")
		if false {
			print(false, "\n")
		} else {
			print("second else\n")
			if 78.4>25.1*3.2+5.9 && "ahoj" == "nazdar" {
				print("false\n")
			} else if "abc" != "cba" && "bbb" == "bbb" {
				print("third else if\n")
				if true == false {

				} else if false == true {

				} else if !true {

				} else if !false {
					print("fourth else if\n")
					if !("a" == "b" || "a" > "b" || "a" >= "b" || "a" <= "b" || "a" < "b" || "a" != "b") {

					} else if "a" == "b" && "a" > "b" && "a" >= "b" && "a" <= "b" && "a" < "b" && "a" != "b" {

					} else if "a" == "b" || "a" > "b" || "a" >= "b" || "a" <= "b" && "a" < "b" && "a" != "b" {
						print("fift else if\n")
						if 45 != 45 || 45 > 45 {

						} else if 45 == 45 && 45 >= 45 && 45 <= 45 {
							print("sixth else if\n")

						} else if 45 == 45 {

						} else if 45 < 45 || 45 > 45 {

						}
					}
				} else {

				}
			}
		}
	} else {
		print(false, "\n")
	}
}


