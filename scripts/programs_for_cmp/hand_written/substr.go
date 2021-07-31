package main
func main() {
	x:= "WellWell"
	y,z:=substr(x, 2, 55)
	print(y,"-->",z,"\n")

	y,z = substr(x, -3, 4)
	print(y,"-->",z,"\n")

	y,z = substr(x, 1, -3)
	print(y,"-->",z,"\n")

	y,z = substr(x, 0, 2)
	print(y,"-->",z,"\n")

	y,z = substr(x, 3, 0)
	print(y,"-->",z,"\n")

	y,z = substr(x, 7, 2)
	print(y,"-->",z,"\n")

	y,z = substr(x, 0, 2)
	print(y,"-->",z,"\n")

	_,z = substr("", 0, 3)
	print("Neuspech-->",z,"\n")

	_,z = substr("Lopata", 3, 66)
	print("Uspech-->",z,"\n")
///////////////////TODO
	y,_ = substr(x, 0, 55)
	print(y,"-->\n")

	y,_ = substr(x, -1, -3)
	print(y,"-->\n")
}
