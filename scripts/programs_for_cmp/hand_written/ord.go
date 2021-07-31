package main
func main() {
	x:= "Mandarinka"
	cislo:= 7
	d,c := ord(x, cislo)
	print("Uspech? ", c, "-->", d, "\n")
	d,c = ord(x, -1)
	print("Uspech? ", c, "-->", "\n")
	d,c = ord(x, 55)
	print("Uspech? ", c, "-->", "\n")
	d,c = ord(x, 0)
	print("Uspech? ", c, "-->", d, "\n")
	d,c = ord(x, 10)
	print("Uspech? ", c, "-->",  "\n")
	d,c = ord("", 0)
	print("Uspech? ", c, "-->", "\n")

	d,c = ord(x, -1)
	print("Nothing -->", c, "\n")
	d,_ = ord(x, 0)
	print( "-->", d, "\n")

	_,c = ord(x, 3)
	print("Uspech? ", c, "-->","\n")
	_,c = ord("", 0)
	print("Uspech? ", c, "-->", "\n")


}
