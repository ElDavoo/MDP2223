#pragma once
#include <vector>
#include <string>
#include <cstdint>

struct BinaryImage {
	int W = 0;
	int H = 0;
	std::vector <uint8_t> ImageData;

	bool ReadFromPBM(const std::string& filename);
};