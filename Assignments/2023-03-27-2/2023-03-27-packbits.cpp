#include <istream>

template <typename T>
std::istream& raw_read(std::istream& is, T& val, size_t size = sizeof(T)) {
	return is.read(reinterpret_cast<char*>(&val), size);
}

template <typename T>
std::ostream& raw_write(std::ostream& os, T& val, size_t size = sizeof(T)) {
	return os.write(reinterpret_cast<char*>(&val), size);
}

class bitreader {
	uint8_t buffer_;
	uint8_t n_ = 0;
	std::istream& is_;
public:
	bitreader(std::istream& is) : is_(is) {}

	bool read_bit() {
		if (n_ == 0) {
			raw_read(is_, buffer_);
			n_ = 8;
		}
		--n_;
		return (buffer_ >> n_) & 1;
	}

	uint32_t read(uint8_t n) {

	}

};