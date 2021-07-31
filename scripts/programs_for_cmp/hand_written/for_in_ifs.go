package main

func main() {
    j, _ := inputi()
    u, _ := inputb()
    i, _ := inputi()
    p, _ := inputi()
    m, _ := inputi()
    for t := 0; t < j; t += 1 {
        if t > 5 {
            print("1")
            if u {
                for u:= 0; u < i; u += 1 {
                    print("3")
                    if u > 3 {
                        for a:=0; a < m; a += 1 {
                            print("t")
                        }
                    }
                }
			}
        } else if t < 3 {
            if !u {
                print("tu")
            } else {
                if(u) {
                    for i:=0; i < 10; i += 1 {
                        print("4")
                        for q:=0; q < p; q += 1{
                            print(q -10)
                            if q == 4 {
                                  for e := 0; e < j; e += 1 {
                                       print("w")
                                  }
                            }
                        }
                    }
                }
            }
        }
    }
}
