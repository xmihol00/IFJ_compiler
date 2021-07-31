package main

func main () {
   if f1(15) > f2(4) {
        for i:=0; f5(i) || f3(i); i += 1 {
            for j:= i; f6(j); j -= 1 {
                print("*")
            }
            print("\n")
        }
   } else if (5 < 22) {
        print(20)
   }
}

func f1 (i int) int {
    if f2(i) > 10 {
        return 15
    }
    return 12
}

func f2 (i int) int {
    if f3(i) {
        return 1
    }
    return 3
}

func f3 (i int) bool {
    if i > 20 {
        return false
    } else {
        return true
    }
}

func f5 (i int) bool {
	//print("f5 i: ", i, "\n")
    if i > 2 {
        return f5(i-1)
    }
    return false
}

func f6 (i int) bool {
    if f7(i) {
        return true
    }
    return false
}

func f7 (r int) bool{
    if r > 0 {
        return true
    }
    return false
}