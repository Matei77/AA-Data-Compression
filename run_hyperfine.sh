#! /bin/bash

echo "----------------------------> Comparing compression times. <----------------------------"
for i in {01..60}
do
	echo ;

	hyperfine -N "./Lempel-Ziv-Welch/lzw -c in/test$i.in out/test$i.out1" "./Huffman/huffman -c in/test$i.in out/test$i.out2"

	echo ;
	echo '-----------------------------------------------------------------------------------------------------'

done

echo ;

echo "----------------------------> Comparing decompression times. <----------------------------"
for i in {01..60}
do
	echo ;

	hyperfine -N "./Lempel-Ziv-Welch/lzw -d out/test$i.out1 aux/aux$i.in1" "./Huffman/huffman -d out/test$i.out2 aux/aux$i.in2"

	echo ;
	echo '-----------------------------------------------------------------------------------------------------'

done