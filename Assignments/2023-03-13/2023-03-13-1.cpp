#include <iostream>
#include <fstream>
#include <map>
#include <unordered_map>
#include 

template<typename T, typename CountT = uint32_t>
class frequ {
	std::unordered_map<T, CountT> count_;

	void operator()(const T& val) {
		++count_[val];
	}

	auto begin() { return count_.begin(); }
	auto end() { return count_.end(); }


	auto cbegin() { return count_.cbegin(); }
	auto cend() { return count_.cend(); }
};


int main(int argc, char* argv[]) {
	if (argc != 3) {
		std::cout << "Usage: " << argv[0] << "<filein.txt> <fileout.txt>\n";
		return 1;
	}

	std::ifstream is(argv[1], std::ios::binary);

	if (is.fail()) {
		printf("Error opening input file.\n");
		return 1;
	}


	std::ofstream os(argv[2]);
	if (os.fail()) {
		printf("Error opening output file.\n");
		return 1;
	}

//	std::map<uint8_t, unsigned int> m;
	std::array<uint32_t, 256> m;
	while (1) {
		uint8_t x;

		int num = is.get();
		
		if (num == EOF) {
			break;
		}

		if (is.eof()) {
			break;
		}

		uint8_t x = num;

		std::basic_ios::fill(m.start(), m.end(), 0);
		/*if (m.find(x) == m.end()) {
			m[x] = 1;
		}
		else {
			m[x]++;
		}*/

	}

	return 0;

}