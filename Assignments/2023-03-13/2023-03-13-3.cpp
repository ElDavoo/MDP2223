#include <fstream>
#include <iostream>
#include <vector>

int main(int argc, char* argv[]) {
	
	if (argc != 3) {
		std::cout << "Usage: " << argv << "filein.txt fileout.bin" << std::endl;
		return EXIT_FAILURE;
	}

	std::ifstream ifs(argv[1], std::ios::binary);

	if (ifs.fail()) {
		std::cout << "Error opening input file" << std::endl;
		return EXIT_FAILURE;
	}

	std::ofstream ofs(argv[2]);

	if (ofs.fail()) {
		std::cout << "Error opening output file" << std::endl;
		return EXIT_FAILURE;
	}

	while (1) {

		int32_t num;
		ifs.read(reinterpret_cast<char *>(&num), sizeof(int32_t));
		if (ifs.gcount() != sizeof(int32_t)) break;

		ofs << num << std::endl;

	}

	ifs.close();
	ofs.close();

	return EXIT_SUCCESS;

}