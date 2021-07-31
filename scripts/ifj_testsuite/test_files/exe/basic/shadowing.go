//
package main

func main () {
	a := 30
	print(a)
	for a:=10;a>0;a=a-1 {
		print(a, "\n")
		a := 3.14
		print(a, "\n")
		if a>5.0 {
			print(a, "\n")
			a:="hello"
			print(a, "\n")
		} else {
			print(a, "\n")
			a:="world"
			print(a, "\n")
		}
	}
	print(a, "\n")
}
