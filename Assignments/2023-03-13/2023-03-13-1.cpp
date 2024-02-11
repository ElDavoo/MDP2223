#include <iostream>
#include <fstream>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <cstdlib>
#include <array>
#include <iomanip>

typedef struct cha {
	uint64_t f_;
	uint8_t c_;

	inline bool operator==(const cha& rhs) { return c_ == rhs.c_ && f_ == rhs.f_; }
	inline bool operator!=(const cha& rhs) { return c_ != rhs.c_ || f_ != rhs.f_; }
	inline bool operator< (const cha& rhs) { return f_ < rhs.f_; }
	inline bool operator> (const cha& rhs) { return f_ > rhs.f_; }
	inline bool operator<=(const cha& rhs) { return f_ <= rhs.f_; }
	inline bool operator>=(const cha& rhs) { return f_ >= rhs.f_;	}
}cha;

int main(int argc, char* argv[]) {
	if (argc != 3) {
		std::cout << "Usage: " << argv[0] << "<filein.txt> <fileout.txt>\n";
		return 1;
	}

	std::array<cha, 256> ar{};

	for (int i = 0; i < 256; i++) ar[i].c_ = i;

	std::ifstream is(argv[1], std::ios::binary);
	if (!is) return EXIT_FAILURE;



	uint8_t tmp;
	while (is >> tmp) {
		++ar[tmp].f_;
	}

	is.close();

	/*struct
	{
		bool operator()(cha a, cha b) const { return a.c < b.c; }
	}
	customLess;*/
	std::sort(ar.begin(), ar.end(), std::less<>());

	std::ofstream os(argv[2]);
	if (!os) return EXIT_FAILURE;

	for (auto e : ar) {
		if (!e.f_) continue;
		os << std::setfill('0') << std::setw(2) << std::hex << static_cast<unsigned int>(e.c_) << std::dec;
		os << "\t";
		os << e.f_;
		os << std::endl;
	}

	os.close();

	return 0;

}