// wrong ret vals
package main

func main() {

}

func str(test string,  i int) (string, float64) {
	return "test string"
}

func flt(i int) float64 {
	return int2float(i+78-2)
}
