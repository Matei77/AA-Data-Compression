.PHONY: run-p1
run-p1:
	./run_lzw.sh

.PHONY: run-p2
run-p2:
	./run_huffman.sh

.PHONY: clean
clean:
	rm out/* aux/*
