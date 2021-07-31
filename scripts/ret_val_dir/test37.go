// wrong number of function ret values
package main
func main() {
    if 8 < foo() {

	}
}

func foo () (a int, b int) {
	return
}