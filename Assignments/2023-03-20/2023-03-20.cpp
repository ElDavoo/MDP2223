#include <iostream>
#include <fstream>
#include <vector>

template <typename T>
std::istream& raw_read(std::istream& is, T& val, const size_t size = sizeof(T)) {
	return is.read(reinterpret_cast<char*>(&val), size);
}

template <typename T>
std::ostream& raw_write(std::ostream& os, const T& val, const size_t size = sizeof(T)) {
	return os.write(reinterpret_cast<const char*>(&val), size);
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
	bitreader(std::ifstream& is) : is_(is) {
		buf_ = 0;
		rem_ = 0;
	}

	T read(uint8_t n) {
		T tmp = 0;
		while (n-- > 0) {
			tmp = (tmp << 1) | (read_bit() & 1);
		}
		return tmp;
	}

	bool fail() { return is_.fail(); }
};

template <typename T>
class bitwriter {
	uint8_t buf_;
	uint8_t pos_;
	std::ofstream& os_;

	void write_bit(bool bit){
		if (pos_ == 8) {
			raw_write(os_, buf_);
			pos_ = 0;
		}
		++pos_;
		buf_ = (buf_ << 1) | (bit & 1);
	
	}
public:
	bitwriter(std::ofstream& os) : os_(os) {
		buf_ = 0;
		pos_ = 0;
	}

	void write(T content, int size) {
		while (size-- > 0) {
			write_bit((content >> size) & 1);
		}
	}

	bool fail() { return os_.fail(); }

	~bitwriter() {
		while (pos_ != 1) write_bit(0);
		os_.close();
	}
};

unsigned int map(const int n) {
	if (n < 0) return n * -2;
	return (n * 2) + 1;
}

int unmap(const unsigned int n) {
	if (n % 2) {
		return (n - 1) / 2;
	}
	return (n / 2) * -1;
}

void compress(char* filein, char* fileout) {
	std::ifstream is(filein);
	if (!is) exit(EXIT_FAILURE);

	std::ofstream os(fileout, std::ofstream::binary);
	if (!os) exit(EXIT_FAILURE);

	bitwriter<int> bw(os);
	
	// non c'è bisogno di fare due loop separati
	std::vector<int> nums;
	int tmp;
	while (is >> tmp) {
		nums.push_back(map(tmp));
	}



	for (auto e : nums) {
		auto f = e;
		f = f >> 1;
		int n = 1;
		while (f) {
			bw.write(0, 1);
			++n;
			f = f >> 1;
		}
		bw.write(e, n);

	}
}

void decompress(char *filein, char *fileout) {
	std::ifstream is(filein, std::ios::binary);
	if (!is) exit(EXIT_FAILURE);
	bitreader<int> br(is);

	std::ofstream os(fileout);
	if (!os) exit(EXIT_FAILURE);
	
	while (!br.fail()) {
		int n = 0;
		while (!br.read(1)) ++n;
		if (br.fail()) break;
		//il primo uno è stato già scartato
		int num = 1 << n;

		int num2 = br.read(n) | num;
		if (br.fail()) break;
		os << unmap(num2) << std::endl;
	}

}

int main(int argc, char** argv) {
	if (argc != 4) return EXIT_FAILURE;

	if (argv[1][1] != 0) return EXIT_FAILURE;

	bool compression;

	if (argv[1][0] == 'c') {
		compression = true;
	}
	else if (argv[1][0] == 'd') {
		compression = false;
	}
	else return EXIT_FAILURE;

	if (compression) {
		compress(argv[2], argv[3]);
	}
	else {
		decompress(argv[2], argv[3]);
	}
	return EXIT_SUCCESS;
}