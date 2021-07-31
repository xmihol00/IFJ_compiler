//redefinition of function with the same name
package main
func main() {
    var1:= f1(1)
}

func f1(i int) (int) {
    return i+10
}

func f1(i int) (int) {
    return 10
}