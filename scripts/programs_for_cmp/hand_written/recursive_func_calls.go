package main

func f(n int) int {
   if n <= 1 {
      return n
      }
   return f1(n-1) + f(n-2) + r(r1(r2(n))) * 2
}

func main () {
    f3()
}

func f1(n int) (int) {
    d := n - 1
    if n < 2 {
        return 1
    } else {
        t := f1(d)
        return n * t + r(n) * 2
    }
}

func f3()() {
    for i:=0; i < 13; i+=1 {
        print(f(i),"\n")
    }
}

func r(k int) int{
    res:=0
  if k>0 {
        res = k+r(k-1)
    } else {
        res = 0
    }
  return res
}

func r2(k int) int{
    res:=0
  if k>0 {
        res = k+r(k-2)
    } else {
        res = 0
    }
  return res
}
func r1(k int) int{
    res:=0
  if k>0 {
        res = k+r(k-3)
    } else {
        res = 0
    }
  return res
}

