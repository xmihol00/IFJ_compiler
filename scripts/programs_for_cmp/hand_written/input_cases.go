package main

func main() {
    f, _ := inputf()
    i, j := f5(f)
    b, _ := inputb()
    print(i, "\n", b, "\n", j)
}

func f5(f float64) (int, int){
    if(f > 20.02) {
        varr, _ := inputi()
        f2(f3(varr))
    }
    varr, _ := inputf()
    f6(varr)

    return inputi()
}

func f6(f float64) {
    if(f < 102.0) {
        print(f+10.2, "\n")
    }
    print(f)
}

func f3(i int) bool {
    c, _ := inputi()
    if i > c {
        return true
    } else if i>c+10 {
        return false
    } else if i < c && i >= 0 {
        return true
    }
    return false
}

func f2 (b bool) {
    if(b) {
        print("OK")
        return
    }
    print("NOT OK")
    return
}