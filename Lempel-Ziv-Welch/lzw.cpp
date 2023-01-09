///
/// @author Matei-Stefan Ionescu
/// @brief LZW file compressor
/// @remark This code is a slightly modified version of Julius Pettersson's code
///			that can be found at https://cplusplus.com/articles/iL18T05o/#Version1
///
/// This is the C++11 implementation of a Lempel-Ziv-Welch single-file
/// command-line compressor. It uses the simpler fixed-width code compression
/// method.
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


/// Type used to store and retrieve codes.
using CodeType = std::uint16_t;

namespace globals {

/// Dictionary Maximum Size (when reached, the dictionary will be reset)
const CodeType dms { std::numeric_limits<CodeType>::max() };

} // namespace globals

///
/// @brief Helper operator intended to simplify code.
/// @param vc   original vector
/// @param c    element to be appended
/// @returns vector resulting from appending `c` to `vc`
///
std::vector<char> operator+(std::vector<char> vc, char c)
{
	vc.push_back(c);
	return vc;
}

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

	std::map<std::vector<char>, CodeType> dictionary;

	// "named" lambda function, used to reset the dictionary to its initial
	// contents
	const auto reset_dictionary = [&dictionary] {
		dictionary.clear();

		const long int minc = std::numeric_limits<char>::min();
		const long int maxc = std::numeric_limits<char>::max();

		for (long int c = minc; c <= maxc; ++c) {
			// to prevent Undefined Behavior, resulting from reading and
			// modifying the dictionary object at the same time
			const CodeType dictionary_size = dictionary.size();

			dictionary[{ static_cast<char>(c) }] = dictionary_size;
		}
	};

	reset_dictionary();

	std::vector<char> s; // String
	char c;

	while (input_file.get(c)) {
		// dictionary's maximum size was reached
		if (dictionary.size() == globals::dms)
			reset_dictionary();

		s.push_back(c);

		if (dictionary.count(s) == 0) {
			// to prevent Undefined Behavior, resulting from reading and
			// modifying the dictionary object at the same time
			const CodeType dictionary_size = dictionary.size();

			dictionary[s] = dictionary_size;
			s.pop_back();
			output_file.write(reinterpret_cast<const char *>(&dictionary.at(s)),
							  sizeof(CodeType));
			s = { c };
		}
	}

	if (!s.empty())
		output_file.write(reinterpret_cast<const char *>(&dictionary.at(s)),
						  sizeof(CodeType));
	
}

///
/// @brief Decompresses the contents of `input_file` and writes the result to
/// `output_file`.
/// @param [in] input_file      input stream
/// @param [out] output_file    output stream
///
void decompress(std::istream &input_file, std::ostream &output_file)
{
	std::vector<std::vector<char>> dictionary;

	// "named" lambda function, used to reset the dictionary to its initial
	// contents
	const auto reset_dictionary = [&dictionary] {
		dictionary.clear();
		dictionary.reserve(globals::dms);

		const long int minc = std::numeric_limits<char>::min();
		const long int maxc = std::numeric_limits<char>::max();

		for (long int c = minc; c <= maxc; ++c)
			dictionary.push_back({ static_cast<char>(c) });
	};

	reset_dictionary();

	std::vector<char> s; // String
	CodeType k; // Key

	while (input_file.read(reinterpret_cast<char *>(&k), sizeof(CodeType))) {
		// dictionary's maximum size was reached
		if (dictionary.size() == globals::dms)
			reset_dictionary();

		if (k > dictionary.size())
			throw std::runtime_error("invalid compressed code");

		if (k == dictionary.size())
			dictionary.push_back(s + s.front());
		else if (!s.empty())
			dictionary.push_back(s + dictionary.at(k).front());

		output_file.write(&dictionary.at(k).front(), dictionary.at(k).size());
		s = dictionary.at(k);
	}

	if (!input_file.eof() || input_file.gcount() != 0)
		throw std::runtime_error("corrupted compressed file");
}

///
/// @brief Prints usage information and a custom error message.
/// @param message      custom error message to be printed
/// @param show_usage   Show Usage information
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
		std::cerr << "\t./lzw -c file.in compressed_file.out\n";
		std::cerr << "\t./lzw -d compressed_file.out decompressed_file.in\n";
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

			// uncomment to show memroy usage
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