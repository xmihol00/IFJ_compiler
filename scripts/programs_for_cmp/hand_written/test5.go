//correct programm
package main

func main () () {

    a := 2
	a, a, a = 1, 2, 5
	print(a, "\n")

	var3, err := inputi()
	print(var3, " ", err, "\n")
	var1 := -1+ a
	print(var1, " ", err, "\n")
	x := foo(27, 55)
	print(x,"\n")

	var2:=false
	print(var2,"\n")
	var2 = var2||(8>1||9>2 &&23+6*2 > 6)&&false
	print(var2,"\n")
	var2 = 8>1&&21>9 &&23+6*2 > 6
	print(var2,"\n")
	str1 := "test "
	print(str1,"\n")
	str2 := "jeste jednou\n"
	print(str2,"\n")
	str1 += str2
	print(str1,"\n")
    print (var3, "   ", var2, " ", str1)
    print(x, "\n")
	y := 2
	print(y,"\n")

    for i , j:=0, 0; i<var3; i , j = i+1, j-1 {
        print(i+1, " ", j-1, "\n")
    }
}

func foo (var2 int, var1 int) (int) {
    if var2 > var1 {
        return var2/3
    } else if var2 == var1 {
        return 100
    } else {
        return var1
    }
}