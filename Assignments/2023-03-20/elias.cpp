// 20230320-Elias.cpp : Questo file contiene la funzione 'main', in cui inizia e termina l'esecuzione del programma.
//

#include <iostream>
#include <cmath>
#include <fstream>

template<typename T>
std::istream& raw_read(std::istream& is, T& val, size_t size = sizeof(T)) {
	return is.read(reinterpret_cast<char*>(&val), size);
}

template<typename T>
std::ostream& raw_write(std::ostream& os, const T& val, size_t size = sizeof(T)) {
	return os.write(reinterpret_cast<const char*>(&val), size);
}

class bitreader {
	uint8_t buffer_;
	uint8_t n_ = 0;
	std::istream& is_;

public:
	bitreader(std::istream& is) : is_(is) {}

	uint32_t read_bit() {
		if (n_ == 0) {
			raw_read(is_, buffer_);
			n_ = 8;
		}
		--n_;
		return (buffer_ >> n_) & 1;
	}

	uint32_t read(uint8_t n) {
		uint32_t u = 0;
		while (n-- > 0) {
			u = (u << 1) | read_bit();
		}
		return u;
	}

	uint32_t operator()(uint8_t n) {
		return read(n);
	}

	bool fail() const {
		return is_.fail();
	}

	explicit operator bool() const {
		return !fail();
	}
};

class bitwriter {
	uint8_t buffer_;
	int n_ = 0;
	std::ostream& os_;

	std::ostream& write_bit(uint32_t bit) {
		buffer_ = (buffer_ << 1) | (bit & 1);
		++n_;
		if (n_ == 8) {
			raw_write(os_, buffer_);
			n_ = 0;
		}
		return os_;
	}

public:

	void const write(uint32_t number_) {
		int loga = log2(number_);
		for (int i = 0; i < loga; i++) {
			write_bit(0);
		}
		for (int i = 0; i <= loga; i++) {
			write_bit((number_ >> (loga - i)) & 1);
		}
	}

	bitwriter(std::ostream& os) : os_(os) {}

	std::ostream& flush(uint32_t bit = 0) {
		while (n_ > 0) {
			write_bit(bit);
		}
		return os_;
	}

	~bitwriter() {
		flush();
	}
};

uint32_t const map_number(int x) {
	if (x >= 0) {
			return 2 * x + 1;
	}
	return -2*x;
}






int main(int argc, char* argv[])
{
	if (argc != 4 || strlen(argv[1]) != 1) {
		std::cout << "Usage: " << argv[0] << " [c|d] <filein> <fileout>";
		return 1;
	}



	switch (argv[1][0]) {
	case 'c':
	case 'C':
	{

		std::ifstream is(argv[2]);
		std::ofstream os(argv[3], std::ios::binary);

		if (!is || !os) {
			std::cout << "Apertura file fallita" << std::endl;
			return 1;
		}

		bitwriter bw(os);

		int32_t number;
		while (is >> number) {
			bw.write(map_number(number));
		}

		return 0;
	}
	case 'd':
	case 'D':
	{

		std::ifstream is(argv[2], std::ios::binary);
		std::ofstream os(argv[3]);

		if (!is || !os) {
			std::cout << "Apertura file fallita" << std::endl;
			return 1;
		}


		// ????

		return 0;
	}
	default:
	{
		std::cout << "Invalid mode: " << argv[1];
		return 1;
	}
	}

}
