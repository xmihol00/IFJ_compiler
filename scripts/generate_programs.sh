#!/bin/bash

for i in {1..15}
do
	file="test$i.go"
	./expression_generator >programs_for_cmp/logic_int_if_conditions_recursive/$file
	go run programs_for_cmp/logic_int_if_conditions_recursive/$file
	until [[ $? == 0 ]]
	do
		./expression_generator >programs_for_cmp/logic_int_if_conditions_recursive/$file
		go run programs_for_cmp/logic_int_if_conditions_recursive/$file
	done
done
