#! /bin/bash

echo "Compressing using the LZW algorithm"
for i in {1..40}
do
	./lzw_v5/lzw -c in/test$i.in out/test$i.out1
	echo "compressed test$i.in"
done

echo ;

echo "Decompressing using the LZW algorithm"
for i in {1..40}
do
	./lzw_v5/lzw -d out/test$i.out1 aux/aux$i.in1
	echo "decompressed test$i.out1"
done

echo ;

echo "Checking for differences after decompressing"
for i in {1..40}
do
	diff -s in/test$i.in aux/aux$i.in1
done