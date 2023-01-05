///
/// @author Matei-Stefan Ionescu
/// @brief Huffman file compressor
/// @remark This code is a slightly modified version of Patrick Sheehan's code
///			that can be found at https://github.com/patricksheehan/Huffman-Compression
///
/// This is the C++11 implementation of a Huffman single-file command-line compressor.
///

#include <cstdint>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <ios>
#include <iostream>
#include <istream>
#include <limits>
#include <map>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <sys/resource.h>
#include <unistd.h>

#include "utils.h"


long get_mem_usage()
{
	struct rusage myusage;

	getrusage(RUSAGE_SELF, &myusage);
	return myusage.ru_maxrss;
}

///
/// @brief Compresses the contents of `input_file` and writes the result to
/// `output_file`.
/// @param [in] input_file      input stream
/// @param [out] output_file    output stream
///
void compress(std::istream &input_file, std::ofstream &output_file)
{
	unsigned int frequencies[256] = { 0 };

	std::string codebook[256];

	unsigned char nextChar;
	input_file >> noskipws;
	while (input_file >> nextChar) {
		frequencies[nextChar]++;
	}

	Node *root = constructHeap(frequencies);

	std::string code;
	root->fillCodebook(codebook, code);

	printCompressedData(frequencies, codebook, input_file, output_file);
}

///
/// @brief Decompresses the contents of `input_file` and writes the result to
/// `output_file`.
/// @param [in] input_file      input stream
/// @param [out] output_file    output stream
///
void decompress(std::istream &input_file, std::ostream &output_file)
{
	unsigned int frequencies[256] = { 0 };

	std::string codebook[256];

	char magic[8];
	input_file.read(magic, 8);
	unsigned char nextByte;
	for (int i = 0; i < 256; i++) {
		input_file.read((char *)&frequencies[i], 4);
	}

	Node *root = constructHeap(frequencies);

	std::string code;
	root->fillCodebook(codebook, code);

	input_file >> noskipws;
	while (input_file >> nextByte) {
		for (int i = 0; i < 8; i++) {
			if ((nextByte >> i) & 0x01)
				code += '1';
			else
				code += '0';
			for (int i = 0; i < 256; i++) {
				if (codebook[i] == code) {
					if (frequencies[i]) {
						output_file.write((const char *)&i, sizeof(char));
						code.clear();
						frequencies[i]--;
						break;
					} else
						return;
				}
			}
		}
	}
}

///
/// @brief Prints usage information and a custom error message.
/// @param s    custom error message to be printed
/// @param su   Show Usage information
///
void print_error(const std::string &message = "", bool show_usage = true)
{
	if (!message.empty()) {
		std::cerr << "\nERROR: " << message << '\n';
	}

	if (show_usage) {
		std::cerr << "\nUsage:\n";
		std::cerr << "\tprogram -flag input_file output_file\n\n";
		std::cerr << "Where `flag' is either `c' for compressing, or `d' for "
					 "decompressing, and\n";
		std::cerr << "`input_file' and `output_file' are distinct files.\n\n";
		std::cerr << "Examples:\n";
		std::cerr << "\t./huffman -c test.in test.out\n";
		std::cerr << "\t./huffman -d test.out aux.in\n";
	}

	std::cerr << std::endl;
}

int main(int argc, char *argv[])
{

	if (argc != 4) {
		print_error("Wrong number of argumets.");
		return EXIT_FAILURE;
	}

	enum class Mode { Compress, Decompress };

	Mode mode;

	if (std::string(argv[1]) == "-c") {
		mode = Mode::Compress;

	} else if (std::string(argv[1]) == "-d") {
		mode = Mode::Decompress;

	} else {
		print_error(std::string("flag `") + argv[1] + "' is not recognized.");
		return EXIT_FAILURE;
	}

	std::ifstream input_file(argv[2], std::ios_base::binary);

	if (!input_file.is_open()) {
		print_error(std::string("input_file `") + argv[2] +
					"' could not be opened.");
		return EXIT_FAILURE;
	}

	std::ofstream output_file(argv[3], std::ios_base::binary);

	if (!output_file.is_open()) {
		print_error(std::string("output_file `") + argv[3] +
					"' could not be opened.");
		return EXIT_FAILURE;
	}

	try {
		input_file.exceptions(std::ios_base::badbit);
		output_file.exceptions(std::ios_base::badbit | std::ios_base::failbit);

		if (mode == Mode::Compress) {
			compress(input_file, output_file);

			// uncomment to show memory usage
			// printf("memory usage: %ld\n", get_mem_usage());


		} else if (mode == Mode::Decompress) {
			decompress(input_file, output_file);
			
			// uncomment to show memory usage
			// printf("memory usage: %ld\n", get_mem_usage());

		}
	} catch (const std::ios_base::failure &f) {
		print_error(std::string("File input/output failure: ") + f.what() + '.',
					false);
		return EXIT_FAILURE;
	} catch (const std::exception &e) {
		print_error(std::string("Caught exception: ") + e.what() + '.', false);
		return EXIT_FAILURE;
	}

	input_file.close();
	output_file.close();

	return EXIT_SUCCESS;
}