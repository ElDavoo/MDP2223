#include <fstream>

template <typename T>
std::istream& raw_read(std::istream& is, T& val, const size_t size = sizeof(T)) {
	return is.read(reinterpret_cast<char*>(&val), size);
}

int main(int argc, char ** argv) {
	if (argc != 3) return EXIT_FAILURE;
	
	std::ifstream is(argv[1], std::ios::binary);
	if (!is) return EXIT_FAILURE;

	std::ofstream os(argv[2]);
	if (!os) return EXIT_FAILURE;

	int32_t tmp;

	while (raw_read(is, tmp)) { os << tmp << std::endl; }

	is.close();
	os.close();

	return EXIT_SUCCESS;
}