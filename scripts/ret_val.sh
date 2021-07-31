#!/bin/bash

# add new test file here
file_names=("test1.go" "test2.go" "test3.go" "test4.go" "test5.go" "test6.go" "test7.go" "test8.go" "test9.go" "test10.go"
            "test11.go" "test12.go" "test13.go" "test14.go" "test15.go" "test16.go" "test17.go" "test18.go" "test19.go"
            "test20.go" "test21.go" "test22.go" "test23.go" "test24.go" "test25.go" "test26.go" "test27.go" "test28.go"
            "test29.go" "test30.go" "test31.go" "test32.go" "test33.go" "test34.go" "test35.go" "test36.go" "test37.go"
            "test38.go" "test39.go" "test40.go" "test41.go" "test42.go" "test43.go" "test44.go" "test45.go" "test46.go"
            "test47.go" "test48.go" "test49.go" "test50.go" "test51.go" "test52.go" "test53.go" "test54.go" "test55.go")
# add the expected exit code
ret_vals=(3 3 1 0 0 9 5 2 3 1 0 9 6 6 3 3 3 6 6 6 6 2 2 0 2 2 2 2 2 2 2 3 7 7 7 6 6 2 0 2 0 2 2 2 6 0 2 2 2 2 2 6 0 2 2)

RED='\033[1;31m'
GREEN='\033[1;32m'
NOCOLOR='\033[0m'

for (( i=0; i<${#ret_vals[@]}; i++))
do
    line="$(head -1 "scripts/ret_val_dir/${file_names[$i]}")"
    echo ${line}

    ./compiler <"./scripts/ret_val_dir/${file_names[$i]}" 2>/dev/null >/dev/null
    exit_code=$?
    if [ $exit_code -ne ${ret_vals[$i]} ] ; then
        echo -e "file: ${file_names[$i]} exited with worng error code$RED $exit_code$NOCOLOR expected$RED ${ret_vals[$i]}$NOCOLOR";
    else
        echo -e "file: ${file_names[$i]} run$GREEN OK$NOCOLOR";
    fi
done 
