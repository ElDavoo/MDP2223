#include <fstream>
#include <iostream>
#include <vector>

template <typename T>
std::istream& raw_read(std::istream& is, T& val, const size_t size = sizeof(T)) {
	return is.read(reinterpret_cast<char*>(&val), size);
}

template <typename T>
std::ofstream& raw_write(std::ofstream& os, const T& val, const size_t size = sizeof(T)) {
	return os.write(reinterpret_cast<const char*>(&val), size);
}


template <typename T>
class bitreader {
	uint8_t buf_;
	uint8_t rem_;
	std::ifstream &is_;
public:
	bitreader(std::ifstream &is) : is_(is) {
		std::cout << "bitread constructor" << std::endl;
		buf_ = 0;
		rem_ = 0;
	}
	bool readbit() {
		if (!rem_) {
			raw_read(is_, buf_);
			rem_ = sizeof(buf_) * 8;
		}
		return buf_ >> --rem_ & 1;
	}
	T read(uint8_t n) {
		T tmp = 0;
		while (n-- > 0) {
			tmp = (tmp << 1) | readbit();
		}
		return tmp;
	}

	T operator()(uint8_t n) {
		return read(n);
	}

	bool fail() const { return is_.fail(); }
	operator bool() const {
		std::cout << "bool operator" << std::endl;
		return !is_.fail(); 
	}

	~bitreader() {
		is_.close();
	}

};

int main(int argc, char** argv) {
	if (argc != 3) return EXIT_FAILURE;

	std::ifstream is(argv[1]);
	if (!is) return EXIT_FAILURE;

	std::ofstream os(argv[2], std::ofstream::binary);
	if (!os) return EXIT_FAILURE;

	bitreader<int16_t> br(is);

	std::vector<int16_t> v;
	int16_t temp;
	while (true) {
		temp = br.read(11);
		if (temp > 1000) {
			temp = temp - 2048;
		}
		if (!br) break;
		v.push_back(temp);
	}

	for (auto e : v) {
		os << e << std::endl;
	}

	os.close();

	return EXIT_SUCCESS;
	
}