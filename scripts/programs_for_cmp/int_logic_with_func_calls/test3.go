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

	x0 := (foo2()+(-p)+(-(-1))+6-foo1())
	print(x0,"\n")

	x1 := !!(foo2() >= (-(-1))) && foo1() != (-3)+foo2() && (foo0()+(+o)+6*+foo1()+(+(-(-1)))-6-foo0())+(+foo1())+q <= p
	print(x1,"\n")

	x2 := !((foo2()*+(foo1()/6))+(-6)-foo0()*(-(-1))*q/foo1()+(+foo1())*foo2()+(-p)*foo1()/o+(+foo2())*foo2() < foo0())
	print(x2,"\n")

	x3 := !!((foo1()+o)+(+2)*-(foo0()-(-(-1))/+p+(-6)-(+2)*+(o*6*+o)+(+(-(-1)))-foo0()+foo0()*p) != (+2)*(+2))
	print(x3,"\n")

	x4 := ((foo1()*o))-(-o)+o-(-foo0())*o
	print(x4,"\n")

	x5 := (-3)*(((-(-1))-(-foo0())-p*+foo1()-(-p)+(-3)))+(+foo0())+foo0()
	print(x5,"\n")

	x6 := !!(foo1()+((o*-(-(-1))+(-(-(-1)))+6)) < o)
	print(x6,"\n")

	x7 := !!!!((p/-foo0()-(+2)+(-foo2())/foo0()) == (-3)) && 6 < p
	print(x7,"\n")

	x8 := !(((foo0()-(+2))) <= o)
	print(x8,"\n")

	x9 := !!(p != ((foo1()-q))*(-(-1)))
	print(x9,"\n")

	x10 := !(((-3)/-q) > foo2()+(-foo0())-foo2()) && q >= q
	print(x10,"\n")

	x11 := foo0() < (((p/-p+(-3)+foo0())))*+(-3)
	print(x11,"\n")

	x12 := !!(o-(-foo0())*foo0() > (((foo2()+(+foo2())+q))))
	print(x12,"\n")

	x13 := !(foo2()+foo1()+(-(-1)) >= 6) || q == foo2() || !!((-3) != foo2()) && !(o/+(((+2)*+(p-o+(-(-3))*(+2)))*q)-(-3)+(-q)*p < q)
	print(x13,"\n")

	x14 := 6 > (foo0()/foo0()) || !!!!!(foo1() <= 6-foo1()) || (-3) <= q || !!((+2) > (-(-1)))
	print(x14,"\n")

	print(o,p,q,"\n")
}
