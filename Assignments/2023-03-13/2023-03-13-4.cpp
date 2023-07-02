#pragma warning(disable: 4710)

#include <fstream>
#include <iostream>

template<typename T>
std::ostream& raw_write(std::ostream &os, const T val, const size_t size = sizeof(T)) {
	return os.write((char*)(&val), static_cast<std::streamsize>(size));
}

class bitwriter {

	std::ostream &os_;
	uint8_t counter_ = 8;
	char byte_ = 0;

public:

	bitwriter(std::ostream &os) : os_(os) {
		if (os.fail()) throw std::invalid_argument("Errore: lo stream è in errore");
	}

	/* Ci ho provato...
	bitwriter(const char* name) {
		if (name == nullptr) throw std::invalid_argument("Errore durante l'apertura del file");
		auto oh = std::ofstream(name);
		os_ = *oh;

		if (!os_ || !os_->good()) {
			throw std::invalid_argument("Errore durante l'apertura del file");
		}

	}*/

	bool write_bit(const bool value) {
		
		byte_ = (byte_ << 1) | (value & 1);

		if (!--counter_) {
			counter_ = 8;
			return !raw_write(os_, byte_).fail();
		}

		return true;

	}

	bool write_uint16(const int16_t value, uint8_t size = 11) {

		if (value < -1000 || value > 1000) throw std::invalid_argument("Number outside range");

		if (size > 15) throw std::invalid_argument("Size outside range");

		while (size --> 0) {
			
			 return write_bit(value >> size & 1);

		}

		return true;
	}

	bool operator()(const int16_t value, const uint8_t size = 11) {
		return write_uint16(value, size);
	}

	~bitwriter() { 
		while (counter_ != 8) write_bit(false);
		os_.flush(); 
	}
};


int main(int argc, char* argv[]) {

	if (argc != 3) {
		std::cout << "Usage: " << argv << "<filein.txt> <fileout.bin>" << std::endl;
		return EXIT_FAILURE;
	}

	std::ifstream ifs(argv[1]);

	if (ifs.fail()) {
		std::cout << "Error opening input file" << std::endl;
		return EXIT_FAILURE;
	}

	std::ofstream ofs(argv[2]);

	if (ofs.fail()) {
		std::cout << "Error opening output file" << std::endl;
		return EXIT_FAILURE;
	}

	bitwriter bw(ofs);

	while (1) {

		int16_t num;

		ifs >> num;

		if (ifs.fail()) break;

		// Non è proprio bello mischiare controlli su ritorno ed eccezioni ma ok
		try {
			if (!bw(num)) {
				std::cout << "Errore durante la scrittura del file" << std::endl;
			}
		}
		catch (std::invalid_argument const& ex) {
			std::cout << ex.what() << std::endl;
		}
	}

	return EXIT_SUCCESS;

}