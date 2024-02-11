#include <vector>
#include <fstream>

template <typename T>
std::ostream& raw_write(std::ostream &os, const T& val, size_t size = sizeof(T)) {
	return os.write(reinterpret_cast<const char*>( & val), size);
}

int main(int argc, char** argv) {
	if (argc != 3) return EXIT_FAILURE;

	std::ifstream is(argv[1]);
	if (!is) return EXIT_FAILURE;

	std::ofstream os(argv[2], std::ofstream::binary);
	if (!os) return EXIT_FAILURE;

	int16_t tmp;
	while (is >> tmp) {
		if (tmp < -1000 || tmp > 1000) break;
		raw_write(os, static_cast<int32_t>(tmp));
	}

	is.close();
	os.close();

	return EXIT_SUCCESS;
}