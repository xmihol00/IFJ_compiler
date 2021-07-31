#!/bin/bash

## takes any "*.go" file from --> TEST_FILES_PATH <-- directory and runs it with both GOlang and ifj20 compiler, 
## then compares the results. GOlang must be installed

## run from ../ directory

RED='\033[1;31m'
ORANGE='\033[0;33m'
GREEN='\033[1;32m'
NOCOLOR='\033[0m'

## the path and name of the tested compiler
COMPILER_PATH_NAME="./compiler"
## the path of the tested files
TEST_FILES_PATH="scripts/programs_for_cmp/ifj20_examples"

for file in "$TEST_FILES_PATH"/* ; do
	if [[ "$file" != ""$TEST_FILES_PATH"/ifj20.go" ]] && [[ "$file" != *.txt ]]; then
		
		compiler_output=$(valgrind --tool=memcheck --leak-check=full --show-reachable=yes --errors-for-leak-kinds=all --error-exitcode=69 "$COMPILER_PATH_NAME" <"$file" 2> /dev/null)
		compiler_ret_val=$?
		
		if [ -f "${file/\.go/\.txt}" ]; then
			interpret_output=$(./scripts/ic20int <(echo "$compiler_output") <"${file/\.go/\.txt}" 2>/dev/null)
			go_output=$(cat "${file/\.go/\.txt}" | go run $file "$TEST_FILES_PATH"/ifj20.go 2> /dev/null) 
		else
			interpret_output=$(./scripts/ic20int <(echo "$compiler_output") 2>/dev/null)
			go_output=$(go run $file "$TEST_FILES_PATH"/ifj20.go 2>/dev/null) 
		fi
		go_ret_val=$?
		
		## rounding of a floating point number for Go output replacing p+0 for p+
		go_output=$(echo "$go_output" | sed 's/[0-9a-f]\{4\}p+/p+/g; s/[0-9a-f]\{4\}p-/p-/g; s/p+0/p+/g; s/p-0/p-/g;' )
		interpret_output=$(echo "$interpret_output" | sed 's/[0-9a-f]\{4\}p+/p+/g; s/[0-9a-f]\{4\}p-/p-/g;' )
		
		# uncomment to compare files from float_expressions
		# the outputs of Go sometimes have different lenghts than the interpret output
		go_output=$(echo "$go_output" | cut -c -11 )
		interpret_output=$(echo "$interpret_output" | cut -c -11 )

		if [ $go_ret_val -eq 127 ]; then
			echo "error: GOlang compiler missing"
			exit
		elif [ $compiler_ret_val -eq 127 ]; then
			echo "error: Project needs to be compiled"
			exit
		fi
		
		if [ $compiler_ret_val -eq 69 ]; then
			echo -e "Memory leaks in ${file##*/}! -->$RED FAILED$NOCOLOR" 
		elif [ $go_ret_val -gt 0   -a  $compiler_ret_val -gt 0 ] && [ $go_ret_val -eq $compiler_ret_val ] ; then
			echo -e "For ${file##*/} compilation failed with the same error: $go_ret_val -->$GREEN PASSED$NOCOLOR"
		elif [ $go_ret_val -eq 0 -a $compiler_ret_val -eq 0 ]; then
			if [ "$go_output" == "$interpret_output" ]; then
				echo -e "Same output for ${file##*/} -->$GREEN PASSED$NOCOLOR"
			else
				echo -e "Different output for ${file##*/} -->$RED FAILED$NOCOLOR"
				echo "$( diff --color=always <(echo "$go_output") <(echo "$interpret_output"))"
			fi			
		else
			echo -e "For ${file##*/} compilation failed, go compiler ret. value is $go_ret_val, our compiler ret. val is $compiler_ret_val -->$ORANGE WARNING$NOCOLOR"
		fi

	fi
done


