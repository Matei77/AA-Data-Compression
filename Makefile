.PHONY: build
build: lzw huffman

lzw: Lempel-Ziv-Welch/lzw.cpp
	g++ -Wall -Wextra -std=c++11 -O3 Lempel-Ziv-Welch/lzw.cpp -o Lempel-Ziv-Welch/lzw

huffman: Huffman/huffman.cpp Huffman/utils.cpp
	g++ -Wall -Wextra -std=c++11 -O3 Huffman/huffman.cpp Huffman/utils.cpp -o Huffman/huffman

.PHONY: run-best
run-best: scripts/run_lzw.sh
	./scripts/run_lzw.sh

.PHONY: run-p1
run-p1: scripts/run_lzw.sh
	./scripts/run_lzw.sh

.PHONY: run-p2
run-p2: scripts/run_huffman.sh
	./scripts/run_huffman.sh

.PHONY: run_hyperfine
run-hyperfine: scripts/run_hyperfine.sh
	./scripts/run_hyperfine.sh

.PHONY: clean
clean:
	rm -f out/* aux/* Lempel-Ziv-Welch/lzw Huffman/huffman
