// wrong number of function ret values
package main
func main() {
    if foo() {

	}
}

func foo () (a int, b int) {
	return
}