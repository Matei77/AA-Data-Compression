**Name: Ionescu Matei-Ștefan**  
**Group: 323CAb**

# AA Homework #1 - Data Compression #

## Summary: ##

This project aims to compare two popular lossless data compression algorithms, the Huffman coding
algorithm and the Lempel-Ziv-Welch algorithm.

## Algorithms ##

### Huffman coding ###

The code for the huffman coding algorithm is a slightly modified version of Patrick Sheehan's code
that can be found at https://github.com/patricksheehan/Huffman-Compression.

### LZW ###

The code for the LZW algorithm is a slightly modified version of Julius Pettersson's code
that can be found at https://cplusplus.com/articles/iL18T05o/#Version1.

More information about the algorithms can also be found in the paper comparing them (the '10.pdf' file). 

## Scripts ##

The folowing three scripts are included in the appropriately named scripts folder:
1. run_lzw.sh
	
	This script will compress using the LZW algorithm each of the 60 input files, showing the
	compression ratio achieved for each of them, then decompress them in aux files and finally
	comparing the input file with the decompressed one to ensure that the compression was in fact
	lossless.

2. run_huffman.sh
	
	This script will do the same things as the run_lzw.sh script, the only difference being that
	it will use the Huffman coding algorithm.

3. run_hyperfine.sh
	
	This script will run the hyperfine benchmark utilty to compare the execution time of the LZW and
	Huffman algorithms for both compression and decompression.
	In order to work, it is necesary to have the hyperfine utility installed. Instruction for doing
	so can be found at https://github.com/sharkdp/hyperfine.

## How to run the code ##

A makefile is included that has the following rules:

1. build - will compile the codes and create an executable file for each of them
2. run-best - will run the 'run_lzw.sh' script
3. run-p1 - will also run the 'run_lzw.sh' script
4. run-p2 - will run 'run_huffman.sh; script
5. run-hyperfine - will run the 'run_hyperfine.sh' script
6. clean - will delete the output and aux files, as well as the executables for each algorithm.