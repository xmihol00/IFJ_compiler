//undefined function
package main
func main() {

    var2 := 89*undef(78)
    return
}

func test1 (
    var1 int, 
var2 float64) (
    string, 
float64) {
    return "test", 78.3
}

func test2 (var1 int, var2 bool) int {
    var3 := 5
    return var3
}

func test3 (

) (
    named1 string, 
    named2 int) {
    var3 := 5
    return
}

func test4 (var5 int /*inside comment*/) float64 {
    var12 := 3.9*4.3-5.3/2.1
    //single line comment
    return var12*2.2
}
/* multi
line
comment */
