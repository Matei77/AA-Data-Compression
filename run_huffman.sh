#! /bin/bash

echo "Compressing using the Huffman algorithm"
for i in {1..40}
do
	./Huffman/huffman -c in/test$i.in out/test$i.out2
	echo "compressed test$i.in"
done

echo ;

echo "Decompressing using the Huffman algorithm"
for i in {1..40}
do
	./Huffman/huffman -d out/test$i.out2 aux/aux$i.in2
	echo "decompressed test$i.out2"
done

echo ;

echo "Checking for differences after decompressing"
for i in {1..40}
do
	diff -s in/test$i.in aux/aux$i.in2
done