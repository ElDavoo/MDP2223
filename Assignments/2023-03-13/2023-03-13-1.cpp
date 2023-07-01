#pragma warning(disable: 4710)

#include <algorithm>
#include <concepts>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <unordered_map> 

template<typename KeyT, typename ValueT> auto sortMapByValue(std::unordered_map<KeyT, ValueT>& map) {

	std::vector<std::pair<KeyT, ValueT>> sorted(map.begin(), map.end());
	std::sort(sorted.begin(), sorted.end(), [](const auto& a, const auto& b) {
		if (a.second != b.second) return a.second < b.second;
		return a.first < b.first;
		});
	return sorted;

}

template<typename T, typename CountT = uint32_t>
class frequencies {

	std::unordered_map<T, CountT> count_;

	auto it() { return sortMapByValue(count_); }

public:

	void operator()(const T& val) {
		++count_[val];
	}

	void print(std::ostream &os) requires std::integral<T> {
		for (const auto& pair : it()) {
			os << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(pair.first) << std::dec << "\t" << pair.second << std::endl;
			if (os.fail()) {
				std::cout << "Error writing to stream." << std::endl;
				exit(EXIT_FAILURE);
			}
		}
	}
};


int main(int argc, char* argv[]) {

	if (argc != 3) {
		std::cout << "Usage: " << argv << "<filein.txt> <fileout.txt>\n";
		return EXIT_FAILURE;
	}

	std::ifstream is(argv[1], std::ios::binary);

	if (is.fail()) {
		std::cout << "Error opening input file." << std::endl;
		return EXIT_FAILURE;
	}

	std::ofstream os(argv[2]);
	if (os.fail()) {
		std::cout << "Error opening output file." << std::endl;
		return EXIT_FAILURE;
	}

	frequencies<uint8_t> freq;

	while (1) {

		int num = is.get();

		if (num == EOF || is.eof()) {
			break;
		}

		uint8_t x = static_cast<uint8_t>(num);
		freq(x);

	}

	freq.print(os);

	return EXIT_SUCCESS;

}