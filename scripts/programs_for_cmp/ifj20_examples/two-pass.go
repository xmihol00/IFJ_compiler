// IFJ20: Ukazka volani funkce f pred jeji definici

package main

func g() {
	j := 0
	j = f(3)
	print(j, "\n")
	j = f(j)
	print(j, "\n")
}

func f(i int) (int) {
	print(i, "\n")
	return i
}

func main() {
  g()
}