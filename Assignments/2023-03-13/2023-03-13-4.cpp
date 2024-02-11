#include <fstream>
#include <cstdint>

template <typename T>
std::ifstream& raw_read(std::ifstream &is, T &val, const size_t size = sizeof(T)) {
	return is.read(reinterpret_cast<char *>(& val), size);
}

template <typename T>
std::ostream& raw_write(std::ostream& os, T& val, const size_t size = sizeof(T)) {
	return os.write(reinterpret_cast<const char *>(&val), size);
}

template <typename T>
class bitwriter {
	uint8_t buf_;
	uint8_t rem_;
	std::ostream& os_;
	std::ostream& write_bit(bool bit) {
		buf_ = (buf_ << 1) | (bit & 1);
		++rem_;
		if (rem_ == sizeof(buf_) * 8) {
			raw_write(os_, buf_, sizeof(buf_));
			rem_ = 0;
		}
		return os_;
	}
public:
	bitwriter(std::ostream& os) : os_(os) {
		buf_ = 0;
		rem_ = 0;
	}
	std::ostream& write(T content, int n) {
		while (n-- > 0) {
			uint16_t test = (content >> n) & 1;
			write_bit(test);
		}
		return os_;
	}

	std::ostream& operator()(T content, int n) {
		return write(content, n);
	}

	~bitwriter() {
		while (rem_) {
			write_bit(0);
		}
		os_.flush();
	}
};

int main(int argc, char** argv) {
	if (argc != 3) return EXIT_FAILURE;

	std::ifstream is(argv[1]);
	if (!is) return EXIT_FAILURE;

	std::ofstream os(argv[2]);
	if (!os) return EXIT_FAILURE;

	bitwriter<int16_t> bw(os);

	int16_t tmp;

	while (is >> tmp) {
		if (tmp < -1000 || tmp > 1000) break;
		bw(tmp, 11);
	}

	is.close();
	return EXIT_SUCCESS;

}