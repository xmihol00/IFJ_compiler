#!/bin/bash

term=("12" "123abc" ".0" "\"test\"" "\"a\\x41\\x42\"b" "\"abc\\x2r\""
"73.65e+3" "73.65e3" "73.65e.3" "73.65e+" "73.65e-" "15e10" "15.e10" "15.99e+-10" "15.99ee-10" "$" "]"
"123_456" "88_88_99" "12.12.36" "1_2_3" "1_" "0b0_1_1_0_" "0x0__2F" ": =" "\"\\x41\\x42")
# add expected return code here
ret_vals=(2 2 1 2 2 1 2 2 1 1 1 2 1 1 1 1 1 2 2 1 2 1 1 1 1 1)
NL=$'\n'

for (( i=0; i<${#ret_vals[@]}; i++))
do    
    ./compiler <<<${term[$i]}${NL} 2>/dev/null >/dev/null
    err_code=$?
    if [ ${err_code} -ne ${ret_vals[$i]} ] ; then
        echo "term: ${term[$i]} exited with worng error code $err_code expected ${ret_vals[$i]}"
    else
        echo "${term[$i]} run OK"
    fi
done 
