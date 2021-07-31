//redefinition of variable
package main
func main() {
    var1:= 2
    for i:=12; i < 100; i += 12 {
        var2:=2
        var3:=0
        if i > 14 {
            var3:=5
        }
        var2:= var3
    }
}