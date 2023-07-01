#pragma warning(disable: 4710)

#include <fstream>
#include <iostream>

int main(int argc, char* argv[]) {

	if (argc != 3) {
		std::cout << "Usage: " << argv << "<filein.bin> <fileout.txt>" << std::endl;
		return EXIT_FAILURE;
	}

	std::ifstream ifs(argv[1]);
	
	if (ifs.fail()) {
		std::cout << "Error opening input file" << std::endl;
		return EXIT_FAILURE;
	}

	std::ofstream ofs(argv[2], std::ios::binary);

	if (ofs.fail()) {
		std::cout << "Error opening output file" << std::endl;
		return EXIT_FAILURE;
	}

	int32_t ui;

	while (1) {

		ifs >> ui;

		if (ifs.fail()) break;

		if (ui < -1000 || ui > 1000) continue;

		ofs.write(reinterpret_cast<char*>(&ui), sizeof(int32_t));

		if (ofs.fail()) {
			std::cout << "Error writing to output file" << std::endl;
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;

}