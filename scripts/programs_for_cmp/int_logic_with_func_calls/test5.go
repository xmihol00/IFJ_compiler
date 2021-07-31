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
	o := 1
	p := 2
	q := 3

	x0 := foo0()+(-p)-(-(-1))-(-(-3))-(((q*6)-(-(-(-1)))*(-(-1))-foo2())) >= foo0()
	print(x0,"\n")

	x1 := !!((-3)*(6-p-(+foo0())-(-3)) < 6*6) || !(foo1()-foo2()/foo0() == foo0())
	print(x1,"\n")

	x2 := !(foo1()-p == p+(q*o+(-6)*(-(-1))-q-foo0()*+q/-o/+o+foo1())-foo1())
	print(x2,"\n")

	x3 := !((((foo1()+p-q+6)-foo0()/q-foo1()+(-(+2))/(-(-1)))*-q)-(+o)/foo1() != foo2()/p)
	print(x3,"\n")

	x4 := (-(-1))-((-(-1))+(-(-1))-(-o)-6*foo1()) < (-(-1))
	print(x4,"\n")

	x5 := foo2() == 6-(-6)-o*+(foo0()/(+2)-(foo2()-(+foo1())+q))
	print(x5,"\n")

	x6 := !!!!!!!(foo1() <= (q-(p/q+(-(-3))-foo0()))) || (q-(-p)+(+2)+(-foo1())+p-foo2()+(+foo2())*foo2()-foo1()) >= foo2()
	print(x6,"\n")

	x7 := ((+2)*o+foo2())
	print(x7,"\n")

	x8 := ((+2)-p) >= foo0()
	print(x8,"\n")

	x9 := (foo1()-(-o)-(6/+p)) == (p*+o*q+q)*foo2()
	print(x9,"\n")

	x10 := 6 <= (foo2()*(-3)) || !(((6/+o)) == foo0())
	print(x10,"\n")

	x11 := !((-3) < (foo0()-(-3)*o+q))
	print(x11,"\n")

	x12 := !(foo1()+(+(-(-1)))-(+2) >= (+2)) && !!!!(((((-3)-(-p)-p))) != (+2))
	print(x12,"\n")

	x13 := q < 6+(+o)-foo0() || ((+2)*-(foo2()+(-6)+6)) >= p
	print(x13,"\n")

	x14 := !!(q > 6-(((-(-1))-6)))
	print(x14,"\n")

	print(o,p,q,"\n")
}
