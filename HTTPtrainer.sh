#!/bin/bash

tab=("get" "Host" "User-Agent" "Accept" "Connection")
nb=12

for i in `seq 1 $nb`
do
echo get$i
for mot in ${tab[*]}
do
./httpparser $(pwd)/Only4G33ks/testFile/test$i.txt $mot
echo
./main $(pwd)/Only4G33ks/testFile/test$i.txt $mot
echo
done
echo
echo
echo
done
