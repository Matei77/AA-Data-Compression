#! /bin/bash

echo "Compressing using the LZW algorithm"
for i in {01..60}
do
	./Lempel-Ziv-Welch/lzw -c in/test$i.in out/test$i.out1

	uncompressed_size=$(du -b in/test$i.in | awk '{print $1;}')
	compressed_size=$(du -b out/test$i.out1 | awk '{print $1;}')

	awk "BEGIN {printf(\"compressed test$i.in \t -- compression ratio: %.2f \t -- space saving ratio: %.2f%%\n\",
	($uncompressed_size / $compressed_size), ((1.0 - $compressed_size/$uncompressed_size) * 100.0))}"
done

echo ;

echo "Decompressing using the LZW algorithm"
for i in {01..60}
do
	./Lempel-Ziv-Welch/lzw -d out/test$i.out1 aux/aux$i.in1
	echo "decompressed test$i.out1"
done

echo ;

echo "Checking for differences after decompressing"
for i in {01..60}
do
	diff -s in/test$i.in aux/aux$i.in1
done