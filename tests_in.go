package main

func fib(n int) int {
   if n <= 1 {
      return n
      }
   return fib(n-1) + fib(n-2)
}

func main (){
  n, _ := inputi()
  print("Should be 34, is: ", fib(n), "\n")
  n, _ = inputi()
  print("Should be 34, is: ", fib(n), "\n")
}
