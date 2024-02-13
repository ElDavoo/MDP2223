#include "pbm.h"
#include <fstream>
#include <iostream>

template <typename T>
std::istream& raw_read(std::istream& is, T &content, size_t size = sizeof(T)) {
	return is.read(reinterpret_cast<char*>(&content), size);
}

template <typename T>
class bitreader {
	uint8_t buf_;
	uint8_t rem_;
	std::ifstream& is_;
	bool read_bit() {
		if (rem_ == 0) {
			raw_read(is_, buf_);
			rem_ = 8;
		}
		return (buf_ >> --rem_) & 1;
	}
public:
	T read(size_t size) {
		T read = 0;
		while (size-- > 0) {
			read = (read << 1) | (read_bit() & 1);
		}
		return read;
	}
	bool fail() { return is_.fail(); };
	bitreader(std::ifstream& is) : is_(is) {
		buf_ = 0;
		rem_ = 0;
	}
	~bitreader() {
		is_.close();
	}

};

bool BinaryImage::ReadFromPBM(const std::string& filename) {
	std::ifstream is(filename, std::ios::binary);
	if (!is) return false;
	bitreader<uint8_t> br(is);
	const std::string magic = "P4";
	std::string str;
	is >> str;
	if (magic != str) return false;
	is >> str;
	if (str[0] == '#') is >> str;
	W = std::stoi(str);
	is >> str;
	H = std::stoi(str);
	if (br.read(8) != '\n') return false;
	while (!br.fail()) {
		if (is.peek() == EOF) break;
		ImageData.push_back(br.read(8));
	}
	return true;

}

Image BinaryImageToImage(const BinaryImage& bimg) {
	Image image;
	image.H = bimg.H;
	image.W = bimg.W;
	int bits = image.W;
	for (auto byt : bimg.ImageData) {
			for (int i = 7; i != -1; i--) {
				if (!bits) break;
				image.ImageData.push_back(((byt >> i) & 1) ? 0 : 255);
				bits--;
			}
		if (!bits) bits = image.W;
	}
	return image;
}