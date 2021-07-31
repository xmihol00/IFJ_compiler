//redefinition of iteration variable
package main
func main() {
    for i:=1; i < 100; i+=1 {
        i:=500
        print(i)
    }
}