package main
func main() {
	a := 8
	b := 5
	c := 6
	if a > b {
        print("@1\n")
        a = 4
        if a < 5 {
            print("@1.1\n")
            a = -a
        }
        a += a
    } else if c > b {
        print("@2\n")
        b = 5
    } else if b > a {
        print("@3\n")
        c = 8
    } else if a > c {
        if a*2 < a {
            print("NoNoNo\n")
        }
        print("@4\n")
    } else if b > c {
        a = 5
        print("@5\n")
    } else {
        print("@6\n")
    }
    print(a, " ", b, " ", c, "\n")

    if a > b {
        for i:= 1; i < 5; i += 2 {
            print("test\n")
        }
        print("@7\n")
    } else {
        print("@8\n")
        a = b*2 -a
    }
    print(a, " ", b, " ", c, "\n")
}