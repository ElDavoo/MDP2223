#include "pbm.h"
#include <iostream>
int main(int argc, char** argv) {
	if (argc != 2) return EXIT_FAILURE;

	BinaryImage bi;
	bool result = bi.ReadFromPBM(argv[1]);

	if (result) {
		std::cout << "W: " << bi.W << ", H: " << bi.H << ", W*H:" << bi.W * bi.H;
		std::cout << ", size: " << bi.ImageData.size() << std::endl;
	}

	Image im = BinaryImageToImage(bi);

	std::cout << "W: " << im.W << ", H: " << im.H << ", W*H:" << im.W * im.H;
	std::cout << ", size: " << im.ImageData.size() << std::endl;

	return result ? EXIT_SUCCESS : EXIT_FAILURE;
}