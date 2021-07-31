package main

func main () {
    in, _ := inputi()
    b, i, f, v3 := f1(float2int(f3(int2float(in)))), f2(), f3(int2float(in)-1.6), f4(f3(int2float(in - 3)))
    if b {
        print (i, "\n")
    } else if v3 {
        print (f, "\n")
    } else {
        print(float2int(f) + i, "\n")
    }

}

func f1 (i int) bool {
    if i == 20  || i>25 {
        print("Tu")
        return true
    }
    return false
}

func f3 (t float64) float64{
    if t > 10.10 {
        return 20.0
    }
    return 15.0
}

func f4(f float64) bool {
    print(f, "\n")
    if f > 110.1e-1 {
        return true
    } else if (float2int(f) < 0x19) {
        return false
    } else  {
        return f > 0.0
    }
}

func f2 () int {
    return float2int(f3(20.0))
}