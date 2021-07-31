package main

func main() {
    i := 0b10000
    print(0b10000, "\n")
    f := int2float(i)
    j := float2int(f)
    print(j, "\n")

    print(i - 0o20, "\n")

    y := int2float(0x4DA)
    print(y, "\n")
    print(float2int(y), "\n")

    a := 0o1411 - 0b1010
    print(a, "\n")

    q := int2float(0o154) - 14.2

    print(q + int2float(0x4D), "\n")

    ii := 0b010101
    g := float2int(int2float(ii))
    print(g)

    inp, _ := inputi()
    print(inp - 0b10110, "\n")

    print(0xF14 - float2int(f1()), "\n")

    ff1, _ := inputf()

    print(float2int(ff1) - 0o125, "\n")

    t1 := 0b1111
    print(t1, "\n")
    t1 = -0b1111
    print(t1, "\n")
    t1 = 0B11
    print(t1, "\n")
    t1 = -0B11
    print(t1, "\n")
    t1 = 065
    print(t1, "\n")
    t1 = -065
    print(t1, "\n")
    t1 = 0o652
    print(t1, "\n")
    t1 = -0o657
    print(t1, "\n")
    t1 = 0xAbc123
    print(t1, "\n")
    t1 = -0X012BaB
    print(t1, "\n")
}

func f1() float64 {
    return 5.5
}