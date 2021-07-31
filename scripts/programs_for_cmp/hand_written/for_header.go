package main
func main() {
	
	for i:=2; i > 0; i-=1 {
		print("first cycle\n")
	}

	x := 0 
	for i := 0; i < 2; i, x = i+1, i {
		print("second cycle x: ", x, "\n")
	}
	x = x-x+x+1
	for ;x>0; x = x -1{
		print("third cycle\n")
	}

	for ;x<2; {
		print("fourth cycle\n")
		x += 1
	}

	for i, j := 2, 1 ; i != j ; i,j=i+1,j*2{
		print("fift cycle\n")
	}

	for i,j := 8, 4; i != j;{
		i -= 1
		j +=1
		print("sixth cycle\n")
	}

	for _,_,i := 5, 5, 2 ; i>0 ; i-=1{
		print("seventh cycle\n")
	}

	for i := 2; i>0 ; _,i, _=2, i/2, 5{
		x:= 5
		print("eight cycle x: ", x, "\n")
	}
	print("x at the end ", x, "\n")
}