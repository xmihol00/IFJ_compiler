package main
func main() {
    
    str1 := "test1"
    str2 := "test2"

    if str1 == str2 {
        print("@1\n")
    } else if str2 == "test1" {
        print("@2\n")
    } else if str2 >= str1 {
        print("@3\n")
        str1 = "test"
        if str1 == "test1" {
            str1 = "new test 1"
            print("@3.1\n")
        } else {
            str1 = "new test 2"
            print("@3.2\n")
            if str1 == str2 {
                str1 = "another test1"
                print("@3.3\n")
            } else if str1 < str2 {
                str1 = "another test2"
                print("@3.4\n")
                str1 := str1
                print("new str1: ", str1, "\n")
                if str1 != "" {
                    str1 = "empty"
                    print("new str1 again: ", str1, "\n")
                    str1 := "new s1\n"
                    print(str1)
                    if str1 != str2 {
                        print("not eq\n")
                    }

                    if str1 >= str2 {
                        print("greater or eq\n")
                    } else if str1 <= str2 {
                        str1 := "little"
                        print("less or eq\n")
                        print(str1, "\n")
                    }

                    if str1 == str2 {
                        print(str1, "\n")
                    } else if str1 > str2 {
                        print("greater\n")
                    } else {
                        str1 := "small"
                        print("less\n")
                        print(str1, "\n")
                    }
                    print(str1, " ", str2, "\n")
                    str1 = ":("
                    if str1 == ":)" {
                        print("smile\n")
                    } else if str1 == str2 {
                        print("equal to str2")
                    } else {
                        print("sad ", str1, "\n")
                    }
                }
                print(str1, " ", str2, "\n")
                str1 = ":|"
                if str1 == ":|" {
                    print("midioker\n")
                }
            }
            print(str1, " ", str2, "\n")
            str1 = ":D"
            if str1 == ":)" {
                print("happy\n")
            } else if str1 == ":D" {
                print("funny ", str1, "\n")
            }
        }
        print(str1, " ", str2, "\n")
        str1 = "test1"
        if str1 == "test1" {
            print ("string renewed value is: ", str1, "\n")
        }
    } else if str1 >= str2 {
        print("@4\n")
        if str2 == str2 {
            str2 = str2
        }
        str2:= "test"
        print(str2, "\n")
        if str2 <= "test" {
            str2 = "hello"
        } else if str1 == str2 {
            str2 = "bye"
        }
    } else {
        print(str1, "\n")
        str1 = str2
        print("@5\n")
        print(str1, "\n")
    }
    print(str1, " ", str2, "\n")
}

func foo() (string) {
    return "Good morning"
}
