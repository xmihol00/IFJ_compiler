package main
func main() {
	b := 8*9
	a := 45+36
	print(a, " ", b, "\n")
	b, _ = inputi()
	a, _ = inputi()
	print(a, " ", b, "\n")
	a, b = b*2, a/2
	print(a, " ", b, "\n")
	c := true
	d := false
	print(c, " ", d, "\n")
	a = len("ahoj "+"jak " + "se "+"mas?")+len("celkem")+len("to")+len("jde")
	print(a, " ", b, "\n")
	str1 := " "
	str1, a = inputs()
	str2 := " "
	str2, _ = inputs()
	print(str1, " ", str2, " ", a, "\n")
	print(str1 + " " + str2+ "\n")
	a = float2int(58.36*87.36+74.21)
	print(a, "\n")
	b = float2int(58.36*87.36+74.21*int2float(a-41))
	print(b, "\n")
	str3 := ""
	str3 ,_= chr(55)
	print("len: ", len(str2), " ", len(str1), len(str3), str3)
	str3, a = substr("aaa bbb ccc", len("aaa "), len("aaa bbb"))
	print(str3)
	g:=22
	a,b, g = a+b+g, b+g, g*658
	print(a, " ", b, " ", g, "\n")
	a,b, g = a+b+g, a-b+g, g*658
	print(a, " ", b, " ", g, "\n")
	print(to_int(inputi()))
	str4 := "docela dlouhy string " + "jeste pridavek :)\n"
	print(str4)
	str4, _ = substr(str4, float2int(12.36987), 12+8)
	print(str4, "\n")
	str4, _ = substr(":DDDDDDDDDDDDDDDDDDDDDDD", float2int(12.36987), to_int(len("Ahoj"), float2int(int2float(8))))
	print(str4, "\n")
	str4, _ = substr(":DDDDDDDDDDDDDDDDDDDDDDD", float2int(-12.36987), to_int(len("Ahoj"), float2int(int2float(8))))
	print(str4, "\n")
	str4, a = substr(":DDDDDDDDDDDDDDDDDDDDDDD", float2int(-12.36987), to_int(-len("Ahoj"), -float2int(int2float(8))))
	print(a, "\n")
	str5 := "A"+"B"+"C"+"D"+"E"+"F"+"G"+"H"+"I"+"J"
	b, a = ord(str5, len(str5)-2+4)
	print(a ,"\n")
	b, a = ord(str5, len(str5)-6)
	print(b, " ", a ,"\n")
	b, a = ord(str5, len(str5)-len(str5)*2)
	print(a ,"\n")
	a = -len(str5)*g/123
	print(a, "\n")
	a, _ = ord("a", 0)
	b, _ = ord("z", 0)
	print(a, " ", b, "\n")
	str6 := "abcdefghijklmnopqrstuvwxyz"
	for i := 0; i <= len(str6); i += 1 {
		str := " "
		str, _ = chr(i+a) 
		print(str)
	}
	print("\n")
	_, _ = inputs()
	_, _ = inputf()
	_, _ = inputb()
	_, _ = inputi()
}

func to_int(a int, b int) int {
	return a+b
}
