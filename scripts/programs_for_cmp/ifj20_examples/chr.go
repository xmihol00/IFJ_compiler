package main
func main() {

	x,y:=chr(69)
	print(x," ",y,"\n")

	x,y=chr(0)
	print(x," ",y,"\n")

	x,y=chr(255)
	print(" ",y,"\n")

	x,y=chr(254)
	print(x," ",y,"\n")

	x,y=chr(126)
	print(x," ",y,"\n")

	x,y=chr(127)
	print(x," ",y,"\n")

	x,y=chr(128)
	print(x," ",y,"\n")

	x,y=chr(134)
	print(x," ",y,"\n")

	x,y=chr(176)
	print(x," ",y,"\n")

	x,y=chr(-1)
	print(y,"\n")

	x,y=chr(256)
	print(y,"\n")

	for i := 0; i < 256; i += 1 {
		x, y = chr(i)
		print(x, " ", y, "\n")
	}
}
