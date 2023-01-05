#include "utils.h"

#include <fstream>
#include <iostream>

using namespace std;

void Node::fillCodebook(string *codebook, string &code)
{
	if (!leftC && !rightC) {
		codebook[data] = code;
		return;
	}
	if (leftC) {
		code += '0';
		leftC->fillCodebook(codebook, code);
		code.erase(code.end() - 1);
	}
	if (rightC) {
		code += '1';
		rightC->fillCodebook(codebook, code);
		code.erase(code.end() - 1);
	}
}

Node::Node(Node *rc, Node *lc)
{
	frequency = rc->frequency + lc->frequency;
	rightC = rc;
	leftC = lc;
	min = (rc->min < lc->min) ? rc->min : lc->min;
}

void Heap::push(Node *newNode)
{
	int currentHeapNode = ++heapSize;
	while (currentHeapNode != 1 && *minHeap[currentHeapNode / 2] > *newNode) {
		minHeap[currentHeapNode] = minHeap[currentHeapNode / 2];
		currentHeapNode = currentHeapNode / 2;
	}
	minHeap[currentHeapNode] = newNode;
}

void Heap::pop()
{
	Node *lastNode = minHeap[heapSize];
	minHeap[heapSize--] = minHeap[1];
	int currentHeapNode = 1;
	int child = 2;

	while (child <= heapSize) {
		if (child<heapSize && * minHeap[child]> * minHeap[child + 1])
			child++;

		if (*minHeap[child] > *lastNode)
			break;

		minHeap[currentHeapNode] = minHeap[child];
		currentHeapNode = child;
		child *= 2;
	} // while not at end of heap

	minHeap[currentHeapNode] = lastNode;
}

bool Node::operator>(const Node &rhs)
{
	if (frequency > rhs.frequency)
		return true;
	if (frequency < rhs.frequency)
		return false;
	if (frequency == rhs.frequency)
		if (min > rhs.min)
			return true;
	return false;
}

Node *constructHeap(unsigned int frequencies[])
{
	Heap minHeap;
	Node *nextNode;
	for (int i = 0; i < 256; i++) {
		if (frequencies[i]) {
			nextNode = new Node(i, frequencies[i]);
			minHeap.push(nextNode);
		}
	}

	Node *node1;
	Node *node2;
	Node *merged;
	while (minHeap.size() > 1) {
		node1 = minHeap.top();
		minHeap.pop();
		node2 = minHeap.top();
		minHeap.pop();
		merged = new Node(node1, node2);
		minHeap.push(merged);
	}

	return minHeap.top();
}

void printCompressedData(unsigned int frequencies[], string codebook[],
						 istream &input_file, ofstream &output_file)
{
	output_file << "HUFFMA3" << '\0';

	unsigned int i;
	for (i = 0; i < 256; i++) {
		output_file << (char)(0x000000ff & frequencies[i]);
		output_file << (char)((0x0000ff00 & frequencies[i]) >> 8);
		output_file << (char)((0x00ff0000 & frequencies[i]) >> 16);
		output_file << (char)((0xff000000 & frequencies[i]) >> 24);
	}

	unsigned char nextChar;
	char nextByte = 0;
	int bitCounter = 0;

	input_file.clear();
	input_file.seekg(0);
	while (input_file >> nextChar) {
		for (i = 0; i < codebook[nextChar].size(); i++, bitCounter++) {
			if (bitCounter == 8) {
				output_file << nextByte;
				nextByte = 0;
				bitCounter = 0;
			}
			if (codebook[nextChar][i] == '1')
				nextByte = nextByte | (0x01 << bitCounter);
		}
	}
	if (bitCounter)
		output_file << nextByte;
}