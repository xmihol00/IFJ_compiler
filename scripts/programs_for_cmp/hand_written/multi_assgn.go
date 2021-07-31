package main
func main() {

	print("test of multiple assignment\n")
	vi1, vi2, vf3 := 895/5-250, 412-96, 234.25/71.36
	print("result should be: -71, 316, 0x1.a42deb5619417p+1\n")
	print("result is: ", vi1, ", ", vi2, ", ", vf3, "\n")
	vf4, vi5 := int2float((2*vi2 -8*4)/ 50) + 25.55, 4*float2int(vf3*5.3 +41.36)
	vf4, vi5 = 2.3*int2float(vi2/5), float2int(53.6 + vf3*4.4)/4
	print("result should be: 0x1.21cccccccccccp+7, 17\n")
	print("result is: ", vf4, ", ", vi5, "\n")
	vi2, vf3 = vi5, vf4
	print("result should be: 17, 0x1.21cccccccccccp+7\n")
	print("result is: ", vi2, ", ", vf3, "\n")
	vi1, vi2, vi5 = 25 + 2*vi1*vi1*8/4- 500, 420 + vi2*vi1/vi5 - 14*(vi1 + 68), (vi1 + vi2)*(vi1 - vi2)/(vi2+vi5)
	print("result should be: 19689, 391, 139\n")
	print("result is: ", vi1, ", ", vi2, ", ", vi5, "\n")
	vb6, vb7, vb8 := (vi1 > vi2) && ((vi2 - vi1 <= -5*vi2) || (-vi5 > 0)), vi5*vi2 > vi1/vi5 || vi5*vi2 < vi1/vi2, !(vi5 > vi1) == false
	print("result should be: true true false\n")
	print("result is: ", vb6, ", ", vb7, ", ", vb8, "\n")
}