// Pouziti: go run mycode.go ifj20.go
// POZOR! mycode.go musi zachovat priponu go, jinak bude problem s oznacenim package jako main dle prologu v zadani.
// Zajisteni zakladni kompatibility IFJ20->Go @ Merlin, verze 0.5

package main

import (
	"bufio"
	"fmt"
	"os"
)

var __αscannerα__ = bufio.NewScanner(os.Stdin)

func inputi() (int, int) {
	var a int
	scanner := __αscannerα__
	scanner.Scan()
	line := scanner.Text()
	_, err := fmt.Sscan(line, &a)
	if err != nil {
		return a, 1
	} else {
		return a, 0
	}
}

func inputf() (float64, int) {
	var a float64
	scanner := __αscannerα__
	scanner.Scan()
	line := scanner.Text()
	_, err := fmt.Sscan(line, &a)
	if err != nil {
		return a, 1
	} else {
		return a, 0
	}
}

func inputs() (string, int) {
	scanner := __αscannerα__
	ok := scanner.Scan()
	if ok == false {
		err := scanner.Err()
		if err != nil {
			return "", 1
		} else {	// EOF
			return scanner.Text(), 1
		}
	}
	return scanner.Text(), 0
}

// inputb - podporovano v ramci rozsireni BOOLTHEN
func inputb() (bool, int) {
	var b bool
	scanner := __αscannerα__
	scanner.Scan()
	line := scanner.Text()
	_, err := fmt.Sscan(line, &b)   // case-insensitive načtení "true" nebo "1" dává true, vše ostatní false, chyba nastava pokud jsou na vstupu jen bílé znaky (ani jedno písmeno nebo číslice)
	if err != nil {
		return b, 1
	} else {
		return b, 0
	}
}

func substr(s string, i int, n int) (string, int) {
	var l int = len(s)
	if i < 0 || i >= l {
		return "", 1
	}

	if n < 0 {
		return "", 1
	}

	if n > (l - i) {
		return string(s[i:l]), 0
	} else {
		return string(s[i : i+n]), 0
	}
}

func ord(s string, i int) (int, int) {
	if i < 0 || i >= len(s) {
		return 0, 1
	} else {
		return int(s[i]), 0
	}
}

func chr(i int) (string, int) {
	ascii := [128]string{"\x80", "\x81", "\x82", "\x83", "\x84", "\x85", "\x86", "\x87", "\x88", "\x89", "\x8a", "\x8b", "\x8c", "\x8d", "\x8e", "\x8f",
						 "\x90", "\x91", "\x92", "\x93", "\x94", "\x95", "\x96", "\x97", "\x98", "\x99", "\x9a", "\x9b", "\x9c", "\x9d", "\x9e", "\x9f", 
						 "\xa0", "\xa1", "\xa2", "\xa3", "\xa4", "\xa5", "\xa6", "\xa7", "\xa8", "\xa9", "\xaa", "\xab", "\xac", "\xad", "\xae", "\xaf", 
						 "\xb0", "\xb1", "\xb2", "\xb3", "\xb4", "\xb5", "\xb6", "\xb7", "\xb8", "\xb9", "\xba", "\xbb", "\xbc", "\xbd", "\xbe", "\xbf", 
						 "\xc0", "\xc1", "\xc2", "\xc3", "\xc4", "\xc5", "\xc6", "\xc7", "\xc8", "\xc9", "\xca", "\xcb", "\xcc", "\xcd", "\xce", "\xcf", 
						 "\xd0", "\xd1", "\xd2", "\xd3", "\xd4", "\xd5", "\xd6", "\xd7", "\xd8", "\xd9", "\xda", "\xdb", "\xdc", "\xdd", "\xde", "\xdf", 
						 "\xe0", "\xe1", "\xe2", "\xe3", "\xe4", "\xe5", "\xe6", "\xe7", "\xe8", "\xe9", "\xea", "\xeb", "\xec", "\xed", "\xee", "\xef", 
						 "\xf0", "\xf1", "\xf2", "\xf3", "\xf4", "\xf5", "\xf6", "\xf7", "\xf8", "\xf9", "\xfa", "\xfb", "\xfc", "\xfd", "\xfe", "\xff", };
	if i < 0 || i > 255 {
		return "", 1
	} else {
		if i >= 128 {
			return ascii[i-128], 0
		} else {
			return string(i), 0
		}
	}
}

func int2float(i int) (float64) {
	return float64(i)
}

func float2int(f float64) (int) {
	return int(f)
}

func print(a ...interface{}) {
	for _, item := range a {
		switch item.(type) {
		case float64:
			fmt.Printf("%x", item)
		case int:
			fmt.Printf("%d", item)
		default:
			fmt.Print(item)
		}
	}
}

// Zde bude nasledovat program jazyka IFJ20 (ne tak uplne, kod bude asi pred timto source code)
