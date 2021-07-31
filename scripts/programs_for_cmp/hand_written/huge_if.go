package main

func main() () {

	new_line()
	for i := 0; i < 15; i += 1{
		a,_ := inputi()
		b,_ := inputi()
		c,_ := inputi()
		d,_ := inputi()
		print(special_comaprison(a, b, c, d), "\n") 
	}
	return
}

func new_line() {
	print("\n")
}

func special_comaprison(a int, b int, c int, d int) string { //TODO not all states are correct
	if (a > 0) {
		if b > 0 {
			if c > 0 {
				if d > 0 {
					return "All numbers are positive"
				} else if d == 0 {
					return "D is 0, all other numbers are positive"
				} else {
					return "D is negative, all other numbers are positive"
				}
			} else if c == 0 {
				if d > 0 {
					return "C is 0, all other numbers are positive"
				} else if d == 0 {
					return "C and D are 0, A and B are positive"
				} else {
					return "C is 0, D is negative, A and B are positive"
				}
			} else {
				if d > 0 {
					return "C is negative, all other numbers are positive"
				} else if d == 0 {
					return "C is negative, D is 0, A and B are positive"
				} else {
					return "C and D are negative, A and B are positive"
				}
			}
		} else if b == 0 {
			if c > 0 {
				if d > 0 {
					return "B is 0, all other numbers are positive"
				} else if d == 0 {
					return "B and D are 0, A and C are positive"
				} else {
					return "B is 0, D is negative, A and C are positive"
				}
			} else if c == 0 {
				if d > 0 {
					return "A and D are positive, B and C are 0"
				} else if d == 0 {
					return "A is positive, other numbers are 0"
				} else {
					return "B and C are 0, D is negative, A is positive"
				}
			} else {
				if d > 0 {
					return "B is 0, C is negative, A and D are positive"
				} else if d == 0 {
					return "B and D are 0, C is negative, A is positive"
				} else {
					return "A is positive, B is 0, C and D are negative"
				}
			}
		} else {
			if c > 0 {
				if d > 0 {
					return "B is negative, all other numbers are positive"
				} else if d == 0 {
					return "B is negative D is 0, A and C are positive"
				} else {
					return "B and D are negative, A and C are positive"
				}
			} else if c == 0 {
				if d > 0 {
					return "A and D are positive, B is negative, C is 0"
				} else if d == 0 {
					return "A is positive, B is negative, C and D are 0"
				} else {
					return "B and D are negative, C is 0, A is positive"
				}
			} else {
				if d > 0 {
					return "B and C are negative, A and D are positive"
				} else if d == 0 {
					return "B and C are negative, A is positive, D is 0"
				} else {
					return "A is positive, other numbers are negative"
				}
			}
		}
	} else if a == 0 {
		if b > 0 {
			if c > 0 {
				if d > 0 {
					return "A is 0, all other numbers are positive"
				} else if d == 0 {
					return "A and D are 0, B and C are positive"
				} else {
					return "A is 0, B and C are positive, D is negative,"
				}
			} else if c == 0 {
				if d > 0 {
					return "A and C are 0, B and D are positive"
				} else if d == 0 {
					return "B is positive, all other numbers are 0"
				} else {
					return "B is positive, C is 0, A and D are negative"
				}
			} else {
				if d > 0 {
					return "B and D are positive, A and C are negative"
				} else if d == 0 {
					return "B is positive, D is 0,  A and C are negative"
				} else {
					return "B is positive, all other numbers are negative"
				}
			}
		} else if b == 0 {
			if c > 0 {
				if d > 0 {
					return "A and B are 0, C and D are positive"
				} else if d == 0 {
					return "B and D are 0, A is negative, C is positive"
				} else {
					return "A and D are negative, B is zero C is positive"
				}
			} else if c == 0 {
				if d > 0 {
					return "D is positive, all other numbers are 0"
				} else if d == 0 {
					return "All numbers are 0"
				} else {
					return "D is negativ, all other numbers are 0"
				}
			} else {
				if d > 0 {
					return "A and B are 0, C are negative, D is positive"
				} else if d == 0 {
					return "A and B are 0, C and D are negative"
				} else {
					return "A is positive, B is 0, C and D are negative"
				}
			}
		} else {
			if c > 0 {
				if d > 0 {
					return "B is negative, all other numbers are positive"
				} else if d == 0 {
					return "B is negative D is 0, A and C are positive"
				} else {
					return "B and D are negative, A and C are positive"
				}
			} else if c == 0 {
				if d > 0 {
					return "A and D are positive, B is negative, C is 0"
				} else if d == 0 {
					return "A is positive, B is negative, C and D are 0"
				} else {
					return "B and D are negative, C is 0, A is positive"
				}
			} else {
				if d > 0 {
					return "B and C are negative, A and D are positive"
				} else if d == 0 {
					return "B and C are negative, A is positive, D is 0"
				} else {
					return "A is positive, other numbers are negative"
				}
			}
		}
	} else {
		if b > 0 {
			if c > 0 {
				if d > 0 {
					return "All numbers are positive"
				} else if d == 0 {
					return "D is 0, all other numbers are positive"
				} else {
					return "D is negative, all other numbers are positive"
				}
			} else if c == 0 {
				if d > 0 {
					return "C is 0, all other numbers are positive"
				} else if d == 0 {
					return "C and D are 0, A and B are positive"
				} else {
					return "C is 0, D is negative, A and B are positive"
				}
			} else {
				if d > 0 {
					return "C is negative, all other numbers are positive"
				} else if d == 0 {
					return "C is negative, D is 0, A and B are positive"
				} else {
					return "C and D are negative, A and B are positive"
				}
			}
		} else if b == 0 {
			if c > 0 {
				if d > 0 {
					return "B is 0, all other numbers are positive"
				} else if d == 0 {
					return "B and D are 0, A and C are positive"
				} else {
					return "B is 0, D is negative, A and C are positive"
				}
			} else if c == 0 {
				if d > 0 {
					return "A and D are positive, B and C are 0"
				} else if d == 0 {
					return "A is positive, other numbers are 0"
				} else {
					return "B and C are 0, D is negative, A is positive"
				}
			} else {
				if d > 0 {
					return "B is 0, C is negative, A and D are positive"
				} else if d == 0 {
					return "B and D are 0, C is negative, A is positive"
				} else {
					return "A is positive, B is 0, C and D are negative"
				}
			}
		} else {
			if c > 0 {
				if d > 0 {
					return "B is negative, all other numbers are positive"
				} else if d == 0 {
					return "B is negative D is 0, A and C are positive"
				} else {
					return "B and D are negative, A and C are positive"
				}
			} else if c == 0 {
				if d > 0 {
					return "A and D are positive, B is negative, C is 0"
				} else if d == 0 {
					return "A is positive, B is negative, C and D are 0"
				} else {
					return "B and D are negative, C is 0, A is positive"
				}
			} else {
				if d > 0 {
					return "B and C are negative, A and D are positive"
				} else if d == 0 {
					return "B and C are negative, A is positive, D is 0"
				} else {
					return "A is positive, other numbers are negative"
				}
			}
		}
	}
}