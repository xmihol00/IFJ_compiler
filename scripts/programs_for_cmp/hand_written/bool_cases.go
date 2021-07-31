package main

func f1 (b bool, i int) (bool, int){
    v := false
    if(b) {
        v = f2(b && i < 10)
    }
    return v, f3(i)
}

func f2 (b bool) bool {
    if(b) {
        return f3(200) > 1
    } else {
        return f3(100) > 1
    }
}

func f3 (i int) int {
    return i/100
}

func main () {
    print(f6(f4(f2(f4(true)))))
}

func f4(fdf bool) (bool){
    for ii:=13; f5(fdf, ii); {
        print("IfJ")
        bbb, _ := f1(fdf, ii)
        fdf = fdf || bbb
        ii -= 1
        print("\n")
    }
    bb, _ := f1(f3(f3(20000)) >= 0, 15)
    return 1555 >= 1554 || bb
}

func f5(b bool, i int) bool{
    if (b && i > 0) {
        return true
    } else {
        return false
    }
}

func f6(b bool) (string){
    return "OK"
}
