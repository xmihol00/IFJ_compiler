package main
func main() {

	if "ahoj" == "ahoj" {
		print("ahoj\n")
	}

	if "A" < "B" {
		print("A is less than B\n")
	}

	if "test" > "ahoj" {
		print("greater\n")
	}

	a := "string"
	b := "empty"

	if a == "string" {
		print("a: ", a, "\n")
	}

	if a != b {
		print ("a and b differs\n")
	}

	if b + " " + a == "empty string" {
		print(b, " ", a, "\n")
	}

	if b + " " + a != "not empty string" {
		print("not ", b, " ", a, "\n")
	}

	if b < a {
		print ("b is less\n")
	}

	if b > a {
		print("incorrect\n")
	} else {
		print ("a is not less\n")
	}

	if a >= "string" {
		print ("a is more then a string\n")
	}

	a = "ahoj, " + "jak " + "to " + "jde" + "? " + ":)" + "\n"
	print (a)
}