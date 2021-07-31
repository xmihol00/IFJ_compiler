package main

func main() {
    print(f1())
}

func f1()(int){
    param := 77
    var2 := f2(f3(param))

    return var2
}

func f2 (i int) int {
    return i
}

func f3 (param int) int {
    param += 20
    param /= f5(14.5)

    return float2int(f6(param))
}

func f5(f float64) int {
    return float2int(f)
}

func f6(i int) (float64) {
    if(f7(i) > f8(i+1)) {
        return int2float(f9(i))
    } else {
        return int2float(f9(i+10))
    }
}

func f9(j int) (int){
    v:=j
    str := "number "
    for ; f10(v) == true; v+=1 {
        print(str, v)
    }
    return v
}

func f10(r int) (bool) {
    if r < 10 {
        return true
    } else {
        return false
    }
}

func f7(i int) (float64) {
    v, v1, v3 := 110, f12(14), f13(15)
    return int2float(v3 - i - v +v1)
}

func f8(i int) (float64) {
    v, v1, var2 := f13(20), f12(f13(15)), int2float(f2(11))

    return int2float(v1 + v + float2int(var2))
}

func f12(i int) (int) {
    return i+f13(i)
}

func f13(i int) (int) {
    return 156 + i
}

