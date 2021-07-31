package main

func foo0() (int){
	return 1
}

func foo1() (int){
	return 2
}

func foo2() (int){
	return 3
}

func main() {
	o,p,q :=1,2,3

	x0 := !!((foo1()+p) > p) || (-(-1))-(p+foo1())+(+(+2))*(-(-1))+(+(-3))+(p/foo1()-(+2)-foo1()) < p-(+6)-(-(-1))
	print(x0,"\n")

	x1 := ((6-(-(-(-1)))+q))
	print(x1,"\n")

	x2 := !!(foo2()-(o-(-3)-(-(-1))+foo1())+(+6)+foo1() <= foo2())
	print(x2,"\n")

	x3 := !((-(-1))-(+foo1())+(((-(-1))-foo2())+foo2()) >= (+2)+q)
	print(x3,"\n")

	x4 := (-(-1))/+(+2) == (-3) && !(p > (p+o-(+foo2())*p)-(-o)+q)
	print(x4,"\n")

	x5 := !(foo2() < ((-3)-(-(-(-1)))-foo2())) || !(q*o > q)
	print(x5,"\n")

	x6 := !((-(-1)) == p) && 6-(o-(-(-1))) < (+2)+o
	print(x6,"\n")

	x7 := (foo0()/6) != p
	print(x7,"\n")

	x8 := !(((o+foo2()*+foo1()))-(-(-3))+p+foo0() > (+2))
	print(x8,"\n")

	x9 := ((-3)+((+2)-(-foo2())*(+2)-(+o)/(-3))*p*+foo1())
	print(x9,"\n")

	x10 := !(p-((+2)-(-3)+(+foo1())/foo2()) >= o)
	print(x10,"\n")

	x11 := (q+foo1())
	print(x11,"\n")

	x12 := (foo1()*o-(+p)*foo2()) < (-3)
	print(x12,"\n")

	x13 := !(((-(-1))+((-(-1))+(-(-3))*foo0()))+foo2() < (-3))
	print(x13,"\n")

	x14 := !((6*-(((-3)*-p-(-o)*p)+q*+o-p)*+foo1())-(-3)/foo0() <= p)
	print(x14,"\n")

	print(o,p,q,"\n")
}
