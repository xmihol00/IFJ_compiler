
package main

func main() {
		
	a, b, c := baz(85.3)
	print(a, "\n")
	print(b, "\n")
	print(c, "\n")
	print(bar(78))
	k,l:=foo(88)
	print(k, "\n")
	print(l, "\n")
}

func bar (i int) (
	int ) {
	return i
}

func baz (i float64) (
	int, 
	float64, 
	string) {
	return 45, i, "test"
}

func foo(i int) (
	named int, 
	a int) {
	named = i
	a = -i
	return
}
