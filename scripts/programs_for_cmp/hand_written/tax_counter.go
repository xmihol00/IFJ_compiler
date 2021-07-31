package main

func main () () {
    cents := 1000
    dollars := 100 * cents

    	/*I want to buy 4 Hamburgers ($5.50 each) and 2 Milkshakes ($2.86 each)
    	// Calcualte the Total Price Before Tax = $27.72 = 4 * $5.50 + 2 * $2.86
    	// Calculate the Tax (7.65%) amount $2.12 = $27.72 * 0.765
    	// Calculate the Total Price $29.84 = $27.72 + $2.12*/

    	hamburgerPrice := ToUSD(5.50, dollars)
    	milkshakePrice := ToUSD(2.86, dollars)
    	totalBeforeTax := 4*hamburgerPrice + 2*milkshakePrice
    	print("TotalBeforeTax: ")
    	print(totalBeforeTax, "\n")

    	tax := ToUSD(0.0765 * Dollars(totalBeforeTax, dollars), dollars)
    	total := totalBeforeTax + tax

    	print("Tax: ")
    	print(tax, "\n")
    	print("Total: ")
    	print(total, "\n")
}

func Dollars(i int, dollars int) float64 {
	return int2float(i) / int2float(dollars)
}

func Cents(d int, cents int) float64 {
	return int2float(d) / int2float(cents)
}

// ToUSD creates a USD from a float (e.g. 2.56 -> 2*Dollars + 56*Cents)
func ToUSD(d float64, dollars int) int {
	return float2int(d) * dollars
}

func str (d int) {
	//return fmt.Sprintf("$%.2f", d.Dollars())
	print(d)
}
